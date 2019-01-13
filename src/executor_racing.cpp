
#include "config.h"
#include "executor.h"

ExecutorRacing::ExecutorRacing(const HingeModel &model)
    : model_(model),
      speed_controller_(Config::inst().GetOption<float>("driver_speed_p"),
                        Config::inst().GetOption<float>("driver_speed_i"),
                        Config::inst().GetOption<float>("driver_speed_d"), -0.4, 1.0),
      angle_controller_(Config::inst().GetOption<float>("driver_angle_p"),
                        Config::inst().GetOption<float>("driver_angle_i"),
                        Config::inst().GetOption<float>("driver_angle_d"), -1.0, 1.0),
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
    double corrected_forward =
        state.absolute_odometer + Config::inst().GetOption<float>("forward_boost");

    while (corrected_forward > current_hinge_->GetForward())
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

    double target_speed = current_hinge_->GetSpeed();
    for (std::pair<HingeModel::Hinge *, int> p = {current_hinge_, 0};
         p.first->GetNext() && p.second < 3; p.second += 1)
    {
        target_speed = p.first->GetSpeed();
        p.first = p.first->GetNext();
    }

    double target_crossposition, target_angle;

    if (auto previous_hinge = current_hinge_->GetPrevious())
    {
        auto v_separation = current_hinge_->GetForward() - previous_hinge->GetForward();
        auto h_separation =
            current_hinge_->GetCrossposition() - previous_hinge->GetCrossposition();

        auto forward_from_prev = corrected_forward - previous_hinge->GetForward();
        auto curr_closeness = forward_from_prev / v_separation;

        target_angle = std::atan2(h_separation, v_separation);

        ASSERT(curr_closeness >= 0.0);
        ASSERT(curr_closeness <= 1.0);

        target_crossposition =
            curr_closeness * current_hinge_->GetCrossposition() +
            (1.0 - curr_closeness) * previous_hinge->GetCrossposition();

        ASSERT(target_crossposition >= -1.0);
        ASSERT(target_crossposition <= 1.0);
    }
    else
    {
        target_crossposition = current_hinge_->GetCrossposition();
        target_angle = 0.0;
    }

    target_crossposition *= Config::inst().GetOption<float>("cross_safety_margin");

    ret.gas = speed_controller_.Cycle(target_speed * 1.06, state.speed_x, target_speed);

    auto angle_from_crossposition =
        crossposition_controller_.Cycle(target_crossposition, state.cross_position, dt);

    ret.steering_wheel = angle_controller_.Cycle(
        target_angle + 1.0 * angle_from_crossposition, -state.angle, dt);

    if (std::abs(ret.steering_wheel) > 0.3 && ret.gas > 0.0 && state.speed_x > 50.0)
        ret.gas /= 3.0;

    gearbox_controller_.SetClutchAndGear(state, ret);

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