
#pragma once

class PidController
{
    const double p_, i_, d_;
    double last_feedback_, integral_;
    const double min_, max_;

  public:
    PidController(double p, double i, double d, double min, double max);
    void Reset();
    double Cycle(double target, double feedback, double delta_time);
};