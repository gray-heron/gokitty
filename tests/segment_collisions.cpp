#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Segment collision tests"

#include "hinge_model.h"
#include <algorithm>
#include <boost/test/unit_test.hpp>

class TestSegment : public HingeModel::Segment
{
    void SetupEquationsThis() override {}
    void ComputeScoreThis(adept::aReal &score) const override {}
    void ApplyGradientThis(double) override {}
    void VisualiseThis(std::vector<Visualisation::Object> &objects) const override {}

    std::string GetTooltip() const override { return "mock segment"; };

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
    BOOST_CHECK(IntersectionTest({{-1.0f, 0.0f}}, {{1.0f, 0.0f}}, {{0.0f, -1.0f}},
                                 {{0.0f, 1.0f}}));
    BOOST_CHECK(!IntersectionTest({{-1.0f, 10.0f}}, {{1.0f, 10.0f}}, {{0.0f, -1.0f}},
                                  {{0.0f, 1.0f}}));
    BOOST_CHECK(IntersectionTest({{93.0f, 75.0f}}, {{380.0f, 500.0f}}, {{15.0f, 280.0f}},
                                 {{750.0f, 300.0f}}));
    BOOST_CHECK(!IntersectionTest({{100.0f, 200.0f}}, {{200.0f, 300.0f}},
                                  {{150.0f, 250.0f}}, {{250.0f, 350.0f}}));
};

BOOST_AUTO_TEST_CASE(CircumcircleRadius)
{
    adept::Stack stack;

    Vector<2, true> p1({{-2.0, 0.0}});
    Vector<2, true> p2({{0.0, 2.0}});
    Vector<2, true> p3({{2.0, 0.0}});

    stack.new_recording();
    adept::aReal r = util::CircumcircleRadius(p1, p2, p3);
    r.set_gradient(1.0);
    stack.reverse();

    BOOST_CHECK_CLOSE(r.value(), 2.0, 0.0001);
};
