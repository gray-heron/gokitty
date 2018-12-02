#include "hinge_model.h"
#include "exceptions.h"
#include "util.h"

using std::get;

HingeModel::HingeModel(uint32_t width, uint32_t height, float collision_zone_side)
    : width_(width), height_(height), collision_zone_side_(collision_zone_side),
      first_hinge_(nullptr)
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

void HingeModel::Simulate() {}
void HingeModel::VisualiseThis(std::vector<Visualisation::Object> &objects) {}

HingeModel::HingeCollisionZone::HingeCollisionZone(HingeModel *model,
                                                   adept::Vector position, float side)
    : position_(position), collision_zone_side_(side)
{
}

void HingeModel::HingeCollisionZone::Simulate() {}
void HingeModel::HingeCollisionZone::VisualiseThis(
    std::vector<Visualisation::Object> &objects)
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

HingeModel::Hinge::Hinge(HingeModel *model, adept::Vector position)
    : HingeModel::Segment(model, SDL2pp::Color(0, 255, 0)), position_(position)
{
    model->AddHinge(this);
}

void HingeModel::Hinge::Simulate() {}

adept::Vector HingeModel::Hinge::GetPosition() { return position_.inactive_link(); }

HingeModel::BandSegement::BandSegement(HingeModel *model, adept::Vector position)
    : HingeModel::Segment(model, SDL2pp::Color(255, 0, 0)), position_(position)
{
    model->AddBandSegment(this);
}

void HingeModel::BandSegement::Simulate() {}

adept::Vector HingeModel::BandSegement::GetPosition()
{
    return position_.inactive_link();
}

HingeModel::Segment::Segment(HingeModel *model, SDL2pp::Color vis_color)
    : model_(model), vis_color_(vis_color)
{
}

void HingeModel::Segment::VisualiseThis(std::vector<Visualisation::Object> &objects)
{
    auto o =
        Visualisation::Object({this->GetPosition(), next_->GetPosition()}, vis_color_);

    objects.push_back(o);
}

void HingeModel::Segment::LinkForward(HingeModel::Segment *next)
{
    next_ = next;
    next->LinkBackward(this);
    model_->CoordinatesToCollisionZone(GetPosition());
}

void HingeModel::Segment::LinkBackward(HingeModel::Segment *previous)
{
    previous_ = previous;
}