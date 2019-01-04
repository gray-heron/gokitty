
#pragma once
#include <tuple>

#include <SDL2pp/SDL2pp.hh>
#include <adept_arrays.h>
#include <boost/optional/optional.hpp>
#include <functional>
#include <queue>

#include "log.h"
#include "util.h"

class Visualisation
{
  public:
    class TooltipInterface
    {
      public:
        virtual std::string GetTooltip() const = 0;
    };

    enum Action
    {
        OptimizationPause,
        Exit
    };

    using Object = std::tuple<Vector<2, false>, Vector<2, false>,
                              const TooltipInterface *, SDL2pp::Color>;

  private:
    static SDL2pp::Point TensorToPoint(Vector<2, false> t);
    Vector<2, false> Projection(Vector<2, false> pos);

    void HandleKeyDown(SDL_KeyboardEvent key);
    void HandleMouseKeyDown(SDL_MouseButtonEvent btn, const std::vector<Object> &objects);

    SDL2pp::SDL sdl_;
    SDL2pp::Window window_;
    SDL2pp::Renderer renderer_;

    float zoom_;
    const Vector<2, false> windows_offset_;
    Vector<2, false> camera_pos_;

    std::queue<Action> action_queue_;

    Log log_{"Visualization"};

  public:
    Visualisation();
    void SetCameraPos(Vector<2, false> camera_center);

    boost::optional<Visualisation::Action> DequeueAction();
    void Tick(const std::vector<Object> &objects);
};