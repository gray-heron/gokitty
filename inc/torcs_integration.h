
#pragma once

#include "integration.h"

class TorcsGearbox
{
    int gear_dir_;
    float last_rpm_;

  public:
    void SetClutchAndGear(const CarState &state, CarSteers &steers);
};

class TorcsIntegration : public SimIntegration
{
    using udp = boost::asio::ip::udp;
    udp::endpoint server_endpoint;
    boost::asio::io_service io_service;
    std::unique_ptr<udp::socket> socket;

    CarState ParseCarState(std::string in);
    std::string ParseString(char **cursor);

    void Send(std::string msg);
    std::string Receive();

  public:
    virtual CarState Cycle(const CarSteers &) override;
    virtual CarState Begin() override;

    TorcsIntegration();
};