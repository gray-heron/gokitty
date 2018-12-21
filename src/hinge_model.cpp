#include "hinge_model.h"
#include "config.h"
#include "exceptions.h"
#include "util.h"

using std::get;

HingeModel::HingeModel(uint32_t width, uint32_t height, float collision_zone_side)
    : width_(width), height_(height), collision_zone_side_(collision_zone_side),
      alpha_(Config::inst().GetOption<float>("alpha")), first_hinge_(nullptr),
      max_centrifugal_force_(Config::inst().GetOption<float>("max_centrifugal_force")),
      max_acceleration_(Config::inst().GetOption<float>("max_acceleration"))
{
    for (uint32_t x = 0; x < width; ++x)
    {
        collision_zones_.emplace_back();
        for (uint32_t y = 0; y < height; ++y)
        {
            auto collision_zone = new HingeCollisionZone(
                this,
                Vector<2, false>({{x * collision_zone_side, y * collision_zone_side}}),
                collision_zone_side);
            (collision_zones_.end() - 1)->push_back(collision_zone);
            AddChild(collision_zone);
        }
    }
}

std::pair<int, int> HingeModel::CoordinatesToCollisionZone(Vector<2, false> pos)
{
    std::pair<int, int> ret;

    // ASSERT(pos(0, 0) < collision_zone_side_ * float(width_));
    // ASSERT(pos(0, 1) < collision_zone_side_ * float(height_));

    ret.first = pos(0, 0) / collision_zone_side_;
    ret.second = pos(0, 1) / collision_zone_side_;

    return ret;
}

void HingeModel::AddHinge(Hinge *h)
{
    if (!first_hinge_)
        first_hinge_ = h;

    AddChild(h);
}

void HingeModel::AddBandSegment(BandSegement *bs) { AddChild(bs); }

void HingeModel::SetupEquationsThis() {}

void HingeModel::ComputeScoreThis(adept::aReal &score) const {}

void HingeModel::VisualiseThis(std::vector<Visualisation::Object> &objects) const {}

void HingeModel::ApplyGradientThis(double score_normalization) {}

double HingeModel::Optimize(adept::Stack &stack)
{
    double score_normalization = 1.0;
    if (first_last_score_)
        score_normalization = first_last_score_->first / first_last_score_->second;

    adept::aReal score = 0.0;
    stack.new_recording();

    SetupEquations();
    ComputeScore(score);

    score.set_gradient(1.0); // could use this for score_normalization
    stack.reverse();
    // ApplyGradient(score_normalization);
    ApplyGradient(1.0);

    log_.Info() << "Optimization step done, score = " << score.value()
                << ", normalization = " << score_normalization;

    if (first_last_score_)
        first_last_score_ =
            std::pair<double, double>(first_last_score_->first, (double)score.value());
    else
        first_last_score_ = std::pair<double, double>(score.value(), score.value());

    return score.value();
}

// ================ COLLISION_ZONE ================

HingeModel::HingeCollisionZone::HingeCollisionZone(HingeModel *model,
                                                   Vector<2, false> position, float side)
    : position_(position), collision_zone_side_(side)
{
}

void HingeModel::HingeCollisionZone::ComputeScoreThis(adept::aReal &score) const {}

void HingeModel::HingeCollisionZone::VisualiseThis(
    std::vector<Visualisation::Object> &objects) const
{
    // objects.push_back(Visualisation::Object(
    //    {position_, position_ + Vector<2, false>({collision_zone_side_, 0.0f})},
    //    SDL2pp::Color(32, 32, 32)));
    // objects.push_back(Visualisation::Object(
    //    {position_, position_ + Vector<2, false>({0.0f, collision_zone_side_})},
    //    SDL2pp::Color(32, 32, 32)));
    // objects.push_back(Visualisation::Object(
    //    {position_ + Vector<2, false>(collision_zone_side_, collision_zone_side_),
    //     position_ + Vector<2, false>(collision_zone_side_, 0)},
    //    SDL2pp::Color(32, 32, 32)));
    // objects.push_back(Visualisation::Object(
    //    {position_ + Vector<2, false>(collision_zone_side_, collision_zone_side_),
    //     position_ + Vector<2, false>(0, collision_zone_side_)},
    //    SDL2pp::Color(32, 32, 32)));
}

void HingeModel::HingeCollisionZone::SetupEquationsThis() {}

void HingeModel::HingeCollisionZone::ApplyGradientThis(double score_normalization) {}

bool HingeModel::HingeCollisionZone::Collides(Segment *seg) const
{
    for (const auto &s : segments_)
    {
        if (HingeModel::Segment::Intersects(s, seg))
            return true;
    }

    return false;
}

void HingeModel::HingeCollisionZone::RegisterSegment(HingeModel::Segment *segment)
{
    segments_.insert(segment);
}

// ================ HINGE ================

HingeModel::Hinge::Hinge(HingeModel *model, Vector<2, false> position)
    : HingeModel::Segment(model, false, SDL2pp::Color(0, 255, 0)),
      zero_position_(position), position_(position), crossposition_(0.0f), speed_(10.0)
{
    model->AddHinge(this);
}

void HingeModel::Hinge::SetupEquationsThis()
{
    position_ = zero_position_ + crossposition_vector_ * crossposition_;
}

