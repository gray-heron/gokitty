
#pragma once
#include <tuple>

#include <SDL2pp/SDL2pp.hh>
#include <adept_arrays.h>

class Visualisation
{
  private:
    SDL2pp::Point TensorToPoint(adept::Vector t);
    void HandleKeyDown(SDL_KeyboardEvent key);

    SDL2pp::SDL sdl_;
    SDL2pp::Window window_;
    SDL2pp::Renderer renderer_;

    float zoom_;
    adept::Vector windows_offset_;
    adept::Vector camera_pos_;

    bool exit_;

  public:
    using Object = std::tuple<std::pair<adept::Vector, adept::Vector>, SDL2pp::Color>;

    Visualisation();
    void Tick(const std::vector<Object> &objects);
    bool ExitRequested();
};