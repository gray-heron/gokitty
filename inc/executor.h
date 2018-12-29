#pragma once

#include "data_reader.h"
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
    TrackSaver track_saver_;

    const int sensor_left_, sensor_right_, sensor_front_;

    double last_f_;
    bool recording_enabled_;

    Log log_{"ExecutorRecording"};

  public:
    ExecutorRecording();
    CarSteers Cycle(const CarState &state, double dt) override;

    void Visualise(std::vector<Visualisation::Object> &objects) const;
    bool RecordingDone();
};

class ExecutorRacing : public Executor
{
    const HingeModel &model_;
    TorcsGearbox gearbox_operator_;

    Log log_{"ExecutorRacing"};

  public:
    ExecutorRacing(const HingeModel &model_);
    CarSteers Cycle(const CarState &state, double dt) override;
};