#include <boost/array.hpp>

#include "config.h"
#include "torcs_integration.h"

using std::string;
using namespace std::chrono_literals;

// clang-format off
const std::unordered_map<string, std::pair<size_t, int>> car_state_offset_table =
{
    { "angle",             { offsetof(CarState, angle),             1  } },
    { "curLapTime",        { offsetof(CarState, current_lap_time),  1  } },
    { "distFromStart",     { offsetof(CarState, absolute_odometer), 1  } },
    { "rpm",               { offsetof(CarState, rpm),               1  } },
    { "speedX",            { offsetof(CarState, speed_x),           1  } },
    { "speedY",            { offsetof(CarState, speed_y),           1  } },
    { "speedZ",            { offsetof(CarState, speed_z),           1  } },
    { "track",             { offsetof(CarState, sensors),           19 } },
    { "trackPos",          { offsetof(CarState, cross_position),    1  } },
    { "wheelSpinVel",      { offsetof(CarState, wheels_speeds),     4  } },
    { "z",                 { offsetof(CarState, height),            1  } },
    { "gear",              { offsetof(CarState, gear),              1  } },
};
// clang-format on

void TorcsGearbox::SetClutchAndGear(const CarState &state, CarSteers &steers)
{
    steers.gear = (int)state.gear;

    if (state.rpm > 9200.0f)
    {
        steers.clutch = steers.gear > 0 ? 0.5f : 1.0f;
        gear_dir_ = 1;
    }
    else if (state.rpm < 5000.0f && last_rpm_ >= 5000.0f)
    {
        steers.clutch = 0.5f;
        gear_dir_ = -1;
    }

    if (steers.clutch > 0.4f)
    {
        steers.gas = 0.0f;
        steers.gear += gear_dir_;
        log_.Info() << "Gear to: " << steers.gear;
    }

    if (state.speed_x < 30.0f && steers.gas > 0.4f)
        steers.gear = 1;

    steers.clutch = std::max(0.0f, steers.clutch - 0.1f);
    last_rpm_ = state.rpm;
}

TorcsIntegration::TorcsIntegration() : socket_(io_service_, udp::v4())
{

    int port = Config::inst().GetOption<int>("port");

    udp::resolver resolver(io_service_);
    udp::resolver::query query(udp::v4(), Config::inst().GetOption<std::string>("host"),
                               std::to_string(port));
    server_endpoint_ = *resolver.resolve(query);

    ASSERT(server_endpoint_.address().to_string() != "");

    socket_.non_blocking(true);
}

CarState TorcsIntegration::Begin()
{
    string init_string = "SCR(init", instring;
    std::string in_msg;

    for (int i = -9; i <= 9; i++)
    {
        init_string += string(" ") + std::to_string(Config::inst().GetOption<int>(
                                         string("distance_sensor") + std::to_string(i)));
    }
    init_string += ")";

    while (true)
    {
        do
        {
            Send(init_string);
            log_.Info() << "Sent session request...";
            std::this_thread::sleep_for(1s);
        } while ((in_msg = Receive()).length() == 0);

        int i;
        for (i = 0; i < strlen("***identified***"); i++)
        {
            if ("***identified***"[i] != in_msg[i])
            {
                break;
            }
        }

        if (!in_msg.compare("***identified***"))
        {
            log_.Info() << "Session created";
            break;
        }
        else
        {
            log_.Warning() << "Unexpected answer received: " << in_msg;
        }
    }

    while ((in_msg = Receive()).length() == 0)
        ;

    if (in_msg[0] == '*' && in_msg[1] == '*' && in_msg[2] == '*')
    {
        log_.Error() << "Unimplemented command received from the TORCS server!";
        throw std::runtime_error("");
    }

    return ParseCarState(in_msg);
}

CarState TorcsIntegration::Cycle(const CarSteers &steers)
{
    string out, in;

    out += "(accel " + std::to_string(steers.gas) + ")";
    out += "(brake " + std::to_string(steers.hand_brake) + ")";
    out += "(gear " + std::to_string(steers.gear) + ")";
    out += "(clutch " + std::to_string(steers.clutch) + ")";
    out += "(steer " + std::to_string(steers.steering_wheel) + ")";
    // out += "(focus " + std::to_string(steers.focus) + ")";
    // out += "(meta " + std::to_string(steers.gas) + ")";

    while ((in = Receive()).length() == 0)
        ;

    auto state = ParseCarState(in);
    Send(out);

    return state;
}

CarState TorcsIntegration::ParseCarState(std::string in)
{
    CarState out;

    char *cursor = (char *)in.c_str();
    string param_name;

    if (in == "***shutdown***")
    {
        log_.Info() << "Shutdown command received. Bye, bye.";
        exit(0); // FIXME: propagate exit upwards indead of exiting on the spot
    }

    while (*cursor != '\0')
    {
        if (*(cursor++) != '(')
        {
            log_.Error() << "Unimplemented command received from the TORCS server!";
            throw std::runtime_error("");
        }

        param_name = ParseString(&cursor);
        auto offset = car_state_offset_table.find(param_name);

        if (offset != car_state_offset_table.end())
        {
            for (int i = 0; i < offset->second.second; i++)
            {
                // behold!
                *((float *)((uint8_t *)&out + offset->second.first +
                            (i * sizeof(float)))) = strtof(cursor, &cursor);
            }

            if (*(cursor++) != ')')
                throw;
        }
        else
        {
            while (*(cursor++) != ')')
                ;
        }
    }

    return out;
}

string TorcsIntegration::ParseString(char **cursor)
{
    // FIXME: use C++ style string ops
    string out;
    char *tmp_cursor = *cursor;

    while (*(tmp_cursor++) != ' ')
        out += *(tmp_cursor - 1);

    *cursor = tmp_cursor;

    return out;
}

void TorcsIntegration::Send(std::string msg)
{
    boost::system::error_code ignored_error;
    std::vector<char> send_buf(msg.begin(), msg.end());
    socket_.send_to(boost::asio::buffer(send_buf), server_endpoint_, 0, ignored_error);
}

std::string TorcsIntegration::Receive()
{
    boost::system::error_code ec;

    boost::array<char, UINT16_MAX> recv_buf;
    size_t len =
        socket_.receive_from(boost::asio::buffer(recv_buf), server_endpoint_, 0, ec);

    if (ec == boost::asio::error::would_block)
        return "";
    else
        return std::string(recv_buf.data(), len - 1);
}

TorcsIntegration::~TorcsIntegration() {}