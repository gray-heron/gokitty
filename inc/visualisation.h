
#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <torch/torch.h>
#include <tuple>

class Visualisation
{
  public:
    using Object = std::tuple<std::pair<torch::Tensor, torch::Tensor>, SDL2pp::Color>;
};