
#include "pid_controller.h"

PidController::PidController(double p, double i, double d, double min, double max)
    : p_(p), i_(i), d_(d), last_feedback_(0.0), integral_(0.0), min_(min), max_(max)
{
}

void PidController::Reset()
{
    // last_feedback_ = 0.0;
    integral_ = 0.0;
}

double PidController::Cycle(double target, double feedback, double delta_time)
{
    const double p = p_ * (target - feedback);
    const double i = i_ * integral_;
    const double d = d_ * (last_feedback_ - feedback) * delta_time;
    double result = p + i + d;

    result = result > max_ ? max_ : (result < min_ ? min_ : result);

    last_feedback_ = feedback;
    integral_ += (target - feedback) * delta_time;

    integral_ =
        i > max_ ? max_ / i_ : (integral_ = result < min_ ? min_ / i_ : integral_);

    return result;
}