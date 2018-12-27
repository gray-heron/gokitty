#include "torcs_integration.h"

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
    }

    if (state.speed_x < 30.0f && steers.gas > 0.4f)
        steers.gear = 1;

    steers.clutch = std::max(0.0f, steers.clutch - 0.1f);
    last_rpm_ = state.rpm;
}