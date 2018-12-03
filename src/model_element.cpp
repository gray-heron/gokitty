#include "model_element.h"

void ModelElement::AddChild(ModelElement *child) { children_.push_back(child); }

void ModelElement::ComputeScore(adept::adouble &score)
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

void ModelElement::ApplyGradient()
{
    ApplyGradientThis();
    for (auto child : children_)
    {
        child->ApplyGradient();
    }
}