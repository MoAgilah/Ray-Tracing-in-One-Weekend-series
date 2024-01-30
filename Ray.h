#pragma once

#include "Vector3.h"

class Ray {
public:
	Ray() {}
	Ray(const Point3D& origin, const Vector3& direction)
		: m_origin(origin), m_direction(direction), m_time(0.0)
	{}
	Ray(const Point3D& origin, const Vector3& direction, double time = 0.0)
		: m_origin(origin), m_direction(direction), m_time(time)
	{}

	Point3D Origin() const { return m_origin; }
	Vector3 Direction() const { return m_direction; }
	double Time() const { return m_time; }

	Point3D At(double t) const
	{
		/*
			P(t)=A+tb
			where
				P == Point3D
				A == ray's origin
				b == ray's direction
				t == real number*
			* plug in different t and P(t) moves point along the ray.
				* negative t you can go anywhere on the 3D line
				* positive t you get only the parts in front of A; half-line or ray
		  t=  -1  0  1  2
			<--|--|--|--|-->
				  --->
				 A   b
		*/
		return m_origin + t * m_direction;
	}

private:
	Point3D m_origin;
	Vector3 m_direction;
	double m_time;
};