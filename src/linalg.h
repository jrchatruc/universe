#ifndef __LINALG__H_
#define __LINALG__H_

#define ZERO_VECTOR (Vector2(0, 0))

namespace LinAlg
{
	typedef struct Vector2
	{
		double x;
		double y;
		Vector2 operator+(Vector2 other);
		Vector2 &operator+=(const Vector2 &rhs);
		Vector2 operator-(Vector2 &other);
		Vector2 operator*(double scalar);
		Vector2 operator/(double scalar);
		Vector2()
		{
		}
		Vector2(double x_, double y_)
		{
			x = x_;
			y = y_;
		}
		Vector2(int x_, int y_)
		{
			x = (double)x_;
			y = (double)y_;
		}
	} Vector2;

	double dot(Vector2 v, Vector2 w);
	double distance(Vector2 v, Vector2 w);
	Vector2 normalize(Vector2 v);
} // namespace LinAlg
#endif
