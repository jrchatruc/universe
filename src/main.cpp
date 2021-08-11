#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "universe.h"
#include "linalg.h"

using LinAlg::Matrix3;
using LinAlg::Vector2;
using LinAlg::Vector3;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

auto camera = Vector3(-2, -2, -2);
auto current_basis = LinAlg::Basis{Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)};

// TODO: Add a ground or something to have as reference, I can't tell if there's something wrong with the camera or not.
// TODO: User can pause the simulation and enter commands into the terminal to add/remove planets and then resume.
// TODO: The result of the simulation seems to be tied to the window's height and width; figure out why and fix it.
// TODO: Replace std::vector with a custom bump allocator to allocate at most 1_000 planets or something.

bool init();
void close();
void draw_system(std::vector<Universe::CelestialBody> &solar_system);
void draw_body(Universe::CelestialBody &body);
std::vector<Universe::CelestialBody> create_solar_system();
void render_mesh(std::vector<std::pair<Vector3, Vector3>> edges, int scaler, Vector2 translator);
void draw_sphere(int N, int M, Vector3 position);
Vector3 sphere_sample_point(int n, int m, int N, int M, Vector3 position);
Matrix3 get_rotation_matrix(Vector3 axis, double angle);

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Surface *gScreenSurface = NULL;

int main(int argc, char *args[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		SDL_Event e;
		bool quit = false;
		bool paused = false;
		std::vector<Universe::CelestialBody> solar_system = create_solar_system();
		SDL_SetRelativeMouseMode(SDL_TRUE);

		struct timespec now, new_now;
		double delta_t;
		clock_gettime(CLOCK_MONOTONIC, &now);
		while (!quit)
		{
			SDL_RenderClear(gRenderer);

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
						auto direction = current_basis.v_3;
						camera += direction * 0.1;
						break;
					}
					case SDLK_s:
					{
						auto direction = current_basis.v_3;
						camera += direction * (-0.1);
						break;
					}
					case SDLK_a:
					{
						auto direction = current_basis.v_1;
						camera += direction * (-0.1);
						break;
					}
					case SDLK_d:
					{
						auto direction = current_basis.v_1;
						camera += direction * 0.1;
						break;
					}
					case SDLK_q:
					{
						// "forward" as the axis of rotation
						auto rotation = get_rotation_matrix(current_basis.v_3 * (-1), M_PI / 100);
						current_basis.rotate(rotation);
						break;
					}
					case SDLK_e:
					{
						// "forward" as the axis of rotation, opposite direction
						auto rotation = get_rotation_matrix(current_basis.v_3, M_PI / 100);
						current_basis.rotate(rotation);
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
					// The rotation axis is the vector orthogonal to (xpos, ypos, 0) (in current_basis coordinates) and current_basis.v_3.
					if (xpos != 0 || ypos != 0)
					{
						auto rotation_angle = M_PI / 100;
						auto input = LinAlg::normalize(Vector3{xpos, ypos, 0});
						auto axis = LinAlg::cross(current_basis.v_3, current_basis.v_1 * input.x + current_basis.v_2 * input.y);
						auto rotation = get_rotation_matrix(axis, rotation_angle);
						current_basis.rotate(rotation);
					}
					break;
				}
				case SDL_QUIT:
					quit = true;
				}
			}

			Universe::update_system(solar_system, delta_t);
			draw_system(solar_system);

			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
			SDL_UpdateWindowSurface(gWindow);
		}
	}

	close();

	return 0;
}

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

void draw_system(std::vector<Universe::CelestialBody> &solar_system)
{
	for (auto &body : solar_system)
	{
		draw_body(body);
	}
}

void draw_body(Universe::CelestialBody &body)
{
	draw_sphere(20, 20, body.position);
}

std::vector<Universe::CelestialBody> create_solar_system()
{
	std::vector<Universe::CelestialBody> solar_system;
	Universe::CelestialBody planet_one = Universe::CelestialBody(
		1000000,
		Vector3(0, 0, 0),
		ZERO_VECTOR_3D,
		ZERO_VECTOR_3D);

	Universe::CelestialBody planet_two = Universe::CelestialBody(
		10,
		Vector3(10, 0, 0),
		Vector3(0, -2, 0),
		ZERO_VECTOR_3D);

	Universe::CelestialBody planet_three = Universe::CelestialBody(
		10,
		Vector3(20, 0, 0),
		Vector3(0, 1, 0),
		ZERO_VECTOR_3D);

	solar_system.push_back(planet_one);
	solar_system.push_back(planet_two);
	solar_system.push_back(planet_three);

	return solar_system;
}

// edges correspond to every pair ((m, n), (m', n')) where |m - m'| = 1 xor |n - n'| = 1
void draw_sphere(int N, int M, Vector3 position)
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

	render_mesh(mesh, 200, Vector2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
}

Vector3 sphere_sample_point(int n, int m, int N, int M, Vector3 position)
{
	return Vector3(sin(M_PI * m / M) * cos(2 * M_PI * n / N), sin(M_PI * m / M) * sin(2 * M_PI * n / N), cos(M_PI * m / M)) + position;
}

void render_mesh(std::vector<std::pair<Vector3, Vector3>> edges, int scaler, Vector2 translator)
{
	SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 0);
	for (auto &edge : edges)
	{
		auto vertex_1 = current_basis.coordinates(edge.first - camera);
		auto vertex_2 = current_basis.coordinates(edge.second - camera);

		// Only render edge if it's in front of the camera
		if (vertex_1.z > 0 && vertex_2.z > 0)
		{
			auto v_1 = Vector2(vertex_1.x / vertex_1.z, vertex_1.y / vertex_1.z);
			auto v_2 = Vector2(vertex_2.x / vertex_2.z, vertex_2.y / vertex_2.z);

			v_1 = v_1 * scaler + translator;
			v_2 = v_2 * scaler + translator;

			SDL_RenderDrawLine(gRenderer, (int)v_2.x, (int)v_2.y, (int)v_1.x, (int)v_1.y);
		}
	}
}

// Source for this calculation https://math.stackexchange.com/questions/142821/matrix-for-rotation-around-a-vector
Matrix3 get_rotation_matrix(Vector3 axis, double angle)
{
	auto W = Matrix3{{{0, -axis.z, axis.y},
					  {axis.z, 0, -axis.x},
					  {-axis.y, axis.x, 0}}};
	return IDENTITY_MATRIX + W * (sin(angle)) + (W * W) * (2 * pow(sin(angle / 2), 2));
}