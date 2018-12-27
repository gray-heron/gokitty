#pragma once

#include "hinge_model.h"
#include "torcs_integration.h"

class Executor
{
  public:
    virtual CarSteers Cycle(const CarState &state) = 0;
};

class ExecutorRecording : public Executor
{
    TorcsGearbox gearbox_operator_;

  public:
    ExecutorRecording();
    CarSteers Cycle(const CarState &state) override;
};

class ExecutorRacing : public Executor
{
    const HingeModel &model_;

  public:
    ExecutorRacing(const HingeModel &model_);
    CarSteers Cycle(const CarState &state) override;
};