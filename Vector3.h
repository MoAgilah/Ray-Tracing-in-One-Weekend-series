#pragma once

#include <cmath>
#include <iostream>

class Vector3
{
public:
	Vector3() : m_elements{ 0,0,0 } {}
	Vector3(double e0, double e1, double e2) : m_elements{ e0, e1, e2 } {}

	inline double X() const { return m_elements[0]; }
	inline double Y() const { return m_elements[1]; }
	inline double Z() const { return m_elements[2]; }

	Vector3 operator-() const { return Vector3(-m_elements[0], -m_elements[1], -m_elements[2]); }
	double operator[](int i) const { return m_elements[i]; }
	double& operator[](int i) { return m_elements[i]; }

	Vector3& operator+=(const Vector3& v)
	{
		m_elements[0] += v.X();
		m_elements[1] += v.Y();
		m_elements[2] += v.Z();
		return *this;
	}

	Vector3& operator*=(const double t)
	{
		m_elements[0] *= t;
		m_elements[1] *= t;
		m_elements[2] *= t;
		return *this;
	}

	Vector3& operator/=(const double t)
	{
		return *this *= 1 / t;
	}

	double Length() const
	{
		return std::sqrt(LengthSquared());
	}

	double LengthSquared() const
	{
		return m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2];
	}

	bool NearZero() const
	{
		// Return true if the vector is close to zero in all dimensions.
		const auto s = 1e-8;
		return (std::fabs(m_elements[0]) < s) && (std::fabs(m_elements[1]) < s) && (std::fabs(m_elements[2]) < s);
	}

	inline static Vector3 Random()
	{
		return Vector3(RandomDouble(), RandomDouble(), RandomDouble());
	}

	inline static Vector3 Random(double min, double max)
	{
		return Vector3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
	}

private:
	double m_elements[3];
};

// Type aliases for Vector3
using Point3D = Vector3;   // 3D point
using Colour = Vector3;    // RGB Colour

// Vector3 Utility Functions
inline std::ostream& operator<<(std::ostream& out, const Vector3& v)
{
	return out << v.X() << ' ' << v.Y() << ' ' << v.Z();
}

inline Vector3 operator+(const Vector3& u, const Vector3& v)
{
	return Vector3(u.X() + v.X(), u.Y() + v.Y(), u.Z() + v.Z());
}

inline Vector3 operator-(const Vector3& u, const Vector3& v)
{
	return Vector3(u.X() - v.X(), u.Y() - v.Y(), u.Z() - v.Z());
}

inline Vector3 operator*(const Vector3& u, const Vector3& v)
{
	return Vector3(u.X() * v.X(), u.Y() * v.Y(), u.Z() * v.Z());
}

inline Vector3 operator*(double t, const Vector3& v)
{
	return Vector3(t * v.X(), t * v.Y(), t * v.Z());
}

inline Vector3 operator*(const Vector3& v, double t)
{
	return t * v;
}

inline Vector3 operator/(Vector3 v, double t)
{
	return (1 / t) * v;
}

inline double Dot(const Vector3& u, const Vector3& v)
{
	return	u.X() * v.X()
		+	u.Y() * v.Y()
		+	u.Z() * v.Z();
}

inline Vector3 Cross(const Vector3& u, const Vector3& v)
{
	return Vector3(u.Y() * v.Z() - u.Z() * v.Y(),
				u.Z() * v.X() - u.X() * v.Z(),
				u.X() * v.Y() - u.Y() * v.X());
}

inline Vector3 UnitVector(Vector3 v)
{
	return v / v.Length();
}

inline Vector3 RandomInUnitSphere()
{
	while (true)
	{
		auto point3D = Vector3::Random(-1, 1);
		if (point3D.LengthSquared() < 1)
			return point3D;
	}
}

inline Vector3 RandomUnitVector()
{
	return UnitVector(RandomInUnitSphere());
}

inline Vector3 RandomInHemisphere(const Vector3& normal)
{
	Vector3 inUnitSphere = RandomUnitVector();
	if (Dot(inUnitSphere, normal) > 0.0) // In the same hemisphere as the normal
		return inUnitSphere;
	else
		return -inUnitSphere;
}

inline Vector3 RandomInUnitDisk()
{
	while (true)
	{
		auto point = Vector3(RandomDouble(-1, 1), RandomDouble(-1, 1), 0);
		if (point.LengthSquared() < 1)
			return point;
	}
}

inline Vector3 Reflect(const Vector3& v, const Vector3& n)
{
	/* fig.11 ray reflection
		 N   R
		 |  /\
	  v\ |  / |
		\| / /|\B
	-----\----|
		 v\	 /|\B
		   \  |
		   \/
		   R = v + 2b. where N is the unit vector,
		   but V might not be,
		   The length should be v.n.
		   Because v points in, we need a minus sign
	*/
	return v - 2 * Dot(v, n) * n;
}

inline Vector3 Refract(const Vector3& uv, const Vector3& n, double etaiOverEtat)
{
	/*
		in order to determine the direction of the refracted ray, we have to solve for sin θ′:
			sin θ′= n / n′ * sin θ
		On the refracted side of the surface their is a refracted ray R' and a normal N' and there exists an angle θ′ between then.
		We can split R' into the parts of the ray that are perpendicular or parallel to n':
			R′=R′⊥+R′∥
		if we solve for R′⊥  and R′∥ we get:
			R′⊥= η / η′(R+cosθn)
				   __________
			R′∥=−√1−|R′⊥|^2n
		We still need to solve for cosθ, dot products of two vectors can be explained in the terms of the cosine of the angle between them:
			a* b = |a|b|cosθ
		Restricting a and b to unit vectors gives:
			a * b = cosθ
		We can now rewrite R`⊥ in the terms of the known quantities:
			R′⊥=η / η′(R+(−R⋅n)n)
		Combining it back together we can write a function to calculate R`
	*/
	auto cosTheta = fmin(Dot(-uv, n), 1.0);
	Vector3 rOutPerp = etaiOverEtat * (uv + cosTheta * n);
	Vector3 rOutParallel = -std::sqrt(std::fabs(1.0 - rOutPerp.LengthSquared())) * n;
	return rOutPerp + rOutParallel;
}