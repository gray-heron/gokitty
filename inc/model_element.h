#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <torch/torch.h>
#include <tuple>

#include "log.h"
#include "visualisation.h"

class ModelElement
{
    std::vector<ModelElement *> children_;
    virtual void Simulate() = 0;
    virtual void VisualiseThis(std::vector<Visualisation::Object> &objects) = 0;

  protected:
    void AddChild(ModelElement *child);
    void Visualise(std::vector<Visualisation::Object> &objects);
    void Step();

  public:
    virtual ~ModelElement() = default;
};