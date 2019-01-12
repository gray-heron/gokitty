
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
                                1.0),
      current_hinge_(model_.GetFirstHinge()),
      forward_boost_(Config::inst().GetOption<float>("forward_boost"))
{
    log_.Info() << "Created racing executor.";
}

CarSteers ExecutorRacing::Cycle(const CarState &state, double dt)
{
    // refresh configuration
    auto config_path = Config::inst().GetOption<std::string>("config");
    if (config_path != "")
    {
        Config::inst().Load(config_path);
    }

    CarSteers ret;

    double target_speed = Config::inst().GetOption<float>("racing_speed");
    double target_crossposition;

    if (auto previous_hinge = current_hinge_->GetPrevious())
    {
        auto separation = current_hinge_->GetForward() - previous_hinge->GetForward();
        auto forward_from_prev = state.absolute_odometer - previous_hinge->GetForward();
        auto curr_closeness = forward_from_prev / separation;
        target_crossposition =
            curr_closeness * current_hinge_->GetCrossposition() +
            (1.0 - curr_closeness) * previous_hinge->GetCrossposition();
    }
    else
    {
        target_crossposition = current_hinge_->GetCrossposition();
    }

    target_crossposition *= Config::inst().GetOption<float>("bound_factor");

    ret.gas = speed_controller_.Cycle(target_speed, state.speed_x, target_speed);
    ret.steering_wheel =
        crossposition_controller_.Cycle(target_crossposition, state.cross_position, dt);

    gearbox_controller_.SetClutchAndGear(state, ret);

    while (state.absolute_odometer + Config::inst().GetOption<float>("forward_boost") >
           current_hinge_->GetForward())
    {
        auto next = current_hinge_->GetNext();
        if (next)
        {
            current_hinge_ = current_hinge_->GetNext();
        }
        else
        {
            current_hinge_ = model_.GetFirstHinge();
            log_.Info() << "Lap completed";
            break;
        }
    }

    log_.Info() << state.cross_position << " " << target_crossposition << " "
                << ret.steering_wheel;

    if (state.speed_x < 25.0)
    {
        speed_controller_.Reset();
        crossposition_controller_.Reset();
    }

    if (ret.gas < 0.0)
        ret.hand_brake = -ret.gas;

    return ret;
}

void ExecutorRacing::Visualise(std::vector<Visualisation::Object> &objects) const
{
    Vector<2, false> cpv = current_hinge_->GetCrosspositionVector() * 3.0;
    objects.push_back(Visualisation::Object(cpv + current_hinge_->GetPosition(),
                                            cpv * -1.0 + current_hinge_->GetPosition(),
                                            nullptr, SDL2pp::Color(255, 255, 255)));
}