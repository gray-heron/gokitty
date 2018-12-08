
#pragma once

#include <adept_arrays.h>
#include <boost/any.hpp>

boost::any ParseValue(const std::type_info &type_id, std::string value);

template <int D1, bool Gradient>
using Vector = adept::FixedArray<adept::Real, Gradient, 1, D1>;

namespace util
{
double cross(const Vector<2, false> &v1, const Vector<2, false> &v2);

adept::adouble det33(const adept::aMatrix33 &m);

adept::adouble CircumcircleRadius(const Vector<2, true> &p1, const Vector<2, true> &p2,
                                  const Vector<2, true> &p3);
}
