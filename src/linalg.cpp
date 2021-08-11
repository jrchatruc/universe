#include "linalg.h"

#include <math.h>
#include <stdio.h>

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

	Vector3 Vector3::operator+(Vector3 other)
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 &Vector3::operator+=(const Vector3 &rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	Vector3 Vector3::operator-(Vector3 &other)
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 Vector3::operator*(double scalar)
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	Vector3 Vector3::operator/(double scalar)
	{
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	double dot(Vector3 v, Vector3 w) { return v.x * w.x + v.y * w.y + v.z * w.z; }

	double distance(Vector3 v, Vector3 w) { return sqrt(dot(v - w, v - w)); }

	Vector3 cross(Vector3 v, Vector3 w)
	{
		Vector3 result;
		result.x = v.y * w.z - v.z * w.y;
		result.y = v.z * w.x - v.x * w.z;
		result.z = v.x * w.y - v.y * w.x;
		return result;
	}

	Vector3 normalize(Vector3 v)
	{
		double norm = distance(v, ZERO_VECTOR_3D);
		return v * (1 / norm);
	}

	void Vector3::print()
	{
		printf("(%f, %f, %f)\n", this->x, this->y, this->z);
	}

	Matrix3 Matrix3::operator+(Matrix3 other)
	{
		Matrix3 result;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				result._matrix[i][j] = _matrix[i][j] + other._matrix[i][j];
			}
		}
		return result;
	}

	Matrix3 &Matrix3::operator+=(const Matrix3 &rhs)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				this->_matrix[i][j] += rhs._matrix[i][j];
			}
		}
		return *this;
	}

	Matrix3 Matrix3::operator-(Matrix3 &other)
	{
		Matrix3 result;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				result._matrix[i][j] = _matrix[i][j] - other._matrix[i][j];
			}
		}
		return result;
	}

	Matrix3 Matrix3::operator*(Matrix3 other)
	{
		Matrix3 result;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				double value = 0;
				for (int k = 0; k < 3; k++)
				{
					value += _matrix[i][k] * other._matrix[k][j];
				}
				result._matrix[i][j] = value;
			}
		}
		return result;
	}

	Matrix3 &Matrix3::operator*=(Matrix3 &other)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				double value = 0;
				for (int k = 0; k < 3; k++)
				{
					value += _matrix[i][k] * other._matrix[k][j];
				}
				this->_matrix[i][j] = value;
			}
		}
		return *this;
	}

	Matrix3 Matrix3::operator*(double scalar)
	{
		Matrix3 result;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				result._matrix[i][j] = _matrix[i][j] * scalar;
			}
		}
		return result;
	}

	Matrix3 Matrix3::operator/(double scalar)
	{
		Matrix3 result;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				result._matrix[i][j] = _matrix[i][j] / scalar;
			}
		}
		return result;
	}

	Vector3 Matrix3::operator*(Vector3 vector)
	{
		Vector3 result;
		result.x = vector.x * _matrix[0][0] + vector.y * _matrix[0][1] + vector.z * _matrix[0][2];
		result.y = vector.x * _matrix[1][0] + vector.y * _matrix[1][1] + vector.z * _matrix[1][2];
		result.z = vector.x * _matrix[2][0] + vector.y * _matrix[2][1] + vector.z * _matrix[2][2];
		return result;
	}

	// This is obviously assuming that the matrix is a rotation (or at the very least invertible,
	// otherwise the result is not even a basis, let alone an orthonormal one)
	void Basis::rotate(Matrix3 matrix)
	{
		this->v_1 = matrix * v_1;
		this->v_2 = matrix * v_2;
		this->v_3 = matrix * v_3;
	}

	// Same comment as above
	void Basis::rotate(Vector3 axis, double rotation_angle)
	{
		auto rotation = get_rotation_matrix(axis, rotation_angle);
		this->rotate(rotation);
	}

	Vector3 Basis::coordinates(Vector3 vector)
	{
		Vector3 result;
		result.x = dot(vector, v_1);
		result.y = dot(vector, v_2);
		result.z = dot(vector, v_3);
		return result;
	}

	void Basis::print()
	{
		printf("{\n");
		v_1.print();
		v_2.print();
		v_3.print();
		printf("}\n");
	}

	// Source for this calculation https://math.stackexchange.com/questions/142821/matrix-for-rotation-around-a-vector
	static Matrix3 get_rotation_matrix(Vector3 axis, double angle)
	{
		auto W = Matrix3{{{0, -axis.z, axis.y},
						  {axis.z, 0, -axis.x},
						  {-axis.y, axis.x, 0}}};
		return IDENTITY_MATRIX + W * (sin(angle)) + (W * W) * (2 * pow(sin(angle / 2), 2));
	}
} // namespace LinAlg
