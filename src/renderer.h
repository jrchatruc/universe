#ifndef __RENDERER__H_
#define __RENDERER__H_

#include "linalg.h"
#include "universe.h"

using LinAlg::Vector2;
using LinAlg::Vector3;

extern LinAlg::Basis current_camera_basis;

namespace Renderer
{
    bool init();
    void close();
    void clear_window();
    void update_window();

    void move_camera(Vector3 direction);
    void draw_system(std::vector<Universe::CelestialBody> &solar_system);
} // namespace Renderer
#endif