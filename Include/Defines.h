#pragma once

#include <cmath>

// Define the ASSERT and DEBUG macros
#ifdef _WIN32
	#define ASSERT _ASSERT
	#ifdef _DEBUG 
		#define DEBUG
	#endif
#endif

#define INSPECTOR

namespace Osm
{

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long long ullong;

const float Pi = 3.1415926535897932384626433832795f;
const float TwoPi = Pi * 2;
const float HalfPi = Pi / 2;
const float QuarterPi = Pi / 4;

inline float DegToRad(float deg) { return deg * Pi / 180.0f; }

/// Returns a random float between zero and 1
inline float RandFloat() { return static_cast<float>((rand()) / (RAND_MAX + 1.0)); }

/// Returns a random float between x and y
inline float RandInRange(float x, float y) { return x + RandFloat()*(y - x); }

/// Returns a random int between from and to
inline int RandInRange(int from, int to) { return from + rand() % (to - from); }

/// Modulo that works with negative number as well
template <class T>
inline T Modulo(T x, T m) { return (x % m + m) % m; }

/// Quick (and not ugly) square
template <class T>
inline T Sqr(T x) { return x * x; }

// https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
///  Branchless, type-safe sign function
template <typename T>
int Sign(T val) { return (T(0) < val) - (val < T(0)); }

template <class T>
T Lerp(T a, T b, float t)
{
	if (t < 0)
		t = 0;
	else if (t > 1)
		t = 1;
	return a * (1.0f - t) + b * t;
}

inline float SphereVolume(float r)
{
	return 4.0f / 3.0f * r * r * r * Pi;
}

inline float SphereRaduis(float V)
{
	float t = (V * 3.0f) / (4.0f * Pi);
	return pow(t, 1.0f / 3.0f);
}

inline float DiskArea(float r)
{
	return r * r * Pi;
}

inline float DiskRadius(float A)
{
	return sqrt(A / Pi);
}

#ifdef DEBUG

#define LOG(macro_format, ...)										\
{																	\
	printf("[Osmium] ");												\
	printf(macro_format, ##__VA_ARGS__);							\
    const size_t macro_len = strlen(macro_format);					\
    if(macro_len > 0 && macro_format[macro_len - 1] != '\n')		\
	{																\
        printf("\n");												\
    }																\
}

#else
#define LOG(...) { }
#endif

}