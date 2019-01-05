
#include <math.h>
#include <pugixml.hpp>

#include "config.h"
#include "data_reader.h"
#include "exceptions.h"
#include "util.h"

using namespace pugi;
using std::to_string;

void DataReader::ReadTORCSTrack(std::string xml_path, HingeModel &model,
                                Vector<2, false> startpoint)
{
    Log log{"DataReader"};
    log.Info() << "Begin track reading.";

    pugi::xml_document doc;
    if (!doc.load_file(xml_path.c_str()))
        throw Exception("Couldn't parse track!");

    const auto angle_factor = Config::inst().GetOption<float>("angle_factor");
    const auto bound_factor = Config::inst().GetOption<float>("bound_factor");
    const auto forward_factor = Config::inst().GetOption<float>("forward_factor");
    const auto band_separation = Config::inst().GetOption<float>("band_separation");

    float x = startpoint(0, 0), y = startpoint(0, 1);
    float heading = M_PI_2 / 2.0f;
    float fuse = 0.0f;
    float forward_total = 0.0f;
    int hinges_n = 0;

    HingeModel::BandSegement *first_left_band = nullptr, *first_right_band = nullptr,
                             *last_left_band = nullptr, *last_right_band = nullptr;

    HingeModel::Hinge *last_hinge = nullptr, *first_hinge = nullptr;

    for (auto child : doc.root().child("track").children())
    {
        float forward = child.child("forward").text().as_float();
        float angle = child.child("angle").text().as_float();
        float left = child.child("left").text().as_float();
        float right = child.child("right").text().as_float();

        x += std::cos(heading) * forward_factor * forward;
        y += std::sin(heading) * forward_factor * forward;
        heading += angle * angle_factor;

        if (fuse > band_separation)
        {
            ASSERT((std::string)child.name() == "waypoint");

            float lx = x + std::cos(heading + M_PI / 2.0f) * left * bound_factor;
            float ly = y + std::sin(heading + M_PI / 2.0f) * left * bound_factor;

            float rx = x + std::cos(heading - M_PI / 2.0f) * right * bound_factor;
            float ry = y + std::sin(heading - M_PI / 2.0f) * right * bound_factor;

            auto left_band =
                new HingeModel::BandSegement(&model, Vector<2, false>({{lx, ly}}));
            auto right_band =
                new HingeModel::BandSegement(&model, Vector<2, false>({{rx, ry}}));
            auto hinge = new HingeModel::Hinge(
                &model, Vector<2, false>({{(rx + lx) / 2.0f, (ry + ly) / 2.0f}}),
                (left + right) / 2.0, forward_total);

            if (!first_left_band && !first_right_band)
            {
                first_left_band = left_band;
                first_right_band = right_band;
                first_hinge = hinge;
            }
            else
            {
                last_left_band->LinkForward(left_band);
                last_right_band->LinkForward(right_band);
                last_hinge->LinkForward(hinge);
            }

            last_left_band = left_band;
            last_right_band = right_band;
            last_hinge = hinge;
            fuse = forward;

            hinges_n += 1;
        }
        else
        {
            fuse += forward;
        }

        forward_total += forward;
    }

    // last_left_band->LinkForward(first_left_band);
    // last_right_band->LinkForward(first_right_band);
    // last_hinge->LinkForward(first_hinge);

    log.Info() << "Track reading done. " << hinges_n << " hinges produced.";
}

TrackSaver::TrackSaver(std::string track_name)
    : track_name_(track_name), heading_(0.0), x_(0.0), y_(0.0)
{
    root_node_ = doc_.append_child("track");
}

void TrackSaver::MarkWaypoint(float f, float l, float r, float angle, float speed)
{
    auto w = root_node_.append_child("waypoint");
    w.append_child("forward")
        .append_child(pugi::node_pcdata)
        .set_value(to_string(f).c_str());
    w.append_child("left").append_child(node_pcdata).set_value(to_string(l).c_str());
    w.append_child("right").append_child(node_pcdata).set_value(to_string(r).c_str());
    w.append_child("angle").append_child(node_pcdata).set_value(to_string(angle).c_str());

    const auto angle_factor = Config::inst().GetOption<float>("angle_factor");
    const auto forward_factor = Config::inst().GetOption<float>("forward_factor");

    x_ += std::cos(heading_) * forward_factor * f;
    y_ += std::sin(heading_) * forward_factor * f;

    if (waypoint_sep_++ % 120 == 0)
    {
        auto v2 = Vector<2, false>({{x_, y_}});

        objects_.push_back(
            Visualisation::Object(v1_, v2, nullptr, SDL2pp::Color(255, 255, 255)));

        v1_ = v2;
    }

    heading_ += angle * angle_factor;
}

void TrackSaver::Visualise(std::vector<Visualisation::Object> &objects) const
{
    std::copy(objects_.begin(), objects_.end(), std::back_inserter(objects));
}

TrackSaver::~TrackSaver()
{
    doc_.save_file(track_name_.c_str());
    Log("TrackSaver").Info() << "Track saved to: " << track_name_;
}
