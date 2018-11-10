
#include <pugixml.hpp>

#include "config.h"

bool Config::Load(std::string config)
{
    pugi::xml_document doc;
    doc.load_string(config.c_str());
    return true;
}
