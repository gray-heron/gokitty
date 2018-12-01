#include "hinge_model.h"
#include "exceptions.h"
#include "util.h"

using std::get;

HingeModel::HingeModel(uint32_t width, uint32_t height, float collision_zone_side)
    : width_(width), height_(height), collision_zone_side_(collision_zone_side)
{
    for (uint32_t x = 0; x < width; ++x)
    {
        collision_zones_.emplace_back();
        for (uint32_t y = 0; y < height; ++y)
        {
            auto collision_zone = new HingeCollisionZone(
                this, TFV(x * collision_zone_side, y * collision_zone_side),
                collision_zone_side);
            (collision_zones_.end() - 1)->push_back(collision_zone);
            AddChild(collision_zone);
        }
    }
}

std::pair<uint32_t, uint32_t> HingeModel::CoordinatesToCollisionZone(torch::Tensor pos)
{
    std::pair<uint32_t, uint32_t> ret;

    ASSERT(S(pos[0][0]) < collision_zone_side_ * float(width_));
    ASSERT(S(pos[1][0]) < collision_zone_side_ * float(height_));

    ret.first = S(pos[0][0]) / collision_zone_side_;
    ret.second = S(pos[1][0]) / collision_zone_side_;

    return ret;
}

void HingeModel::AddHinge(Hinge *h) { AddChild(h); }

void HingeModel::AddBandSegment(BandSegement *bs) { AddChild(bs); }

void HingeModel::Simulate() {}
void HingeModel::VisualiseThis(std::vector<Visualisation::Object> &objects) {}

HingeModel::HingeCollisionZone::HingeCollisionZone(HingeModel *model,
                                                   torch::Tensor position, float side)
    : position_(position), collision_zone_side_(side)
{
}

void HingeModel::HingeCollisionZone::Simulate() {}
void HingeModel::HingeCollisionZone::VisualiseThis(
    std::vector<Visualisation::Object> &objects)
{
    objects.push_back(
        Visualisation::Object({position_, position_ + TFV(collision_zone_side_, 0)},
                              SDL2pp::Color(32, 32, 32)));
    objects.push_back(
        Visualisation::Object({position_, position_ + TFV(0, collision_zone_side_)},
                              SDL2pp::Color(32, 32, 32)));
    objects.push_back(Visualisation::Object(
        {position_ + TFV(collision_zone_side_, collision_zone_side_),
         position_ + TFV(collision_zone_side_, 0)},
        SDL2pp::Color(32, 32, 32)));
    objects.push_back(Visualisation::Object(
        {position_ + TFV(collision_zone_side_, collision_zone_side_),
         position_ + TFV(0, collision_zone_side_)},
        SDL2pp::Color(32, 32, 32)));
}

HingeModel::Hinge::Hinge(HingeModel *model, torch::Tensor position)
{
    model->AddHinge(this);
}

void HingeModel::Hinge::Simulate() {}

HingeModel::BandSegement::BandSegement(HingeModel *model, torch::Tensor position)
{
    position_ = position;
    model->AddBandSegment(this);
}

void HingeModel::BandSegement::Simulate() {}

void HingeModel::Segment::VisualiseThis(std::vector<Visualisation::Object> &objects)
{
    auto o = Visualisation::Object(
        {
            this->position_, next_->position_,
        },
        SDL2pp::Color(255, 0, 0));

    objects.push_back(o);
}

void HingeModel::Segment::LinkForward(HingeModel::Segment *next)
{
    next_ = next;
    next->LinkBackward(this);
}

void HingeModel::Segment::LinkBackward(HingeModel::Segment *previous)
{
    previous_ = previous;
}