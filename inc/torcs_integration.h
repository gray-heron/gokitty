
#pragma once

#include "integration.h"
#include "log.h"

class TorcsGearbox
{
    int gear_dir_;
    float last_rpm_;

    Log log_{"Gearbox"};

  public:
    void SetClutchAndGear(const CarState &state, CarSteers &steers);
};

class TorcsIntegration : public SimIntegration
{
    using udp = boost::asio::ip::udp;
    udp::endpoint server_endpoint_;
    boost::asio::io_service io_service_;
    udp::socket socket_;

    CarState ParseCarState(std::string in);
    std::string ParseString(char **cursor);

    void Send(std::string msg);
    std::string Receive();

    Log log_{"TorcsIntegration"};

  public:
    CarState Begin() override;
    CarState Cycle(const CarSteers &) override;

    TorcsIntegration();
    ~TorcsIntegration();
};