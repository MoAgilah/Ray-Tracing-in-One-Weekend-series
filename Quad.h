#pragma once

#include "Common.h"
#include "Collidable.h"

class Quad : public Collidable
{
public:
	Quad(const Point3D& _Q, const Vector3& _u, const Vector3& _v, std::shared_ptr<Material> m)
		: m_Q(_Q), m_u(_u), m_v(_v), m_mat(m)
	{
		/*
			Finding the equation for the plane that contains the quad. We have quadrilateral
			parameters Q, u, and v, and want the corresponding equation of the plane containing
			the quad defined by these three values.

			Recall that in the equation Ax + By + Cz = D, (A,B,C) represents the normal vector.
			To get this, we just use the cross product of the two side vectors u and v:
				n= unit_vector(u×v)
:
			The plane is defined as all points (x,y,z) that satisfy the equation Ax+By+Cz=D
.			Well, we know that Q lies on the plane, so that's enough to solve for D
:				D = nxQx + nyQy + nzQz
				= n⋅Q
		*/
		auto n = Cross(m_u, m_v);
		m_normal = UnitVector(n);
		m_d = Dot(m_normal, m_Q);

		// w = n/⋅(u×v) = n / n⋅n
		m_w = n / Dot(n, n);

		m_bbox = AABB(m_Q, m_Q + m_u + m_v).Pad();
	}

	virtual AABB BoundingBox() const override
	{
		return m_bbox;
	}

	virtual bool Collide(const Ray& inRay, Interval rayI, CollideRecord& record) const override
	{
		/*
			Here's an intuitive way to think of this formula: given the plane perpendicular to the
			normal vector n = (A,B,C), and the position vector v = (x,y,z) (that is, the vector from
			the origin to any point on the plane), then we can use the dot product to solve for D:
				n . v = D

			or any position on the plane. This is an equivalent formulation of the Ax + By + Cz =D
			formula given above, only now in terms of vectors.

			Now to find the intersection with some ray R(t) = P + td. Plugging in the ray equation,
			we get:
				n⋅(P + td) =D

			Solving for t
:				n⋅P + n⋅td = D

				n⋅P + t(n⋅d) = D
				t= D − n⋅P / n⋅d

			This gives us t, which we can plug into the ray equation to find the point of intersection.

			Note that the denominator n⋅d will be zero if the ray is parallel to the plane. In this case,
			we can immediately record a miss between the ray and the plane. As for other primitives, if
			the ray t parameter is less than the minimum acceptable value, we also record a miss.
		*/
		auto denom = Dot(m_normal, inRay.Direction());

		// No hit if the ray is parallel to the plane.
		if (fabs(denom) < 1e-8)
			return false;

		// Return false if the hit point parameter t is outside the ray interval.
		auto t = (m_d - Dot(m_normal, inRay.Origin())) / denom;
		if (!rayI.contains(t))
			return false;

		// Determine the hit point lies within the planar shape using its plane coordinates.
		auto intersection = inRay.At(t);
		Vector3 planar_hitpt_vector = intersection - m_Q;

		/*
			α = w⋅(p×v)
			β = w⋅(u×p)
		*/
		auto alpha = Dot(m_w, Cross(planar_hitpt_vector, m_v));
		auto beta = Dot(m_w, Cross(m_u, planar_hitpt_vector));

		if (!IsInterior(alpha, beta, record))
			return false;

		record.m_t = t;
		record.m_point = intersection;
		record.m_pMaterial = m_mat;
		record.SetFaceNormal(inRay, m_normal);

		return true;
	}

	virtual bool IsInterior(double a, double b, CollideRecord& rec) const
	{
		// Given the hit point in plane coordinates, return false if it is outside the
		// primitive, otherwise set the hit record UV coordinates and return true.
		if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
			return false;

		rec.m_u = a;
		rec.m_v = b;
		return true;
	}
private:
	Point3D m_Q;
	Vector3 m_u, m_v;
	std::shared_ptr<Material> m_mat;
	AABB m_bbox;
	Vector3 m_normal;
	double m_d;
	Vector3 m_w;
};