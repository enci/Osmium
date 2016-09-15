#pragma once

#include <cmath>

namespace Osm
{
///
/// A 3d vector
///
struct Vector3
{
	union
	{
		// Holds all the values 
		float f[3];

		struct
		{
			/// Holds the value along the x axis
			float x;

			/// Holds the value along the y axis
			float y;

			/// Holds the value along the z axis
			float z;
		};
	};

	/// The default constructor creates a zero vector.
	Vector3();

	/// Creates a vector with the given components
	Vector3(float x, float y, float z);

	/// Returns the value of the given vector added to this
	Vector3 operator+(const Vector3& v) const;

	/// Returns the value of the given vector subtracted from this
	Vector3 operator-(const Vector3& v) const;

	/// Returns a copy of this vector scaled the given value
	Vector3 operator*(const float value) const;

	/// Returns a copy of this vector scaled the inverse of the value
	Vector3 operator/(const float value) const;

	/// Returns the negative this vector
	Vector3 operator-() const;

	/// Calculates and returns the dot product of this
	/// with the given vector
	float operator *(const Vector3& vector) const;

	/// Adds the given vector to this
	void operator+=(const Vector3& v);

	/// Subtracts the given vector from this
	void operator-=(const Vector3& v);

	/// Multiplies this vector by the given scalar
	void operator*=(const float value);

	/// Calculates and returns the cross product of this vector
	/// with the given vector
	Vector3 Cross(const Vector3& vector) const;

	/// Calculates and returns the dot product of this vector
	/// with the given vector
	float Dot(const Vector3& vector) const;

	///  Gets the magnitude of this vector
	float Magnitude() const;

	///  Gets the squared magnitude of this vector
	float SquareMagnitude() const;

	/// Turns a non-zero vector into a vector of unit length
	void Normalize();


	/// Checks if the two vectors have identical components
	bool operator==(const Vector3& other) const;

	/// Checks if the two vectors have non-identical components
	bool operator!=(const Vector3& other) const;

	/// Zero all the components of the vector
	void Clear();

	Vector3 Slerp(float fact, const Vector3& r) const;
};

/// Multiplication with Rhs Vector
inline Vector3 operator*(float val, const Vector3& rhs)
{
	return rhs * val;
}

} // Osm

