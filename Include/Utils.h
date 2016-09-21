#pragma once

#include <string>
#include <Defines.h>
#include <Math/Vector3.h>
#include <Math/Vector2.h>
#include <Math/Matrix33.h>
#include <Math/Matrix44.h>

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

inline Vector2 ToVector2(const Vector3& vec2) { return Vector2(vec2.x, vec2.z); }

void UpdateMatrix44FromMatrix33(Matrix44& output, const Matrix33& input);

/// Convert a cartesian 3D vector to spherical coordinates
SphericalCoordinates CartesianToSpherical(const Vector3& cartesianVector);

/// Converts from spherical coordinates to cartesian
Vector3 SphericalToCartesian(const SphericalCoordinates& sphericalCoord);

Vector3 RandomOnUnitSphere();

}
