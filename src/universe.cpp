#include "universe.h"

#include <math.h>
#include <stdio.h>

#include <vector>

#include "linalg.h"

using LinAlg::Vector2;

const double G = 0.05;

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

	Vector2 CelestialBody::gravitational_force(CelestialBody &other)
	{
		auto r = LinAlg::distance(position, other.position);
		auto force = (mass * other.mass * G) / pow(r, 2);
		auto direction = LinAlg::normalize(other.position - position);
		return direction * force;
	}

	void CelestialBody::update_acceleration(
		std::vector<CelestialBody> &solar_system)
	{
		auto acceleration = ZERO_VECTOR;
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
			"Id: %i\nMass: %f\nPosition: (%f, %f)\nVelocity: (%f, %f)\nAcceleration: "
			"(%f, %f)\n",
			body.id, body.mass, body.position.x, body.position.y, body.velocity.x,
			body.velocity.y, body.acceleration.x, body.acceleration.y);
	}
} // namespace Universe
