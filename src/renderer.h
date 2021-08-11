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
    static void draw_body(Universe::CelestialBody &body);
    static void draw_sphere(int N, int M, Vector3 position);
    static void render_mesh(std::vector<std::pair<Vector3, Vector3>> edges, int scaler, Vector2 translator);
    static Vector3 sphere_sample_point(int n, int m, int N, int M, Vector3 position);
} // namespace Renderer
#endif