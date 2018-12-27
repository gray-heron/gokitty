#pragma once

#include "hinge_model.h"
#include "pid_controller.h"
#include "torcs_integration.h"

class Executor
{
  public:
    virtual CarSteers Cycle(const CarState &state, double dt) = 0;
};

class ExecutorRecording : public Executor
{
    TorcsGearbox gearbox_controller_;
    PidController speed_controller_;
    PidController crossposition_controller_;

  public:
    ExecutorRecording();
    CarSteers Cycle(const CarState &state, double dt) override;
};

class ExecutorRacing : public Executor
{
    const HingeModel &model_;
    TorcsGearbox gearbox_operator_;

  public:
    ExecutorRacing(const HingeModel &model_);
    CarSteers Cycle(const CarState &state, double dt) override;
};