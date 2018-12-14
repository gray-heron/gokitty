#include <boost/any.hpp>

#include "exceptions.h"
#include "util.h"

using std::string;

using namespace util;

boost::any ParseValue(const std::type_info &type_id, std::string value)
{
    if (type_id == typeid(string))
        return value;
    if (type_id == typeid(int))
        return std::stoi(value);
    if (type_id == typeid(float))
        return std::stof(value);
    if (type_id == typeid(double))
        return std::stod(value);
    if (type_id == typeid(bool))
    {
        if (value == "true")
            return true;
        else if (value == "false")
            return false;

        return static_cast<bool>(std::stoi(value));
    }

    throw Exception((string) "Unrecognized type: " + type_id.name());
}

adept::aReal util::det33(const adept::aMatrix33 &m)
{
    adept::aReal p1 = m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2);
    adept::aReal p2 = m(0, 1) * m(2, 2) - m(2, 1) * m(0, 2);
    adept::aReal p3 = m(0, 1) * m(1, 2) - m(1, 1) * m(0, 2);

    return m(0, 0) * p1 - m(1, 0) * p2 + m(2, 0) * p3;
}

double util::cross(const Vector<2, false> &v1, const Vector<2, false> &v2)
{
    return (v1(0, 0) * v2(0, 1)) - (v1(0, 1) * v2(0, 0));
}

template <typename T1, typename T2, typename T3>
adept::aMatrix33 fixme(T1 a, T2 b, T3 c, T1 d, T2 e, T3 f, T1 g, T2 h, T3 i)
{
    adept::aMatrix33 ret;
    ret(0, 0) = a;
    ret(0, 1) = b;
    ret(0, 2) = c;
    ret(1, 0) = d;
    ret(1, 1) = e;
    ret(1, 2) = f;
    ret(2, 0) = g;
    ret(2, 1) = h;
    ret(2, 2) = i;

    return ret;
}

adept::aReal util::CircumcircleRadius(const Vector<2, true> &p1,
                                      const Vector<2, true> &p2,
                                      const Vector<2, true> &p3)
{
    // http://mathworld.wolfram.com/Circumcircle.html

    // clang-format off
    adept::aMatrix aM = fixme(
        p1(0, 0), p1(0, 1), 1,
        p2(0, 0), p2(0, 1), 1,
        p3(0, 0), p3(0, 1), 1);
    adept::aReal a = det33(aM);

    adept::aReal r1 = p1(0, 0) * p1(0, 0) + p1(0, 1) * p1(0, 1);
    adept::aReal r2 = p2(0, 0) * p2(0, 0) + p2(0, 1) * p2(0, 1);
    adept::aReal r3 = p3(0, 0) * p3(0, 0) + p3(0, 1) * p3(0, 1);

    adept::aMatrix33 bxM = fixme(
            r1, p1(0, 1), 1.0,
            r2, p2(0, 1), 1.0,
            r3, p3(0, 1), 1.0);
    adept::aReal bx = det33(bxM);

    adept::aMatrix33 byM = fixme(
            r1, p1(0,0), 1.0,
            r2, p2(0,0), 1.0,
            r3, p3(0,0), 1.0);
    adept::aReal by = det33(byM);

    adept::aMatrix33 cM = fixme(
            r1, p1(0, 0), p1(0, 1),
            r2, p2(0, 0), p2(0, 1),
            r3, p3(0, 0), p3(0, 1));
    adept::aReal c = -det33(cM);

    // clang-format on

    return adept::sqrt(bx * bx + by * by - 4.0 * a * c) / (2.0 * adept::abs(a));
}