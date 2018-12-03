#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <adept_arrays.h>
#include <tuple>

#include "log.h"
#include "visualisation.h"

class ModelElement
{
    std::vector<ModelElement *> children_;
    virtual void ComputeScoreThis(adept::adouble &score) const = 0;
    virtual void ApplyGradientThis() = 0;
    virtual void VisualiseThis(std::vector<Visualisation::Object> &objects) const = 0;

  protected:
    void AddChild(ModelElement *child);
    void ComputeScore(adept::adouble &score);
    void ApplyGradient();

  public:
    virtual ~ModelElement() = default;
    void Visualise(std::vector<Visualisation::Object> &objects);
};