
#pragma once

#include "hinge_model.h"
#include "log.h"

class DataReader
{
  public:
    static void ReadTORCSTrack(std::string xml_path, HingeModel &model);
};
