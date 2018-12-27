
#include "config.h"
#include "executor.h"

ExecutorRecording::ExecutorRecording() {}

CarSteers ExecutorRecording::Cycle(const CarState &state)
{
    CarSteers ret;
    ret.gas = 1.0;
    ret.steering_wheel = 0.1;
    gearbox_operator_.SetClutchAndGear(state, ret);

    return ret;
}