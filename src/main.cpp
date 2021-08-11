#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "renderer.h"
#include "universe.h"
#include "linalg.h"

using LinAlg::Vector2;
using LinAlg::Vector3;

// TODO: Add a ground or something to have as reference, I can't tell if there's something wrong with the camera or not.
// TODO: User can pause the simulation and enter commands into the terminal to add/remove planets and then resume.
// TODO: The result of the simulation seems to be tied to the window's height and width; figure out why and fix it.
// TODO: Replace std::vector with a custom bump allocator to allocate at most 1_000 planets or something.

int main(int argc, char *args[])
{
	if (!Renderer::init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		SDL_Event e;
		bool quit = false;
		bool paused = false;
		std::vector<Universe::CelestialBody> solar_system = Universe::create_solar_system();
		SDL_SetRelativeMouseMode(SDL_TRUE);

		struct timespec now, new_now;
		double delta_t;
		clock_gettime(CLOCK_MONOTONIC, &now);
		while (!quit)
		{
			Renderer::clear_window();

			clock_gettime(CLOCK_MONOTONIC, &new_now);
			delta_t = (new_now.tv_sec - now.tv_sec);
			delta_t += (new_now.tv_nsec - now.tv_nsec) / 1000000000.0;

			now = new_now;
			while (SDL_PollEvent(&e))
			{
				switch (e.type)
				{
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
					case SDLK_w:
					{
						auto direction = current_camera_basis.v_3;
						Renderer::move_camera(direction);
						break;
					}
					case SDLK_s:
					{
						auto direction = current_camera_basis.v_3 * (-1);
						Renderer::move_camera(direction);
						break;
					}
					case SDLK_a:
					{
						auto direction = current_camera_basis.v_1 * (-1);
						Renderer::move_camera(direction);
						break;
					}
					case SDLK_d:
					{
						auto direction = current_camera_basis.v_1;
						Renderer::move_camera(direction);
						break;
					}
					case SDLK_q:
					{
						// "forward" as the axis of rotation
						current_camera_basis.rotate(current_camera_basis.v_3 * (-1), M_PI / 100);
						break;
					}
					case SDLK_e:
					{
						// "forward" as the axis of rotation, opposite direction
						current_camera_basis.rotate(current_camera_basis.v_3, M_PI / 100);
						break;
					}
					default:
						break;
					}
					break;
				case SDL_MOUSEMOTION:
				{
					int xpos = e.motion.xrel;
					int ypos = e.motion.yrel;
					// The rotation axis is the vector orthogonal to (xpos, ypos, 0) (in current_camera_basis coordinates) and current_camera_basis.v_3.
					if (xpos != 0 || ypos != 0)
					{
						auto rotation_angle = M_PI / 100;
						auto input = LinAlg::normalize(Vector3{xpos, ypos, 0});
						auto axis = LinAlg::cross(current_camera_basis.v_3, current_camera_basis.v_1 * input.x + current_camera_basis.v_2 * input.y);
						current_camera_basis.rotate(axis, rotation_angle);
					}
					break;
				}
				case SDL_QUIT:
					quit = true;
				}
			}

			Universe::update_system(solar_system, delta_t);
			Renderer::draw_system(solar_system);

			Renderer::update_window();
		}
	}

	Renderer::close();

	return 0;
}
