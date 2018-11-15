#include <stdio.h>

#include "config.h"
#include "log.h"

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
}