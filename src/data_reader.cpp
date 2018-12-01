
#include <math.h>
#include <pugixml.hpp>

#include "config.h"
#include "data_reader.h"
#include "exceptions.h"
#include "util.h"

void DataReader::ReadTORCSTrack(std::string xml_path, HingeModel &model)
{
    pugi::xml_document doc;
    if (!doc.load_file(xml_path.c_str()))
        throw Exception("Couldn't parse track!");

    const auto angle_factor = Config::inst().GetOption<float>("angle_factor");
    const auto bound_factor = Config::inst().GetOption<float>("bound_factor");
    const auto forward_factor = Config::inst().GetOption<float>("forward_factor");
    const auto band_separation = Config::inst().GetOption<float>("band_separation");

    float x = 0, y = 0;
    float heading = M_PI_2 / 2.0f;
    float fuse = 0.0f;

    HingeModel::BandSegement *first_left_band = nullptr, *first_right_band = nullptr,
                             *last_left_band = nullptr, *last_right_band = nullptr;

    for (auto child : doc.root().child("track").children())
    {
        float forward = boost::any_cast<float>(
            ParseValue(typeid(float), child.child("forward").text().as_string()));
        float angle = boost::any_cast<float>(
            ParseValue(typeid(float), child.child("angle").text().as_string()));

        x += std::cos(heading) * forward_factor * forward;
        y += std::sin(heading) * forward_factor * forward;
        heading += angle * angle_factor;

        if (fuse > band_separation)
        {
            std::string wtf = child.name();
            ASSERT(wtf == "waypoint");

            float lx =
                x + std::cos(heading + M_PI / 2.0f) * /*waypoint.l * */ bound_factor;
            float ly =
                y + std::sin(heading + M_PI / 2.0f) * /*waypoint.l * */ bound_factor;

            float rx =
                x + std::cos(heading - M_PI / 2.0f) * /*waypoint.r * */ bound_factor;
            float ry =
                y + std::sin(heading - M_PI / 2.0f) * /*waypoint.r * */ bound_factor;

            auto left_band =
                new HingeModel::BandSegement(&model, adept::Vector({lx + 500, ly + 500}));
            auto right_band =
                new HingeModel::BandSegement(&model, adept::Vector({rx + 500, ry + 500}));

            if (!first_left_band && !first_right_band)
            {
                first_left_band = left_band;
                first_right_band = right_band;
            }
            else
            {
                last_left_band->LinkForward(left_band);
                last_right_band->LinkForward(right_band);
            }

            last_left_band = left_band;
            last_right_band = right_band;
            fuse = forward;
        }
        else
        {
            fuse += forward;
        }
    }

    last_left_band->LinkForward(first_left_band);
    last_right_band->LinkForward(first_right_band);
}