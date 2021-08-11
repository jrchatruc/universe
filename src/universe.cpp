#include <math.h>
#include <stdio.h>
#include <vector>

#include "universe.h"
#include "linalg.h"

using LinAlg::Vector3;

const double G = 0.00005;

namespace Universe
{
	void CelestialBody::update_position(const double delta_t)
	{
		this->position += velocity * delta_t;
	}

	void CelestialBody::update_velocity(const double delta_t)
	{
		this->velocity += acceleration * delta_t;
	}

	Vector3 CelestialBody::gravitational_force(CelestialBody &other)
	{
		auto r = LinAlg::distance(position, other.position);
		auto force = (mass * other.mass * G) / pow(r, 2);
		auto direction = LinAlg::normalize(other.position - position);
		return direction * force;
	}

	void CelestialBody::update_acceleration(
		std::vector<CelestialBody> &solar_system)
	{
		auto acceleration = ZERO_VECTOR_3D;
		for (auto &other_body : solar_system)
		{
			if (other_body.id != id)
			{
				auto force = this->gravitational_force(other_body);
				acceleration += (force / mass);
			}
		}
		this->acceleration = acceleration;
	}

	void update_system(std::vector<CelestialBody> &solar_system, double delta_t)
	{
		for (auto &body : solar_system)
		{
			body.update_position(delta_t);
			body.update_velocity(delta_t);
		}

		for (auto &body : solar_system)
		{
			body.update_acceleration(solar_system);
		}
	}

	void print_body(CelestialBody body)
	{
		printf(
			"Id: %i\nMass: %f\nPosition: (%f, %f, %f)\nVelocity: (%f, %f, %f)\nAcceleration: "
			"(%f, %f, %f)\n",
			body.id, body.mass, body.position.x, body.position.y, body.position.z, body.velocity.x,
			body.velocity.y, body.velocity.z, body.acceleration.x, body.acceleration.y, body.acceleration.z);
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
} // namespace Universe
