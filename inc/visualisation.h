
#pragma once
#include <tuple>

#include <SDL2pp/SDL2pp.hh>
#include <adept_arrays.h>

#include "util.h"

class Visualisation
{
  private:
    SDL2pp::Point TensorToPoint(Vector<2, false> t);
    void HandleKeyDown(SDL_KeyboardEvent key);

    SDL2pp::SDL sdl_;
    SDL2pp::Window window_;
    SDL2pp::Renderer renderer_;

    float zoom_;
    Vector<2, false> windows_offset_;
    Vector<2, false> camera_pos_;

    bool exit_;

  public:
    using Object =
        std::tuple<std::pair<Vector<2, false>, Vector<2, false>>, SDL2pp::Color>;

    Visualisation();
    void Tick(const std::vector<Object> &objects);
    bool ExitRequested();
};