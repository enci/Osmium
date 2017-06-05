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

	/// Returns the normalised version of a vector
	Vector3 Unit() const;

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


  /// The default constructor creates a zero vector.
inline Osm::Vector3::Vector3() : x(0), y(0), z(0) {}

/// Creates a vector with the given components
inline Osm::Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z) {}

/// Returns the value of the given vector added to this
inline Osm::Vector3 Osm::Vector3::operator+(const Vector3& v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

/// Returns the value of the given vector subtracted from this
inline Osm::Vector3 Osm::Vector3::operator-(const Vector3& v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

/// Returns a copy of this vector scaled the given value
inline Osm::Vector3 Osm::Vector3::operator*(const float value) const
{
	return Vector3(x*value, y*value, z*value);
}

/// Returns a copy of this vector scaled the inverse of the value
inline Osm::Vector3 Osm::Vector3::operator/(const float value) const
{
	return Vector3(x / value, y / value, z / value);
}

/// Returns the negative this vector
inline Osm::Vector3 Osm::Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

/// Calculates and returns the dot product of this
/// with the given vector
inline float Osm::Vector3::operator *(const Vector3 &vector) const
{
	return x*vector.x + y*vector.y + z*vector.z;
}

/// Adds the given vector to this
inline void Osm::Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

/// Subtracts the given vector from this
inline void Osm::Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

/// Multiplies this vector by the given scalar
inline void Osm::Vector3::operator*=(const float value)
{
	x *= value;
	y *= value;
	z *= value;
}

/// Calculates and returns the cross product of this vector
/// with the given vector
inline Osm::Vector3 Osm::Vector3::Cross(const Vector3 &vector) const
{
	return Vector3(y*vector.z - z*vector.y,
		z*vector.x - x*vector.z,
		x*vector.y - y*vector.x);
}

/// Calculates and returns the dot product of this vector
/// with the given vector
inline float Osm::Vector3::Dot(const Vector3 &vector) const
{
	return x*vector.x + y*vector.y + z*vector.z;
}

///  Gets the magnitude of this vector
inline float Osm::Vector3::Magnitude() const
{
	return sqrtf(x*x + y*y + z*z);
}

///  Gets the squared magnitude of this vector
inline float Osm::Vector3::SquareMagnitude() const
{
	return x*x + y*y + z*z;
}

/// Turns a non-zero vector into a vector of unit length
inline void Osm::Vector3::Normalize()
{
	float l = Magnitude();
	if (l > 0)
	{
		(*this) *= 1.0f / l;
	}
}

inline Osm::Vector3 Osm::Vector3::Unit() const
{
	Vector3 result = *this;	// Copy ctor
	result.Normalize();
	return result;
}


/// Checks if the two vectors have identical components
inline bool Osm::Vector3::operator==(const Vector3& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

/// Checks if the two vectors have non-identical components
inline bool Osm::Vector3::operator!=(const Vector3& other) const
{
	return !(*this == other);
}

/// Zero all the components of the vector
inline void Osm::Vector3::Clear()
{
	x = y = z = 0;
}

inline Osm::Vector3 Osm::Vector3::Slerp(float fact, const Vector3& r) const
{
	float cosAlpha, alpha, sinAlpha;
	Vector3 result;

	// get cosine of angle between vectors (-1 -> 1)
	cosAlpha = this->Dot(r);

	// get angle (0 -> pi)
	alpha = acosf(cosAlpha);

	// get sine of angle between vectors (0 -> 1)
	sinAlpha = sinf(alpha);

	// simply perform linear interpolation if sinAlpha = 0
	if (sinAlpha == 0)
		return (*this) + (r - (*this)) * fact;

	// this breaks down when sinAlpha = 0, i.e. alpha = 0 or pi
	float t1, t2;
	t1 = sinf((1.0f - fact) * alpha) / sinAlpha;
	t2 = sinf(fact * alpha) / sinAlpha;

	// interpolate src vectors
	return *this * t1 + r * t2;
}

