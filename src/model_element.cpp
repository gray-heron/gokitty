#include "model_element.h"

void ModelElement::AddChild(ModelElement *child) { children_.push_back(child); }

void ModelElement::ComputeScore(adept::aReal &score)
{
    ComputeScoreThis(score);

    for (auto child : children_)
    {
        child->ComputeScore(score);
    }
}

void ModelElement::Visualise(std::vector<Visualisation::Object> &objects)
{
    VisualiseThis(objects);
    for (auto child : children_)
    {
        child->Visualise(objects);
    }
}

void ModelElement::ApplyGradient(double score_normalization)
{
    ApplyGradientThis(score_normalization);
    for (auto child : children_)
    {
        child->ApplyGradient(score_normalization);
    }
}