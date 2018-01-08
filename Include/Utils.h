#pragma once

#include <Defines.h>
#include <Math/Vector3.h>
#include <Math/Vector2.h>
#include <Math/Matrix33.h>
#include <Math/Matrix44.h>
#include <Graphics/Color.h>

namespace Osm
{

/// Read an entire text file into a string. Only use for small files.
///
/// @filename   Full path to the file
///
std::string ReadFile(const std::string& filename);

/// Save text to a file
/// @filename   Full path to the file
/// @text       Text to save
///
bool SaveFile(const std::string& filename, const std::string& text);

/// String hash using sdbm algorithm
ullong StringHash(const std::string& str);

/// Replace all occurrences of the search string with the replacement string.
///
/// @param subject The string being searched and replaced on, otherwise known as the haystack.
/// @param search The value being searched for, otherwise known as the needle.
/// @param replace The replacement value that replaces found search values.
/// @return a new string with all occurances replaced.
///
std::string StringReplace(const std::string& subject,
	const std::string& search,
	const std::string& replace);

/// Determine whether or not a string starts with the given prefix. Does
/// not create an internal copy.
///
/// @param subject The string being searched in.
/// @param prefix The string to search for.
/// @return a boolean indicating if the prefix was found.
///
bool StringStartsWith(const std::string& subject,
	const std::string& prefix);

/// Determine whether or not a string ends with the given suffix. Does
/// not create an internal copy.
///
/// @param subject The string being searched in.
/// @param prefix The string to search for.
/// @return a boolean indicating if the suffix was found.
///
bool StringEndsWith(const std::string& subject,
	const std::string& suffix);

/// Spherical coordinates defined with radius and two angles
struct SphericalCoordinates
{
	/// Radius
	float r;

	/// Inclination
	float theta;

	/// Azimuth
	float fi;
};

inline Vector3 ToVector3(const Vector2& vec2) { return Vector3(vec2.x, 0.0f, vec2.y); }

inline Vector3 ToVector3(const Color& color) { return Vector3(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f); }

inline Vector2 ToVector2(const Vector3& vec2) { return Vector2(vec2.x, vec2.z); }

inline Vector4 ToVector4(const Color& color) { return Vector4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f); }

void UpdateMatrix44FromMatrix33(Matrix44& output, const Matrix33& input);

inline void SwitchOnBitFlag(uint& flags, uint bit) { flags |= bit; }

inline void SwitchOffBitFlag(uint& flags, uint bit) { flags &= (~bit); }

inline bool CheckBitFlag(uint flags, uint bit) { return (flags & bit) == bit; }

inline bool CheckBitFlagOverlap(uint flag0, uint flag1) { return (flag0 & flag1) != 0; }

/// Convert a cartesian 3D vector to spherical coordinates
SphericalCoordinates CartesianToSpherical(const Vector3& cartesianVector);

/// Converts from spherical coordinates to cartesian
Vector3 SphericalToCartesian(const SphericalCoordinates& sphericalCoord);

Vector3 RandomOnUnitSphere();


/// Does hermite spline interpolation of a real value
template <class T>
T HermiteInterpolation(T from, T to, float t, float m0, float m1)
{
	// Put t in [0,1]
	if (t < 0)
		t = 0;
	else if (t > 1)
		t = 1;

	// Interpolate
	float t2 = t * t;		// t squared
	float t3 = t2 * t;		// t cubbed
							//
	T val = (2 * t3 - 3 * t2 + 1) * from; // h00 * p0
	val += (t3 - 2 * t2 + t)   * m0;	// h10 * m0
	val += (-2 * t3 + 3 * t2)    * to;	// h01 * p1
	val += (t3 - t2)         * m1;  // h11 * m1
									//
	return val;
}

enum InterpolationType
{
	INTERPOLATION_LERP,
	INTERPOLATION_EASE_IN,
	INTERPOLATION_EASE_OUT,
	INTERPOLATION_EASE_IN_OUT,
	INTERPOLATION_EASE_MID,
	INTERPOLATION_SPRING
};


template <class T>
T Interpolate(T a, T b, float t, InterpolationType interpolation)
{
	// Put t in [0,1]
	if (t < 0)
		t = 0;
	else if (t > 1)
		t = 1;

	// Change the interpolation
	switch (interpolation)
	{
	case INTERPOLATION_SPRING:
	case INTERPOLATION_LERP:
		break;

	case INTERPOLATION_EASE_IN:
		t = HermiteInterpolation(0.0f, 1.0f, t, 0.0f, 1.0f);
		break;

	case INTERPOLATION_EASE_OUT:
		t = HermiteInterpolation(0.0f, 1.0f, t, 1.0f, 0.0f);
		break;

	case INTERPOLATION_EASE_IN_OUT:
		t = HermiteInterpolation(0.0f, 1.0f, t, 0.0f, 0.0f);
		break;

	case INTERPOLATION_EASE_MID:
		t = HermiteInterpolation(0.0f, 1.0f, t, 1.0f, 1.0f);
		break;
	}

	// Return a lerp with the
	return Lerp(a, b, t);
}


/// Clamps a value in a certain interval
///
/// @param val Value that will be clamped
/// @param from Low bound
/// @param to High bound
template <class T>
T Clamp(T val, T from, T to)
{
	T t = val;
	if (t < from)
		t = from;
	else if (t > to)
		t = to;

	return t;
}

}
