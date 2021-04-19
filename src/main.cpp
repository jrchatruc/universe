#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#include <chrono>

#include "universe.h"
#include "linalg.h"

using LinAlg::Vector2;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int BLOCK_WIDTH = 16;
const int BLOCK_HEIGHT = 16;

// TODO: User can pause the simulation and enter commands into the terminal to add/remove planets and then resume.
// TODO: The result of the simulation seems to be tied to the window's height and width; figure out why and fix it.
// TODO: Replace std::vector with a custom bump allocator to allocate at most 1_000 planets or something.

bool init();
void close();
void draw_system(std::vector<Universe::CelestialBody> &solar_system);
void draw_body(Universe::CelestialBody &body);
std::vector<Universe::CelestialBody> create_solar_system();

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
				if (e.type == SDL_QUIT)
				{
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
	auto x = (int)floor(body.position.x);
	auto y = (int)floor(body.position.y);

	SDL_Rect rect = {x, y, BLOCK_WIDTH, BLOCK_HEIGHT};
	SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 0);
	SDL_RenderFillRect(gRenderer, &rect);
}

std::vector<Universe::CelestialBody> create_solar_system()
{
	std::vector<Universe::CelestialBody> solar_system;
	Universe::CelestialBody planet_one = Universe::CelestialBody(
		1000000,
		Vector2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2),
		ZERO_VECTOR,
		ZERO_VECTOR);

	Universe::CelestialBody planet_two = Universe::CelestialBody(
		10,
		Vector2(WINDOW_WIDTH / 3 * 2, WINDOW_HEIGHT / 2),
		Vector2(0, -20),
		ZERO_VECTOR);

	Universe::CelestialBody planet_three = Universe::CelestialBody(
		10,
		Vector2(WINDOW_WIDTH / 8 * 7, WINDOW_HEIGHT / 2),
		Vector2(0, 12),
		ZERO_VECTOR);

	solar_system.push_back(planet_one);
	solar_system.push_back(planet_two);
	solar_system.push_back(planet_three);

	return solar_system;
}
