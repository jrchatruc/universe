#ifndef __LINALG__H_
#define __LINALG__H_

#define ZERO_VECTOR (Vector2{0., 0.})
#define ZERO_VECTOR_3D (Vector3{0., 0., 0.})
#define IDENTITY_MATRIX \
	(Matrix3{{{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}}})

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
	} Vector2;

	double dot(Vector2 v, Vector2 w);
	double distance(Vector2 v, Vector2 w);
	Vector2 normalize(Vector2 v);

	typedef struct Vector3
	{
		double x;
		double y;
		double z;
		Vector3 operator+(Vector3 other);
		Vector3 &operator+=(const Vector3 &rhs);
		Vector3 operator-(Vector3 &other);
		Vector3 operator*(double scalar);
		Vector3 operator/(double scalar);
		void print();
	} Vector3;

	double dot(Vector3 v, Vector3 w);
	double distance(Vector3 v, Vector3 w);
	Vector3 cross(Vector3 v, Vector3 w);
	Vector3 normalize(Vector3 v);

	typedef struct Matrix3
	{
		double _matrix[3][3];
		Matrix3 operator+(Matrix3 other);
		Matrix3 &operator+=(const Matrix3 &rhs);
		Matrix3 operator-(Matrix3 &other);
		Matrix3 operator*(Matrix3 other);
		Matrix3 &operator*=(Matrix3 &other);
		Matrix3 operator*(double scalar);
		Matrix3 operator/(double scalar);
		Vector3 operator*(Vector3 vector);
	} Matrix3;
	typedef struct Basis
	{
		Vector3 v_1;
		Vector3 v_2;
		Vector3 v_3;
		void rotate(Matrix3 matrix);
		void rotate(Vector3 axis, double rotation_angle);
		Vector3 coordinates(Vector3 vector);
		void print();
	} Basis;

} // namespace LinAlg
#endif
