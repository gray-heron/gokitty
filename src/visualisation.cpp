#include "visualisation.h"
#include "config.h"
#include "util.h"

using namespace SDL2pp;
using std::get;

Visualisation::Visualisation()
    : sdl_(SDL_INIT_VIDEO),
      window_("gokitty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
              Config::inst().GetOption<int>("resx"),
              Config::inst().GetOption<int>("resy"), 0),
      renderer_(window_, -1, SDL_RENDERER_ACCELERATED),
      windows_offset_(Vector<2, false>({{Config::inst().GetOption<int>("resx") / 2,
                                         Config::inst().GetOption<int>("resy") / 2}})),
      camera_pos_(-windows_offset_), zoom_(1.0f)
{
    renderer_.SetLogicalSize(Config::inst().GetOption<int>("resx"),
                             Config::inst().GetOption<int>("resy"));
}

Vector<2, false> Visualisation::Projection(Vector<2, false> pos)
{
    return (pos + camera_pos_) * zoom_ + windows_offset_;
}

Point Visualisation::TensorToPoint(Vector<2, false> t) { return Point(t(0, 0), t(0, 1)); }

void Visualisation::Tick(const std::vector<Object> &objects)
{
    renderer_.SetDrawColor(SDL2pp::Color(0, 0, 0)).Clear();

    for (const auto &obj : objects)
    {
        renderer_.SetDrawColor(get<3>(obj));

        renderer_.DrawLine(TensorToPoint(Projection(get<0>(obj))),
                           TensorToPoint(Projection(get<1>(obj))));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            HandleKeyDown(event.key);
            break;
        case SDL_MOUSEBUTTONDOWN:
            HandleMouseKeyDown(event.button, objects);
            break;
        }
    }

    renderer_.Present();
}

void Visualisation::HandleKeyDown(SDL_KeyboardEvent key)
{
    switch (key.keysym.sym)
    {
    case SDLK_UP:
        camera_pos_ += Vector<2, false>({{0.0f, 30.0f}}) / zoom_;
        break;
    case SDLK_DOWN:
        camera_pos_ += Vector<2, false>({{0.0f, -30.0f}}) / zoom_;
        break;
    case SDLK_LEFT:
        camera_pos_ += Vector<2, false>({{30.0f, 0.0f}}) / zoom_;
        break;
    case SDLK_RIGHT:
        camera_pos_ += Vector<2, false>({{-30.0f, 0.0f}}) / zoom_;
        break;
    case SDLK_KP_PLUS:
        zoom_ *= 2.0f;
        break;
    case SDLK_KP_MINUS:
        zoom_ *= 0.5f;
        break;
    case SDLK_ESCAPE:
        action_queue_.push(Action::Exit);
        break;
    case SDLK_PAUSE:
        action_queue_.push(Action::OptimizationPause);
        break;
    }
}

void Visualisation::HandleMouseKeyDown(SDL_MouseButtonEvent key,
                                       const std::vector<Object> &objects)
{
    if (key.button != SDL_BUTTON_LEFT)
        return;

    adept::Real min_dist = 1000000;
    const TooltipInterface *min_obj = nullptr;

    for (const auto &obj : objects)
    {
        adept::Real dist =
            adept::norm2(Vector<2, false>({{key.x, key.y}}) - Projection(get<0>(obj)));

        if (dist < min_dist)
        {
            min_obj = get<2>(obj);
            min_dist = dist;
        }
    }

    if (min_obj && min_dist < 30.0)
        log_.Info() << min_obj->GetTooltip();
    else
        log_.Warning() << "Couldn't find object at {" << key.x << "," << key.y << "}";
}

boost::optional<Visualisation::Action> Visualisation::DequeueAction()
{
    if (action_queue_.empty())
        return boost::none;

    auto ret = action_queue_.front();
    action_queue_.pop();
    return ret;
}