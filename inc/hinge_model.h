#pragma once

#include "model_element.h"
#include <adept_arrays.h>
#include <set>

class HingeModel : public ModelElement
{
  public:
    class Segment;

    class HingeCollisionZone : public ModelElement
    {
      private:
        void Simulate() override;
        void VisualiseThis(std::vector<Visualisation::Object> &objects) override;

        adept::Vector position_;
        float collision_zone_side_;

        std::set<Segment *> segments_;

      public:
        HingeCollisionZone(HingeModel *model, adept::Vector position_, float side);
        void RegisterSegment(HingeModel::Segment *segment_);
    };

    class Segment : public ModelElement
    {
        void LinkBackward(Segment *previous);
        void Simulate() = 0;
        void VisualiseThis(std::vector<Visualisation::Object> &objects);
        virtual adept::Vector GetPosition() = 0;

      protected:
        Segment *next_;
        Segment *previous_;

        HingeModel *model_;
        SDL2pp::Color vis_color_;

        Segment(HingeModel *model, SDL2pp::Color vis_color);

      public:
        virtual ~Segment() = default;
        void LinkForward(Segment *next);
    };

    class Hinge : public Segment
    {
      private:
        void Simulate() override;
        adept::aVector position_;

      public:
        Hinge(HingeModel *model, adept::Vector position);
        adept::Vector GetPosition() override;
    };

    class BandSegement : public Segment
    {
      private:
        void Simulate() override;
        adept::Vector position_;

      public:
        BandSegement(HingeModel *model, adept::Vector position);
        adept::Vector GetPosition() override;
    };

  private:
    std::vector<std::vector<HingeCollisionZone *>> collision_zones_;
    std::pair<uint32_t, uint32_t> CoordinatesToCollisionZone(adept::Vector pos);

    uint32_t width_;
    uint32_t height_;
    float collision_zone_side_;

    Hinge *first_hinge_;

    void Simulate() override;
    void VisualiseThis(std::vector<Visualisation::Object> &objects) override;

    void AddHinge(Hinge *h);
    void AddBandSegment(BandSegement *bs);
    void PositionHinge(Hinge *h);

  public:
    HingeModel(uint32_t width, uint32_t height, float collision_zone_side);
};