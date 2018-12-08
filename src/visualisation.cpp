

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
      camera_pos_(-windows_offset_), zoom_(1.0f), exit_(false)
{
    renderer_.SetLogicalSize(Config::inst().GetOption<int>("resx"),
                             Config::inst().GetOption<int>("resy"));
}

Point Visualisation::TensorToPoint(Vector<2, false> t) { return Point(t(0, 0), t(0, 1)); }

void Visualisation::Tick(const std::vector<Object> &objects)
{
    renderer_.SetDrawColor(SDL2pp::Color(0, 0, 0)).Clear();

    for (const auto &obj : objects)
    {
        renderer_.SetDrawColor(get<1>(obj));

        renderer_.DrawLine(
            TensorToPoint((get<0>(obj).first + camera_pos_) * zoom_ + windows_offset_),
            TensorToPoint((get<0>(obj).second + camera_pos_) * zoom_ + windows_offset_));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            HandleKeyDown(event.key);
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
        exit_ = true;
        break;
    }
}

bool Visualisation::ExitRequested() { return exit_; }