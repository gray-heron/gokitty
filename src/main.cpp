#include <stdio.h>

#include "config.h"
#include "data_reader.h"
#include "executor.h"
#include "hinge_model.h"
#include "integration.h"
#include "log.h"
#include "visualisation.h"

using std::string;

int main(int argc, char **argv)
{
    Log log("main");
    log.Info() << "gokitty demo application";

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

    Config::inst().DumpSettings();

    //====================

    adept::Stack main_stack;
    HingeModel model(100, 100, 10.0f);
    DataReader::ReadTORCSTrack(Config::inst().GetOption<string>("track"), model);

    bool optimization_paused = false;
    bool exit_requested = false;

    std::unique_ptr<Visualisation> vis;
    if (Config::inst().GetOption<bool>("gui"))
        vis = std::make_unique<Visualisation>();

    TorcsIntegration integration;
    ExecutorRecording executor;

    auto state = integration.Begin();
    while (true)
    {
        auto steers = executor.Cycle(state, 1.0);
        state = integration.Cycle(steers);
        log.Info() << "Crossposition: " << state.cross_position
                   << " F: " << state.sensors[9];
    }

    while (!exit_requested)
    {
        if (vis)
        {
            std::vector<Visualisation::Object> objects;
            model.Visualise(objects);
            vis->Tick(objects);
        }

        if (!optimization_paused)
        {
            if (model.Optimize(main_stack) < 250.0)
                return 0;
        }
        else
        {
            main_stack.new_recording();
        }

        if (vis)
        {
            while (auto action = vis->DequeueAction())
            {
                switch (*action)
                {
                case Visualisation::Exit:
                    exit_requested = true;
                    break;
                case Visualisation::OptimizationPause:
                    optimization_paused = !optimization_paused;
                    break;
                default:
                    ASSERT(0, "Action not implemented!")
                }
            }
        }
    }
}