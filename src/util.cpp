#include <boost/any.hpp>

#include "exceptions.h"
#include "util.h"

using std::string;

torch::Tensor TFV(float x, float y)
{
    auto ret = torch::ones({2, 1});
    ret[0][0] = x;
    ret[1][0] = y;

    return ret;
}

torch::Tensor TFS(float x)
{
    auto ret = torch::ones({1, 1});
    ret[0][0] = x;

    return ret;
}

float S(torch::Tensor t) { return t._local_scalar().toDouble(); }

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