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
    };

    class Segment : public ModelElement
    {
        void LinkBackward(Segment *previous);
        void Simulate() = 0;
        void VisualiseThis(std::vector<Visualisation::Object> &objects);

      protected:
        Segment *next_;
        Segment *previous_;
        adept::Vector position_;

      public:
        virtual ~Segment() = default;
        void LinkForward(Segment *next);
    };

    class Hinge : public Segment
    {
      private:
        void Simulate() override;

      public:
        Hinge(HingeModel *model, adept::Vector position);
    };

    class BandSegement : public Segment
    {
      private:
        void Simulate() override;

      public:
        BandSegement(HingeModel *model, adept::Vector position);
    };

  private:
    std::vector<std::vector<HingeCollisionZone *>> collision_zones_;
    std::pair<uint32_t, uint32_t> CoordinatesToCollisionZone(adept::Vector pos);

    uint32_t width_;
    uint32_t height_;
    float collision_zone_side_;

    void Simulate();
    void VisualiseThis(std::vector<Visualisation::Object> &objects) override;

    void AddHinge(Hinge *h);
    void AddBandSegment(BandSegement *bs);
    void PositionHinge(Hinge *h);

  public:
    HingeModel(uint32_t width, uint32_t height, float collision_zone_side);
};