#pragma once

#include "model_element.h"
#include "util.h"

#include <adept_arrays.h>
#include <set>

class HingeModel : public ModelElement
{
  public:
    class Segment;

    class HingeCollisionZone : public ModelElement
    {
      private:
        void ComputeScoreThis(adept::adouble &score) const override;
        void ApplyGradientThis() override;
        void VisualiseThis(std::vector<Visualisation::Object> &objects) const override;

        Vector<2, false> position_;
        float collision_zone_side_;

        std::set<Segment *> segments_;

      public:
        HingeCollisionZone(HingeModel *model, Vector<2, false> position_, float side);
        bool Collides(Segment *seg) const;
        void RegisterSegment(HingeModel::Segment *segment_);
    };

    class Segment : public ModelElement
    {
        void LinkBackward(Segment *previous);
        void VisualiseThis(std::vector<Visualisation::Object> &objects) const override;

      protected:
        Segment *next_;
        Segment *previous_;

        HingeModel *model_;
        const bool solid_;
        SDL2pp::Color vis_color_;

        Segment(HingeModel *model, bool solid, SDL2pp::Color vis_color);

      public:
        virtual ~Segment() = default;
        virtual void LinkForward(Segment *next);
        static bool Intersects(const Segment *s1, const Segment *s2);
        virtual Vector<2, false> GetPosition() const = 0;
    };

    class Hinge : public Segment
    {
      private:
        void ComputeScoreThis(adept::adouble &score) const override;
        void ApplyGradientThis() override;

        const Vector<2, false> zero_position_;
        Vector<2, true> position_;
        adept::adouble crossposition_;
        Vector<2, false> crossposition_vector_;

      public:
        Hinge(HingeModel *model, Vector<2, false> position);
        void LinkForward(Segment *next) override;
        Vector<2, false> GetPosition() const override;
    };

    class BandSegement : public Segment
    {
      private:
        void ComputeScoreThis(adept::adouble &score) const override;
        void ApplyGradientThis() override;
        Vector<2, false> position_;

      public:
        BandSegement(HingeModel *model, Vector<2, false> position);
        Vector<2, false> GetPosition() const override;
    };

  private:
    std::vector<std::vector<HingeCollisionZone *>> collision_zones_;
    std::pair<uint32_t, uint32_t> CoordinatesToCollisionZone(Vector<2, false> pos);

    uint32_t width_;
    uint32_t height_;
    float collision_zone_side_;
    float alpha_;

    Hinge *first_hinge_;

    void ComputeScoreThis(adept::adouble &score) const override;
    void ApplyGradientThis() override;
    void VisualiseThis(std::vector<Visualisation::Object> &objects) const override;

    void AddHinge(Hinge *h);
    void AddBandSegment(BandSegement *bs);
    void PositionHinge(Hinge *h);

    Log log_{"HingeModel"};

  public:
    HingeModel(uint32_t width, uint32_t height, float collision_zone_side);

    double Optimize(adept::Stack &stack);
};