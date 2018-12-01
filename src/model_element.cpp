#include "model_element.h"

void ModelElement::AddChild(ModelElement *child) { children_.push_back(child); }

void ModelElement::Step()
{
    Simulate();
    for (auto child : children_)
    {
        child->Step();
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