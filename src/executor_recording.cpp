
#include "config.h"
#include "executor.h"

ExecutorRecording::ExecutorRecording()
    : speed_controller_(Config::inst().GetOption<float>("driver_speed_p"),
                        Config::inst().GetOption<float>("driver_speed_i"),
                        Config::inst().GetOption<float>("driver_speed_d"), -0.2, 0.6),
      crossposition_controller_(Config::inst().GetOption<float>("driver_cross_p"),
                                Config::inst().GetOption<float>("driver_cross_i"),
                                Config::inst().GetOption<float>("driver_cross_d"), -1.0,
                                1.0),
      track_saver_(Config::inst().GetOption<std::string>("track")),
      sensor_left_(Config::inst().GetOption<int>("sensor_left")),
      sensor_right_(Config::inst().GetOption<int>("sensor_right")),
      sensor_front_(Config::inst().GetOption<int>("sensor_front")), last_f_(5000.0),
      recording_enabled_(false)
{
    log_.Info() << "Created recording executor.";
}

CarSteers ExecutorRecording::Cycle(const CarState &state, double dt)
{
    CarSteers ret;
    ret.gas = 1.0;
    ret.steering_wheel = 0.1;

    float target_speed = Config::inst().GetOption<float>("recording_speed");

    if (state.sensors[sensor_front_] < 150.0)
        target_speed *= 0.66;

    if (state.sensors[sensor_front_] < 100.0)
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

    if (last_f_ - state.absolute_odometer > 500.0)
    {
        recording_enabled_ = !recording_enabled_;
        log_.Info() << "Recording switched to " << int(recording_enabled_);
        last_f_ = 0.0;
    }

    if (recording_enabled_)
    {
        track_saver_.MarkWaypoint(
            state.absolute_odometer - last_f_, state.sensors[sensor_left_],
            state.sensors[sensor_right_], state.wheels_speeds[0] - state.wheels_speeds[1],
            state.speed_x);
    }

    last_f_ = state.absolute_odometer;
    return ret;
}

void ExecutorRecording::Visualise(std::vector<Visualisation::Object> &objects) const
{
    track_saver_.Visualise(objects);
}

bool ExecutorRecording::RecordingDone() { return !recording_enabled_ && last_f_ < 500.0; }