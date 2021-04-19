#include "linalg.h"

#include <math.h>

namespace LinAlg
{
	Vector2 Vector2::operator+(Vector2 other)
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 &Vector2::operator+=(const Vector2 &rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	Vector2 Vector2::operator-(Vector2 &other)
	{
		return Vector2(x - other.x, y - other.y);
	}

	Vector2 Vector2::operator*(double scalar)
	{
		return Vector2(x * scalar, y * scalar);
	}

	Vector2 Vector2::operator/(double scalar)
	{
		return Vector2(x / scalar, y / scalar);
	}

	double dot(Vector2 v, Vector2 w) { return v.x * w.x + v.y * w.y; }

	double distance(Vector2 v, Vector2 w) { return sqrt(dot(v - w, v - w)); }

	Vector2 normalize(Vector2 v)
	{
		double norm = distance(v, ZERO_VECTOR);
		return v * (1 / norm);
	}
} // namespace LinAlg
