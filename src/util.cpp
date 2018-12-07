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

double util::cross(const Vector<2, false> &v1, const Vector<2, false> &v2)
{
    return (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

adept::Scalar<adept::aReal> CircumcircleRadius(const adept::aVector &p1,
                                               const adept::aVector &p2,
                                               const adept::aVector &p3)
{
    // http://mathworld.wolfram.com/Circumcircle.html
}