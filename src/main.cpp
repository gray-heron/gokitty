#include <stdio.h>

#include "config.h"
#include "data_reader.h"
#include "hinge_model.h"
#include "log.h"
#include "visualisation.h"

using std::string;

int main(int argc, char **argv)
{
    Log log("main");
    log.Info() << "gokitty demo application";

    Config::inst().Load("res/default_configuration.xml");
    Config::inst().Load(argc, argv);

    auto config_path = Config::inst().GetOption<std::string>("config");
    if (config_path != "")
    {
        Config::inst().Load(config_path);
        Config::inst().Load(argc, argv); // override params from config file
    }

    LoggingSingleton::inst().SetConsoleVerbosity(
        Config::inst().GetOption<bool>("verbose"));
    LoggingSingleton::inst().AddLogFile(
        Config::inst().GetOption<std::string>("log_file"));

    //====================

    HingeModel model(20, 20, 50.0f);
    Visualisation vis;
    DataReader::ReadTORCSTrack(Config::inst().GetOption<string>("track"), model);

    while (!vis.ExitRequested())
    {
        std::vector<Visualisation::Object> objects;
        model.Visualise(objects);
        vis.Tick(objects);
    }
}