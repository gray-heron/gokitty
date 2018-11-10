
#pragma once

#include <boost/any.hpp>
#include <exceptions.h>
#include <map>
#include <string>

class Config
{
  private:
    Config() = default;
    std::map<std::string, boost::any> params;

  public:
    Config(Config const &) = delete;
    void operator=(Config const &) = delete;

    static Config &inst()
    {
        static Config instance;
        return instance;
    }

    bool Load(std::string config);

    template <typename T> T GetOption(std::string name)
    {
        auto val = params.find(name);
        ASSERT(val != params.end(), "No such option: " + name);
        ASSERT(val->second.type() == typeid(T),
               "Requested option " + name + " with type " + typeid(T).name() +
                   " but got " + val->second.type().name());
        return boost::any_cast<T>(val->second);
    }
};