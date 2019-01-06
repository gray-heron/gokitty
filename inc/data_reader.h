
#pragma once

#include <pugixml.hpp>

#include "hinge_model.h"
#include "log.h"

class DataReader
{
  public:
    static void ReadTORCSTrack(std::string xml_path, HingeModel &model,
                               Vector<2, false> startpoint);

    static void SaveHingeModel(std::string target_path, const HingeModel &model);
    static void ReadHingeModel(std::string target_path, HingeModel &model);
};

class TrackSaver
{
    pugi::xml_document doc_;
    pugi::xml_node root_node_;
    const std::string track_name_;

    double heading_, x_, y_;

    Vector<2, false> v1_ = {{0.0, 0.0}};
    int waypoint_sep_ = 0;

    std::vector<Visualisation::Object> objects_;

  public:
    TrackSaver(std::string track_name);
    ~TrackSaver();
    void MarkWaypoint(float forward, float l, float r, float angle, float speed);

    void Visualise(std::vector<Visualisation::Object> &objects) const;
};