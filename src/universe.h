#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <vector>

#include "linalg.h"

using LinAlg::Vector3;

extern const double G;

namespace Universe
{
	typedef struct CelestialBody
	{
		int id;
		double mass;
		Vector3 position;
		Vector3 velocity;
		Vector3 acceleration;

		void update_position(const double delta_t);
		void update_velocity(const double delta_t);
		Vector3 gravitational_force(CelestialBody &other);
		void update_acceleration(std::vector<CelestialBody> &solar_system);
	} CelestialBody;

	CelestialBody create_body(double mass, Vector3 position, Vector3 velocity, Vector3 acceleration);

	std::vector<Universe::CelestialBody> create_solar_system();
	void update_system(std::vector<CelestialBody> &solar_system, double delta_t);
	void print_body(CelestialBody body);
} // namespace Universe
#endif
