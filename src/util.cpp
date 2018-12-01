#include <boost/any.hpp>

#include "exceptions.h"
#include "util.h"

using std::string;

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