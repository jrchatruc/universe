#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#include <chrono>

#include "universe.h"
#include "linalg.h"

using LinAlg::Matrix3;
using LinAlg::Vector2;
using LinAlg::Vector3;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int BLOCK_WIDTH = 16;
const int BLOCK_HEIGHT = 16;

auto camera = Vector3(-2, -2, -2);
auto current_basis = LinAlg::Basis{Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)};

// TODO: Allow camera rotation. Start by allowing 90 degree rotations
// TODO: User can pause the simulation and enter commands into the terminal to add/remove planets and then resume.
// TODO: The result of the simulation seems to be tied to the window's height and width; figure out why and fix it.
// TODO: Replace std::vector with a custom bump allocator to allocate at most 1_000 planets or something.

bool init();
void close();
void draw_system(std::vector<Universe::CelestialBody> &solar_system);
void draw_body(Universe::CelestialBody &body);
std::vector<Universe::CelestialBody> create_solar_system();
void render_mesh(std::vector<std::pair<Vector3, Vector3>> edges, int scaler, Vector2 translator);
void draw_cube();
void draw_sphere(int N, int M, Vector3 position);
Vector3 sphere_sample_point(int n, int m, int N, int M, Vector3 position);

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

		auto now = std::chrono::system_clock::now();
		while (!quit)
		{
			SDL_RenderClear(gRenderer);
			auto new_now = std::chrono::system_clock::now();
			double delta_t =
				(std::chrono::duration_cast<
					 std::chrono::duration<float>>(new_now - now))
					.count();
			now = new_now;
			while (SDL_PollEvent(&e))
			{
				switch (e.type)
				{
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
					case SDLK_LEFT:
						camera += Vector3(-0.05, 0., 0.);
						break;
					case SDLK_RIGHT:
						camera += Vector3(0.05, 0., 0.);
						break;
					case SDLK_UP:
						camera += Vector3(0., 0.05, 0.);
						break;
					case SDLK_DOWN:
						camera += Vector3(0., -0.05, 0.);
						break;
					case SDLK_w:
					{
						auto direction = current_basis.v_3;
						camera += direction * 0.05;
						break;
					}
					case SDLK_s:
					{
						auto direction = current_basis.v_3;
						camera += direction * (-0.05);
						break;
					}
					case SDLK_a:
					{
						auto direction = current_basis.v_1;
						camera += direction * (-0.05);
						break;
					}
					case SDLK_d:
					{
						auto direction = current_basis.v_1;
						camera += direction * 0.05;
						break;
					}
					case SDLK_r:
					{
						// X as rotation axis
						Matrix3 rotation = Matrix3{
							{{1, 0, 0},
							 {0, cos(M_PI / 40), -sin(M_PI / 40)},
							 {0, sin(M_PI / 40), cos(M_PI / 40)}}};
						current_basis.rotate(rotation);
						break;
					}
					case SDLK_t:
					{
						// Y as rotation axis
						Matrix3 rotation = Matrix3{
							{{cos(M_PI / 40), 0, -sin(M_PI / 40)},
							 {0, 1, 0},
							 {sin(M_PI / 40), 0, cos(M_PI / 40)}}};
						current_basis.rotate(rotation);
						break;
					}
					case SDLK_y:
					{
						// Z as rotation axis
						Matrix3 rotation = Matrix3{
							{{cos(M_PI / 40), -sin(M_PI / 40), 0},
							 {sin(M_PI / 40), cos(M_PI / 40), 0},
							 {0, 0, 1}}};
						current_basis.rotate(rotation);
						break;
					}
					default:
						break;
					}
					break;
				case SDL_MOUSEMOTION:
				{
					// TODO: FIX THE CAMERA, it does not work well at all.
					int xpos = e.motion.xrel;
					int ypos = e.motion.yrel;
					// The rotation axis is the vector orthogonal to (xpos, ypos, 0) and e_3.
					// Source for this calculation https://math.stackexchange.com/questions/142821/matrix-for-rotation-around-a-vector
					if (xpos != 0 || ypos != 0)
					{
						auto rotation_angle = M_PI / 100;
						auto input = LinAlg::normalize(Vector3{xpos, ypos, 0});
						auto axis = LinAlg::cross(Vector3{0, 0, 1}, input);
						auto W = Matrix3{{{0, -axis.z, axis.y},
										  {axis.z, 0, -axis.x},
										  {-axis.y, axis.x, 0}}};
						auto rotation = IDENTITY_MATRIX + W * (sin(rotation_angle)) + (W * W) * (2 * pow(sin(rotation_angle / 2), 2));
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

void draw_cube()
{

	auto A = Vector3(1, 1, 1);
	auto B = Vector3(-1, 1, 1);
	auto C = Vector3(1, -1, 1);
	auto D = Vector3(-1, -1, 1);
	auto E = Vector3(1, 1, -1);
	auto F = Vector3(-1, 1, -1);
	auto G = Vector3(1, -1, -1);
	auto H = Vector3(-1, -1, -1);

	std::vector<std::pair<Vector3, Vector3>> mesh;
	mesh.push_back(std::make_pair(A, B));
	mesh.push_back(std::make_pair(C, D));
	mesh.push_back(std::make_pair(E, F));
	mesh.push_back(std::make_pair(G, H));
	mesh.push_back(std::make_pair(A, C));
	mesh.push_back(std::make_pair(B, D));
	mesh.push_back(std::make_pair(E, G));
	mesh.push_back(std::make_pair(F, H));
	mesh.push_back(std::make_pair(A, E));
	mesh.push_back(std::make_pair(C, G));
	mesh.push_back(std::make_pair(B, F));
	mesh.push_back(std::make_pair(D, H));

	render_mesh(mesh, 200, Vector2(400, 400));
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

	render_mesh(mesh, 200, Vector2(400, 400));
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
