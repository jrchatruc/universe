#include <SDL2/SDL.h>
#include <math.h>

#include "renderer.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Surface *gScreenSurface = NULL;

Vector3 camera = Vector3{-2, -2, -2};
LinAlg::Basis current_camera_basis = LinAlg::Basis{Vector3{1, 0, 0}, Vector3{0, 1, 0}, Vector3{0, 0, 1}};

namespace Renderer
{
    static void draw_body(Universe::CelestialBody &body);
    static void draw_sphere(int N, int M, Vector3 position);
    static void render_mesh(std::vector<std::pair<Vector3, Vector3>> edges, int scaler, Vector2 translator);
    static Vector3 sphere_sample_point(int n, int m, int N, int M, Vector3 position);

    void close()
    {
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;

        SDL_Quit();
    }

    bool init()
    {
        bool success = true;

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("SDL could not initialize! SDL_Error: %s\n",
                   SDL_GetError());
            success = false;
        }
        else
        {
            gWindow = SDL_CreateWindow(
                "Planets", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
            if (gWindow == NULL)
            {
                printf("Window could not be created! SDL_Error: %s\n",
                       SDL_GetError());
                success = false;
            }
            else
            {
                gScreenSurface = SDL_GetWindowSurface(gWindow);
                gRenderer = SDL_CreateSoftwareRenderer(gScreenSurface);
            }
        }

        return success;
    }

    void clear_window()
    {
        SDL_RenderClear(gRenderer);
    }

    void update_window()
    {
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
        SDL_UpdateWindowSurface(gWindow);
    }

    void move_camera(Vector3 direction)
    {
        camera += direction * 0.1;
    }

    void draw_system(std::vector<Universe::CelestialBody> &solar_system)
    {
        for (auto &body : solar_system)
        {
            draw_body(body);
        }
    }

    static void draw_body(Universe::CelestialBody &body)
    {
        draw_sphere(20, 20, body.position);
    }

    // edges correspond to every pair ((m, n), (m', n')) where |m - m'| = 1 xor |n - n'| = 1
    static void draw_sphere(int N, int M, Vector3 position)
    {
        std::vector<std::pair<Vector3, Vector3>> mesh;
        // This mesh is insanely inefficient, a lot (and I do mean A LOT) of edges will be pushed back twice, but w/e.
        for (int m = 0; m <= M; m++)
        {
            for (int n = 0; n <= N; n++)
            {
                auto v_1 = sphere_sample_point(n, m, N, M, position);
                if (n < N)
                {
                    auto v_2 = sphere_sample_point(n + 1, m, N, M, position);
                    mesh.push_back(std::make_pair(v_1, v_2));
                }
                if (n > 0)
                {
                    auto v_2 = sphere_sample_point(n - 1, m, N, M, position);
                    mesh.push_back(std::make_pair(v_1, v_2));
                }

                if (m < M)
                {
                    auto v_2 = sphere_sample_point(n, m + 1, N, M, position);
                    mesh.push_back(std::make_pair(v_1, v_2));
                }

                if (m > 0)
                {
                    auto v_2 = sphere_sample_point(n, m - 1, N, M, position);
                    mesh.push_back(std::make_pair(v_1, v_2));
                }
            }
        }

        render_mesh(mesh, 200, Vector2{WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2});
    }

    static Vector3 sphere_sample_point(int n, int m, int N, int M, Vector3 position)
    {
        return Vector3{sin(M_PI * m / M) * cos(2 * M_PI * n / N), sin(M_PI * m / M) * sin(2 * M_PI * n / N), cos(M_PI * m / M)} + position;
    }

    static void render_mesh(std::vector<std::pair<Vector3, Vector3>> edges, int scaler, Vector2 translator)
    {
        SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 0);
        for (auto &edge : edges)
        {
            auto vertex_1 = current_camera_basis.coordinates(edge.first - camera);
            auto vertex_2 = current_camera_basis.coordinates(edge.second - camera);

            // Only render edge if it's in front of the camera
            if (vertex_1.z > 0 && vertex_2.z > 0)
            {
                auto v_1 = Vector2{vertex_1.x / vertex_1.z, vertex_1.y / vertex_1.z};
                auto v_2 = Vector2{vertex_2.x / vertex_2.z, vertex_2.y / vertex_2.z};

                v_1 = v_1 * scaler + translator;
                v_2 = v_2 * scaler + translator;

                SDL_RenderDrawLine(gRenderer, (int)v_2.x, (int)v_2.y, (int)v_1.x, (int)v_1.y);
            }
        }
    }
} // namespace Renderer