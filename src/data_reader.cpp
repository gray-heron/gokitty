
#include <math.h>
#include <pugixml.hpp>

#include "config.h"
#include "data_reader.h"
#include "exceptions.h"
#include "util.h"

void DataReader::ReadTORCSTrack(std::string xml_path, HingeModel &model)
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

    float x = 0, y = 0;
    float heading = M_PI_2 / 2.0f;
    float fuse = 0.0f;
    int hinges_n = 0;

    HingeModel::BandSegement *first_left_band = nullptr, *first_right_band = nullptr,
                             *last_left_band = nullptr, *last_right_band = nullptr;

    HingeModel::Hinge *last_hinge = nullptr, *first_hinge = nullptr;

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
            ASSERT((std::string)child.name() == "waypoint");

            float lx =
                x + std::cos(heading + M_PI / 2.0f) * /*waypoint.l * */ bound_factor;
            float ly =
                y + std::sin(heading + M_PI / 2.0f) * /*waypoint.l * */ bound_factor;

            float rx =
                x + std::cos(heading - M_PI / 2.0f) * /*waypoint.r * */ bound_factor;
            float ry =
                y + std::sin(heading - M_PI / 2.0f) * /*waypoint.r * */ bound_factor;

            auto left_band =
                new HingeModel::BandSegement(&model, adept::Vector({lx, ly}) + 500.0f);
            auto right_band =
                new HingeModel::BandSegement(&model, adept::Vector({rx, ry}) + 500.0f);
            auto hinge = new HingeModel::Hinge(
                &model, adept::Vector({(rx + lx) / 2.0f, (ry + ly) / 2.0f}) + 500.0f);

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
    }

    last_left_band->LinkForward(first_left_band);
    last_right_band->LinkForward(first_right_band);
    last_hinge->LinkForward(first_hinge);

    log.Info() << "Track reading done. " << hinges_n << " hinges produced.";
}