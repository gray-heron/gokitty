#include <stdio.h>

#include "config.h"
#include "log.h"

int main()
{
    Log log("main");
    log.Info() << "gokitty demo application";

    LoggingSingleton::inst().SetConsoleVerbosity(
        Config::inst().GetOption<bool>("verbose"));
    LoggingSingleton::inst().AddLogFile(
        Config::inst().GetOption<std::string>("log_file"));
}