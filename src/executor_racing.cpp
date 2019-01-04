
#include "config.h"
#include "executor.h"

ExecutorRacing::ExecutorRacing(const HingeModel &model)
    : model_(model),
      speed_controller_(Config::inst().GetOption<float>("driver_speed_p"),
                        Config::inst().GetOption<float>("driver_speed_i"),
                        Config::inst().GetOption<float>("driver_speed_d"), -1.0, 1.0),
      crossposition_controller_(Config::inst().GetOption<float>("driver_cross_p"),
                                Config::inst().GetOption<float>("driver_cross_i"),
                                Config::inst().GetOption<float>("driver_cross_d"), -1.0,
                                1.0)
{
    log_.Info() << "Created racing executor.";
}

CarSteers ExecutorRacing::Cycle(const CarState &state, double dt)
{
    CarSteers ret;

    double target_speed, target_crossposition;

    ret.gas = speed_controller_.Cycle(target_speed, state.speed_x, dt);
    ret.steering_wheel =
        crossposition_controller_.Cycle(target_crossposition, state.cross_position, dt);

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

void ExecutorRacing::Visualise(std::vector<Visualisation::Object> &objects) const {}