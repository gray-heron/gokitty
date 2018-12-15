#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <adept_arrays.h>
#include <tuple>

#include "log.h"
#include "visualisation.h"

class ModelElement
{
    std::vector<ModelElement *> children_;
    virtual void SetupEquationsThis() = 0;
    virtual void ComputeScoreThis(adept::aReal &score) const = 0;
    virtual void ApplyGradientThis(double score_normalization) = 0;
    virtual void VisualiseThis(std::vector<Visualisation::Object> &objects) const = 0;

  protected:
    void SetupEquations();
    void AddChild(ModelElement *child);
    void ComputeScore(adept::aReal &score);
    void ApplyGradient(double score_normalization);

  public:
    virtual ~ModelElement() = default;
    void Visualise(std::vector<Visualisation::Object> &objects);
};