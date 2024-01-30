#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

// Constants
const double g_infinity = std::numeric_limits<double>::infinity();
const double g_pi = 3.1415926535897932385;

// Utility Functions
inline double DegreesToRadians(double degrees)
{
	return degrees * g_pi / 180.0;
}

inline double Clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline double RandomDouble()
{
	// Returns a random real in [0,1).
	return rand() / (RAND_MAX + 1.0);
}

inline double RandomDouble(double min, double max)
{
	// Returns a random real in [min,max).
	return min + (max - min) * RandomDouble();
}

inline int RandomInt(int min, int max) {
	// Returns a random integer in [min,max].
	return static_cast<int>(RandomDouble(min, max + 1));
}

// Common Headers
#include "Interval.h"
#include "Ray.h"
#include "Vector3.h"
