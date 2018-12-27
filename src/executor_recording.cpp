
#include "config.h"
#include "executor.h"

ExecutorRecording::ExecutorRecording()
    : speed_controller_(Config::inst().GetOption<float>("driver_speed_p"),
                        Config::inst().GetOption<float>("driver_speed_i"),
                        Config::inst().GetOption<float>("driver_speed_d"), -0.2, 0.6),
      crossposition_controller_(Config::inst().GetOption<float>("driver_cross_p"),
                                Config::inst().GetOption<float>("driver_cross_i"),
                                Config::inst().GetOption<float>("driver_cross_d"), -1.0,
                                1.0)
{
}

CarSteers ExecutorRecording::Cycle(const CarState &state, double dt)
{
    CarSteers ret;
    ret.gas = 1.0;
    ret.steering_wheel = 0.1;

    float target_speed = Config::inst().GetOption<float>("recording_speed");

    if (state.sensors[9] < 150.0)
        target_speed *= 0.66;

    if (state.sensors[9] < 100.0)
        target_speed *= 0.3;

    ret.gas = speed_controller_.Cycle(target_speed, state.speed_x, dt);

    ret.steering_wheel = crossposition_controller_.Cycle(0.0, state.cross_position, dt);

    gearbox_controller_.SetClutchAndGear(state, ret);

    if (state.speed_x < 25.0)
    {
        speed_controller_.Reset();
        crossposition_controller_.Reset();
    }

    if (ret.gas < 0.0)
        ret.hand_brake = -ret.gas;

    return ret;
}