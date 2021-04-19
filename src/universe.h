#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <vector>

#include "linalg.h"

using LinAlg::Vector2;

extern const double G;

namespace Universe
{
	typedef struct CelestialBody
	{
		int id;
		double mass;
		Vector2 position;
		Vector2 velocity;
		Vector2 acceleration;

		void update_position(const double delta_t);
		void update_velocity(const double delta_t);
		Vector2 gravitational_force(CelestialBody &other);
		void update_acceleration(std::vector<CelestialBody> &solar_system);
		CelestialBody()
		{
		}
		CelestialBody(double _mass, Vector2 _position, Vector2 _velocity, Vector2 _acceleration) : mass(_mass), position(_position), velocity(_velocity), acceleration(_acceleration)
		{
			static int system_id_counter = 0;
			this->id = system_id_counter++;
		}
	} CelestialBody;

	void update_system(std::vector<CelestialBody> &solar_system, double delta_t);
	void print_body(CelestialBody body);
} // namespace Universe
#endif
