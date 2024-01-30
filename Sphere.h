#pragma once

#include "Common.h"
#include "Collidable.h"

class Sphere : public Collidable
{
public:
	Sphere() : m_center(Vector3()), m_radius(0) {}

	Sphere(Point3D center, double radius, std::shared_ptr<Material> material)
		: m_center(center), m_radius(radius), m_pMaterials(material), m_isMoving(false)
	{
		auto rvec = Vector3(radius, radius, radius);
		m_bbox = AABB(m_center - rvec, m_center + rvec);
	};

	Sphere(Point3D _center1, Point3D _center2, double _radius, std::shared_ptr<Material> _material)
		: m_center(_center1), m_radius(_radius), m_pMaterials(_material), m_isMoving(true)
	{
		auto rvec = Vector3(m_radius, m_radius, m_radius);
		AABB box1(_center1 - rvec, _center1 + rvec);
		AABB box2(_center2 - rvec, _center2 + rvec);
		m_bbox = AABB(box1, box2);
		m_centerVec = _center2 - _center1;
	}

	virtual bool Collide(const Ray& ray, Interval rayI, CollideRecord& record) const override;
	virtual AABB BoundingBox() const override { return m_bbox; }
private:
	Point3D Center(double time) const
	{
		// Linearly interpolate from center1 to center2 according to time, where t=0 yields
		// center1, and t=1 yields center2.
		return m_center + time * m_centerVec;
	}

	static void GetSphereUV(const Point3D& p, double& u, double& v) {
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		auto theta = acos(-p.Y());
		auto phi = atan2(-p.Z(), p.X()) + g_pi;

		u = phi / (2 * g_pi);
		v = theta / g_pi;
	}

	AABB m_bbox;
	bool m_isMoving;
	double m_radius;
	Point3D m_center;
	Vector3 m_centerVec;
	std::shared_ptr<Material> m_pMaterials;
};

bool Sphere::Collide(const Ray& ray, Interval rayI, CollideRecord& record) const
{
	/*
		First, recall that a Vector dotted with itself is equal to the squared length of that vector
		Second, notice how the equation for b has a factor of two in it, consider b = 2h
			 _______				 __________				  ______
		−b± √b^2−4ac			−2h±√(2h)^2−4ac			−2h±2√h^2−ac
		−−−−−−−----		=		---------------		=	-----------------
			2a							2a						2a
							   ______
						=  −h±√h^2−ac
						   ----------
								a
		Using the above observation, we can now simplify the sphere-intersection to as follows:
	*/
	Point3D center = m_isMoving ? Center(ray.Time()) : m_center;
	Vector3 oc = ray.Origin() - center;
	auto a = ray.Direction().LengthSquared();
	auto halfB = Dot(oc, ray.Direction());
	auto c = oc.LengthSquared() - m_radius * m_radius;
	auto discriminant = halfB * halfB - a * c;

	if (discriminant < 0)
		return false;
	auto sqrtD = std::sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range.
	auto root = (-halfB - sqrtD) / a;
	if (!rayI.surrounds(root))
	{
		root = (-halfB + sqrtD) / a;
		if (!rayI.surrounds(root))
			return false;
	}

	record.m_t = root;
	record.m_point = ray.At(record.m_t);
	Vector3 outwardNormal = (record.m_point - m_center) / m_radius;
	record.SetFaceNormal(ray, outwardNormal);
	GetSphereUV(outwardNormal, record.m_u, record.m_v);
	record.m_pMaterial = m_pMaterials;

	return true;
}