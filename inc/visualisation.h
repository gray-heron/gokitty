
#pragma once
#include <torch/torch.h>
#include <tuple>

#include <SDL2pp/SDL2pp.hh>

class Visualisation
{
  private:
    SDL2pp::Point TensorToPoint(torch::Tensor t);
    void HandleKeyDown(SDL_KeyboardEvent key);

    SDL2pp::SDL sdl_;
    SDL2pp::Window window_;
    SDL2pp::Renderer renderer_;

    torch::Tensor zoom_;
    torch::Tensor camera_pos_;

    bool exit_;

  public:
    using Object = std::tuple<std::pair<torch::Tensor, torch::Tensor>, SDL2pp::Color>;

    Visualisation();
    void Tick(const std::vector<Object> &objects);
    bool ExitRequested();
};