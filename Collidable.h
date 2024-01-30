#pragma once

#include "Common.h"
#include "AABB.h"

class Material;

struct CollideRecord
{
	Point3D m_point;
	Vector3 m_normal;
	std::shared_ptr<Material> m_pMaterial;
	double m_t = 0;
	double m_u;
	double m_v;
	bool m_frontFace = false;

	inline void SetFaceNormal(const Ray& ray, const Vector3& outwardNormal)
	{
		m_frontFace = Dot(ray.Direction(), outwardNormal) < 0;
		m_normal = m_frontFace ? outwardNormal : -outwardNormal;
	}
};

class Collidable
{
public:
	Collidable(){}
	virtual bool Collide(const Ray& inRay, Interval rayI, CollideRecord& record) const = 0;
	virtual AABB BoundingBox() const = 0;
};

struct Translate : public Collidable
{
	Translate(std::shared_ptr<Collidable> p, const Vector3& displacement)
		: m_object(p), m_offset(displacement)
	{
		m_bbox = m_object->BoundingBox() + m_offset;
	}

	virtual bool Collide(const Ray& inRay, Interval rayI, CollideRecord& record) const
	{
		// Move the ray backwards by the offset
		Ray offset_r(inRay.Origin() - m_offset, inRay.Direction(), inRay.Time());

		// Determine where (if any) an intersection occurs along the offset ray
		if (!m_object->Collide(offset_r, rayI, record))
			return false;

		// Move the intersection point forwards by the offset
		record.m_point += m_offset;

		return true;
	}

	virtual AABB BoundingBox() const override { return m_bbox; }

private:
	std::shared_ptr<Collidable> m_object;
	Vector3 m_offset;
	AABB m_bbox;
};

struct RotateY : public Collidable
{
public:
	RotateY(std::shared_ptr<Collidable> p, double angle) : m_object(p)
	{
		auto radians = DegreesToRadians(angle);
		m_sinTheta = std::sin(radians);
		m_cosTheta = std::cos(radians);
		m_bbox = m_object->BoundingBox();

		Point3D min(g_infinity, g_infinity, g_infinity);
		Point3D max(-g_infinity, -g_infinity, -g_infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i * m_bbox.x.m_max + (1 - i) * m_bbox.x.m_min;
					auto y = j * m_bbox.y.m_max + (1 - j) * m_bbox.y.m_min;
					auto z = k * m_bbox.z.m_max + (1 - k) * m_bbox.z.m_min;

					auto newx = m_cosTheta * x + m_sinTheta * z;
					auto newz = -m_sinTheta * x + m_cosTheta * z;

					Vector3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++) {
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
					}
				}
			}
		}

		m_bbox = AABB(min, max);
	}

	virtual bool Collide(const Ray& inRay, Interval rayI, CollideRecord& record) const
	{
		// Change the ray from world space to object space
		auto origin = inRay.Origin();
		auto direction = inRay.Direction();

		origin[0] = m_cosTheta * inRay.Origin()[0] - m_sinTheta * inRay.Origin()[2];
		origin[2] = m_sinTheta * inRay.Origin()[0] + m_cosTheta * inRay.Origin()[2];

		direction[0] = m_cosTheta * inRay.Direction()[0] - m_sinTheta * inRay.Direction()[2];
		direction[2] = m_sinTheta * inRay.Direction()[0] + m_cosTheta * inRay.Direction()[2];

		Ray rotated_r(origin, direction, inRay.Time());

		// Determine where (if any) an intersection occurs in object space
		if (!m_object->Collide(rotated_r, rayI, record))
			return false;

		// Change the intersection point from object space to world space
		auto p = record.m_point;
		p[0] = m_cosTheta * record.m_point[0] + m_sinTheta * record.m_point[2];
		p[2] = -m_sinTheta * record.m_point[0] + m_cosTheta * record.m_point[2];

		// Change the normal from object space to world space
		auto normal = record.m_normal;
		normal[0] = m_cosTheta * record.m_normal[0] + m_sinTheta * record.m_normal[2];
		normal[2] = -m_sinTheta * record.m_normal[0] + m_cosTheta * record.m_normal[2];

		record.m_point = p;
		record.m_normal = normal;

		return true;
	}

	virtual AABB BoundingBox() const override { return m_bbox; }

private:
	std::shared_ptr<Collidable> m_object;
	double m_sinTheta;
	double m_cosTheta;
	AABB m_bbox;
};