void HingeModel::Hinge::ComputeScoreThis(adept::aReal &score) const
{
    Vector<2, true> position_diff_n =
        (static_cast<Hinge *>(next_)->position_ - position_);
    Vector<2, true> position_diff_p =
        (position_ - static_cast<Hinge *>(previous_)->position_);

    adept::aReal len =
        (adept::norm2(position_diff_n) + adept::norm2(position_diff_p)) / 2.0;

    adept::aReal r =
        util::CircumcircleRadius(static_cast<Hinge *>(next_)->position_, position_,
                                 static_cast<Hinge *>(previous_)->position_);

    adept::aReal centrifugal_force = (speed_ * speed_) / r;

    score += -1.0 / (centrifugal_force - model_->max_centrifugal_force_);
    score += -1.0 / (adept::abs(speed_ - static_cast<Hinge *>(next_)->speed_) /
                         adept::norm2(position_diff_n) -
                     model_->max_acceleration_);

    last_centrifugal_force_ = centrifugal_force.value();

    score += len / speed_;
}

void HingeModel::Hinge::ApplyGradientThis(double score_normalization)
{
    speed_ -= speed_.get_gradient() * model_->alpha_ * score_normalization * 3.0;
    crossposition_ -=
        crossposition_.get_gradient() * model_->alpha_ * score_normalization;
    if (adept::abs(crossposition_) > 1.0)
        crossposition_ /= adept::abs(crossposition_);

    vis_color_ = SpeedToColor(speed_.value());

    /*
    auto from_collsion_zone = model_->CoordinatesToCollisionZone(GetPosition());
    auto to_collsion_zone = model_->CoordinatesToCollisionZone(GetPosition());

    Vector<2, true> old_position = Vector<2, true>(position_);
        if (from_collsion_zone.first < 0 || to_collsion_zone.first >= model_->width_ ||
            from_collsion_zone.second < 0 || to_collsion_zone.second >= model_->height_)
        {
            model_->log_.Error() << "Hinge overflows the board";
            return;
        }

        for (int czx = from_collsion_zone.first; czx < to_collsion_zone.first + 1; czx++)
        {
            for (int czy = from_collsion_zone.second; czy < to_collsion_zone.second + 1;
                 czy++)
            {
                if (model_->collision_zones_[czx][czy]->Collides(this) ||
                    model_->collision_zones_[czx][czy]->Collides(previous_))
                {
                    position_ = old_position;
                    return;
                }
            }
        }
    */
}

void HingeModel::Hinge::LinkForward(Segment *next)
{
    Segment::LinkForward(next);
    Vector<2, true> heading = next_->GetPosition() - position_;
    heading /= adept::norm2(heading);
    crossposition_vector_ =
        Vector<2, false>({{-heading[0][1].value(), heading[0][0].value()}}) * 6.4;

    position_ = zero_position_ + crossposition_vector_ * crossposition_;
}

std::string HingeModel::Hinge::GetTooltip() const
{
    std::stringstream ret;
    ret << "Hinge | position = " << position_;
    ret << ", speed = " << speed_;
    ret << ", centrifugal_force = " << last_centrifugal_force_;

    return ret.str();
}

Vector<2, false> HingeModel::Hinge::GetPosition() const
{
    return Vector<2, true>(position_).inactive_link();
}

SDL2pp::Color HingeModel::Hinge::SpeedToColor(double speed)
{
    if (speed > 50.0)
        return SDL2pp::Color(0, 255, 0);
    else if (speed < 0.0)
        return SDL2pp::Color(0, 63, 0);
    else
        return SDL2pp::Color(0, speed / 50.0 * 255.0, 0);
}

// ================ BAND_SEGMENT ================

HingeModel::BandSegement::BandSegement(HingeModel *model, Vector<2, false> position)
    : HingeModel::Segment(model, true, SDL2pp::Color(255, 0, 0)), position_(position)
{
    model->AddBandSegment(this);
}

void HingeModel::BandSegement::SetupEquationsThis() {}

void HingeModel::BandSegement::ComputeScoreThis(adept::aReal &score) const {}

void HingeModel::BandSegement::ApplyGradientThis(double score_normalization) {}

std::string HingeModel::BandSegement::GetTooltip() const { return "Band segment"; }

Vector<2, false> HingeModel::BandSegement::GetPosition() const { return position_; }

// ================ SEGMENT ================

HingeModel::Segment::Segment(HingeModel *model, bool solid, SDL2pp::Color vis_color)
    : model_(model), solid_(solid), vis_color_(vis_color)
{
}

void HingeModel::Segment::VisualiseThis(std::vector<Visualisation::Object> &objects) const
{
    auto o = Visualisation::Object(this->GetPosition(), next_->GetPosition(), this,
                                   vis_color_);

    objects.push_back(o);
}

void HingeModel::Segment::LinkForward(HingeModel::Segment *next)
{
    next_ = next;
    next->LinkBackward(this);

    if (solid_)
    {
        auto collison_zone_coords = model_->CoordinatesToCollisionZone(GetPosition());
        model_->collision_zones_[collison_zone_coords.first][collison_zone_coords.second]
            ->RegisterSegment(this);
    }
}

void HingeModel::Segment::LinkBackward(HingeModel::Segment *previous)
{
    previous_ = previous;
}

bool HingeModel::Segment::Intersects(const Segment *s1, const Segment *s2)
{
    using namespace util;
    // https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
    Vector<2, false> p = s1->GetPosition();
    Vector<2, false> r = s1->next_->GetPosition() - s1->GetPosition();
    Vector<2, false> q = s2->GetPosition();
    Vector<2, false> s = s2->next_->GetPosition() - s2->GetPosition();

    double t = cross(q - p, s) / cross(r, s);
    double u = cross(q - p, r) / cross(r, s);

    return std::abs(cross(r, s)) > 0.001 && 0.0 <= t && t <= 1.0 && 0.0 <= u && u <= 1.0;
}