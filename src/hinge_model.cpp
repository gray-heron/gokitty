#include "hinge_model.h"
#include "config.h"
#include "exceptions.h"
#include "util.h"

using std::get;

HingeModel::HingeModel(uint32_t width, uint32_t height, float collision_zone_side)
    : width_(width), height_(height), collision_zone_side_(collision_zone_side),
      alpha_(Config::inst().GetOption<float>("alpha")), first_hinge_(nullptr)
{
    for (uint32_t x = 0; x < width; ++x)
    {
        collision_zones_.emplace_back();
        for (uint32_t y = 0; y < height; ++y)
        {
            auto collision_zone = new HingeCollisionZone(
                this, adept::Vector({x * collision_zone_side, y * collision_zone_side}),
                collision_zone_side);
            (collision_zones_.end() - 1)->push_back(collision_zone);
            AddChild(collision_zone);
        }
    }
}

std::pair<uint32_t, uint32_t> HingeModel::CoordinatesToCollisionZone(adept::Vector pos)
{

    std::pair<uint32_t, uint32_t> ret;

    ASSERT(pos[0] < collision_zone_side_ * float(width_));
    ASSERT(pos[1] < collision_zone_side_ * float(height_));

    ret.first = pos[0] / collision_zone_side_;
    ret.second = pos[1] / collision_zone_side_;

    return ret;
}

void HingeModel::AddHinge(Hinge *h)
{
    if (!first_hinge_)
        first_hinge_ = h;

    AddChild(h);
}

void HingeModel::AddBandSegment(BandSegement *bs) { AddChild(bs); }

void HingeModel::ComputeScoreThis(adept::adouble &score) const {}

void HingeModel::VisualiseThis(std::vector<Visualisation::Object> &objects) const {}

void HingeModel::ApplyGradientThis() {}

double HingeModel::Optimize(adept::Stack &stack)
{
    stack.new_recording();
    adept::adouble score = 0.0;
    ComputeScore(score);
    score.set_gradient(1.0);
    stack.reverse();
    ApplyGradient();

    log_.Info() << "Optimization step done, score = " << score;
    return score.value();
}

// ================ COLLISION_ZONE ================

HingeModel::HingeCollisionZone::HingeCollisionZone(HingeModel *model,
                                                   adept::Vector position, float side)
    : position_(position), collision_zone_side_(side)
{
}

void HingeModel::HingeCollisionZone::ComputeScoreThis(adept::adouble &score) const {}

void HingeModel::HingeCollisionZone::VisualiseThis(
    std::vector<Visualisation::Object> &objects) const
{
    objects.push_back(Visualisation::Object(
        {position_, position_ + adept::Vector({collision_zone_side_, 0.0f})},
        SDL2pp::Color(32, 32, 32)));
    objects.push_back(Visualisation::Object(
        {position_, position_ + adept::Vector({0.0f, collision_zone_side_})},
        SDL2pp::Color(32, 32, 32)));
    // objects.push_back(Visualisation::Object(
    //    {position_ + adept::Vector(collision_zone_side_, collision_zone_side_),
    //     position_ + adept::Vector(collision_zone_side_, 0)},
    //    SDL2pp::Color(32, 32, 32)));
    // objects.push_back(Visualisation::Object(
    //    {position_ + adept::Vector(collision_zone_side_, collision_zone_side_),
    //     position_ + adept::Vector(0, collision_zone_side_)},
    //    SDL2pp::Color(32, 32, 32)));
}

void HingeModel::HingeCollisionZone::ApplyGradientThis() {}

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

HingeModel::Hinge::Hinge(HingeModel *model, adept::Vector position)
    : HingeModel::Segment(model, false, SDL2pp::Color(0, 255, 0)), position_(position)
{
    model->AddHinge(this);
}

void HingeModel::Hinge::ComputeScoreThis(adept::adouble &score) const
{
    auto position_diff = (static_cast<Hinge *>(next_)->position_ - position_);
    score += adept::sqrt(adept::sum(position_diff * position_diff));
}

adept::Vector HingeModel::Hinge::GetPosition() const
{
    return adept::aVector(position_).inactive_link();
}

void HingeModel::Hinge::ApplyGradientThis()
{
    adept::Vector new_position =
        position_.inactive_link() - position_.get_gradient() * model_->alpha_;

    auto from_collsion_zone = model_->CoordinatesToCollisionZone(GetPosition());
    auto to_collsion_zone = model_->CoordinatesToCollisionZone(GetPosition());
    for (int czx = from_collsion_zone.first; czx < to_collsion_zone.first + 1; czx++)
    {
        for (int czy = from_collsion_zone.second; czy < to_collsion_zone.second + 1;
             czy++)
        {
            if (model_->collision_zones_[czx][czy]->Collides(this))
            {
                return;
            }
        }
    }

    position_ = new_position;
}

// ================ BAND_SEGMENT ================

HingeModel::BandSegement::BandSegement(HingeModel *model, adept::Vector position)
    : HingeModel::Segment(model, true, SDL2pp::Color(255, 0, 0)), position_(position)
{
    model->AddBandSegment(this);
}

void HingeModel::BandSegement::ComputeScoreThis(adept::adouble &score) const {}

void HingeModel::BandSegement::ApplyGradientThis() {}

adept::Vector HingeModel::BandSegement::GetPosition() const { return position_; }

// ================ SEGMENT ================

HingeModel::Segment::Segment(HingeModel *model, bool solid, SDL2pp::Color vis_color)
    : model_(model), solid_(solid), vis_color_(vis_color)
{
}

void HingeModel::Segment::VisualiseThis(std::vector<Visualisation::Object> &objects) const
{
    auto o =
        Visualisation::Object({this->GetPosition(), next_->GetPosition()}, vis_color_);

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
    adept::Vector p = s1->GetPosition();
    adept::Vector r = s1->next_->GetPosition() - s1->GetPosition();
    adept::Vector q = s2->GetPosition();
    adept::Vector s = s2->next_->GetPosition() - s2->GetPosition();

    double t = cross(q - p, s) / cross(r, s);
    double u = cross(q - p, r) / cross(r, s);

    return std::abs(cross(r, s)) > 0.001 && 0.0 <= t && t <= 1.0 && 0.0 <= u && u <= 1.0;
}