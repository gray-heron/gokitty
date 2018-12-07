#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Quick n dirty protocol tests"

#include "hinge_model.h"
#include <algorithm>
#include <boost/test/unit_test.hpp>

class TestSegment : public HingeModel::Segment
{
    void ComputeScoreThis(adept::adouble &score) const override {}
    void ApplyGradientThis() override {}
    void VisualiseThis(std::vector<Visualisation::Object> &objects) const override {}

  public:
    Vector<2, false> pos_;
    TestSegment(Vector<2, false> pos)
        : Segment(nullptr, false, SDL2pp::Color()), pos_(pos){};
    Vector<2, false> GetPosition() const override { return pos_; }
};

bool IntersectionTest(Vector<2, false> v11, Vector<2, false> v12, Vector<2, false> v21,
                      Vector<2, false> v22)
{
    TestSegment flat_left(v11);
    TestSegment flat_right(v12);
    flat_left.LinkForward(&flat_right);

    TestSegment vertical_left(v21);
    TestSegment vertical_right(v22);
    vertical_left.LinkForward(&vertical_right);

    return HingeModel::Segment::Intersects(&flat_left, &vertical_left);
}

BOOST_AUTO_TEST_CASE(IntersectionFunction)
{
    BOOST_CHECK(
        IntersectionTest({-1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, -1.0f}, {0.0f, 1.0f}));
    BOOST_CHECK(
        !IntersectionTest({-1.0f, 10.0f}, {1.0f, 10.0f}, {0.0f, -1.0f}, {0.0f, 1.0f}));
    BOOST_CHECK(IntersectionTest({93.0f, 75.0f}, {380.0f, 500.0f}, {15.0f, 280.0f},
                                 {750.0f, 300.0f}));
    BOOST_CHECK(!IntersectionTest({100.0f, 200.0f}, {200.0f, 300.0f}, {150.0f, 250.0f},
                                  {250.0f, 350.0f}));
};
