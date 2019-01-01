#include <stdio.h>

#include "config.h"
#include "data_reader.h"
#include "executor.h"
#include "hinge_model.h"
#include "integration.h"
#include "log.h"
#include "visualisation.h"

using std::string;

#define STAGE_RECORD 0
#define STAGE_RACE 1

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

    std::unique_ptr<Visualisation> vis;
    if (Config::inst().GetOption<bool>("gui"))
        vis = std::make_unique<Visualisation>();

    if (Config::inst().GetOption<int>("stage") == STAGE_RECORD)
    {
        TorcsIntegration integration;
        ExecutorRecording executor;

        auto state = integration.Begin();
        while (!executor.RecordingDone())
        {
            auto steers = executor.Cycle(state, 1.0);
            state = integration.Cycle(steers);

            if (vis)
            {
                std::vector<Visualisation::Object> objects;
                executor.Visualise(objects);
                vis->Tick(objects);
            }
        }
    }

    if (Config::inst().GetOption<int>("stage") == STAGE_RACE)
    {
        HingeModel model(200, 200, 10.0f);
        DataReader::ReadTORCSTrack(Config::inst().GetOption<string>("track"), model);

        bool optimization_paused = false;
        bool exit_requested = false;

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
                model.Optimize(main_stack);
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

    log.Info() << "Nothing more to do.";
}