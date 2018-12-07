
#pragma once

#include <adept_arrays.h>
#include <boost/any.hpp>

boost::any ParseValue(const std::type_info &type_id, std::string value);

template <int D1, bool Gradient>
using Vector = adept::FixedArray<adept::Real, Gradient, D1>;

namespace util
{
double cross(const Vector<2, false> &v1, const Vector<2, false> &v2);

adept::Scalar<adept::aReal> CircumcircleRadius(const adept::aVector &p1,
                                               const adept::aVector &p2,
                                               const adept::aVector &p3);
}
