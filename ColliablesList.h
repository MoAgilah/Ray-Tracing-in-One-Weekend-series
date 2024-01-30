#pragma once

#include "Common.h"
#include "Sphere.h"
#include "AABB.h"
#include "Quad.h"

#include <memory>
#include <vector>
#include <execution>
#include <algorithm>

class CollidableList : public Collidable
{
public:
	CollidableList() {}
	CollidableList(std::shared_ptr<Collidable> object) { Add(object); }

	void Clear() { m_objects.clear(); }
	void Add(std::shared_ptr<Collidable> object)
	{
		m_objects.push_back(object);
		m_bbox = AABB(m_bbox, object->BoundingBox());
	}

	virtual bool Collide(const Ray& ray, Interval rayI, CollideRecord& record) const override;
	virtual AABB BoundingBox() const override { return m_bbox; }
public:
	std::vector<std::shared_ptr<Collidable>> m_objects;
private:
	AABB m_bbox;
};

bool CollidableList::Collide(const Ray& r, Interval rayI, CollideRecord& record) const
{
	CollideRecord tempRecord;
	auto collided = false;
	auto closestSoFar = rayI.m_max;

	std::for_each(m_objects.begin(), m_objects.end(), [&](auto&object)
		{
			if (object->Collide(r, Interval(rayI.m_min, closestSoFar), tempRecord))
			{
				collided = true;
				closestSoFar = tempRecord.m_t;
				record = tempRecord;
			}
		});

	return collided;
}

inline std::shared_ptr<CollidableList> Box(const Point3D& a, const Point3D& b, std::shared_ptr<Material> mat)
{
	// Returns the 3D box (six sides) that contains the two opposite vertices a & b.

	auto sides = std::make_shared<CollidableList>();

	// Construct the two opposite vertices with the minimum and maximum coordinates.
	auto min = Point3D(fmin(a.X(), b.X()), fmin(a.Y(), b.Y()), fmin(a.Z(), b.Z()));
	auto max = Point3D(fmax(a.X(), b.X()), fmax(a.Y(), b.Y()), fmax(a.Z(), b.Z()));

	auto dx = Vector3(max.X() - min.X(), 0, 0);
	auto dy = Vector3(0, max.Y() - min.Y(), 0);
	auto dz = Vector3(0, 0, max.Z() - min.Z());

	sides->Add(std::make_shared<Quad>(Point3D(min.X(), min.Y(), max.Z()), dx, dy, mat)); // front
	sides->Add(std::make_shared<Quad>(Point3D(max.X(), min.Y(), max.Z()), -dz, dy, mat)); // right
	sides->Add(std::make_shared<Quad>(Point3D(max.X(), min.Y(), min.Z()), -dx, dy, mat)); // back
	sides->Add(std::make_shared<Quad>(Point3D(min.X(), min.Y(), min.Z()), dz, dy, mat)); // left
	sides->Add(std::make_shared<Quad>(Point3D(min.X(), max.Y(), max.Z()), dx, -dz, mat)); // top
	sides->Add(std::make_shared<Quad>(Point3D(min.X(), min.Y(), min.Z()), dx, dz, mat)); // bottom

	return sides;
}