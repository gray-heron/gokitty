
#pragma once

#include <adept_arrays.h>
#include <boost/any.hpp>

boost::any ParseValue(const std::type_info &type_id, std::string value);