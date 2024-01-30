#pragma once

#include "Common.h"

class AABB {
public:
	Interval x, y, z;

	AABB() {} // The default AABB is empty, since Intervals are empty by default.

	AABB(const Interval& ix, const Interval& iy, const Interval& iz)
		: x(ix), y(iy), z(iz)
	{
		pad_to_minimums();
	}

	AABB(const Point3D& a, const Point3D& b) {
		// Treat the two points a and b as extrema for the bounding box, so we don't require a
		// particular minimum/maximum coordinate order.
		x = Interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
		y = Interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
		z = Interval(fmin(a[2], b[2]), fmax(a[2], b[2]));

		pad_to_minimums();
	}

	AABB(const AABB& box0, const AABB& box1) {
		x = Interval(box0.x, box1.x);
		y = Interval(box0.y, box1.y);
		z = Interval(box0.z, box1.z);
	}

	AABB Pad()
	{
		// Return an AABB that has no side narrower than some delta, padding if neccsary.
		double delta = 0.0001;
		Interval newX = (x.size() >= delta ? x : x.expand(delta));
		Interval newY = (y.size() >= delta ? y : y.expand(delta));
		Interval newZ = (z.size() >= delta ? z : z.expand(delta));

		return AABB(newX, newY, newZ);
	}

	const Interval& axis(int n) const {
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	bool Collide(const Ray& r, Interval rayI) const {
		for (int a = 0; a < 3; a++) {
			auto invD = 1 / r.Direction()[a];
			auto orig = r.Origin()[a];

			auto t0 = (axis(a).m_min - orig) * invD;
			auto t1 = (axis(a).m_max - orig) * invD;

			if (invD < 0)
				std::swap(t0, t1);

			if (t0 > rayI.m_min) rayI.m_min = t0;
			if (t1 < rayI.m_max) rayI.m_max = t1;

			if (rayI.m_max <= rayI.m_min)
				return false;
		}
		return true;
	}

	int longest_axis() const {
		// Returns the index of the longest axis of the bounding box.

		if (x.size() > y.size())
			return x.size() > z.size() ? 0 : 2;
		else
			return y.size() > z.size() ? 1 : 2;
	}

	static const AABB empty, universe;

private:

	void pad_to_minimums() {
		// Adjust the AABB so that no side is narrower than some delta, padding if necessary.

		double delta = 0.0001;
		if (x.size() < delta) x = x.expand(delta);
		if (y.size() < delta) y = y.expand(delta);
		if (z.size() < delta) z = z.expand(delta);
	}
};

namespace {
	AABB operator+(const AABB& bbox, const Vector3& offset) {
		return AABB(bbox.x + offset.X(), bbox.y + offset.Y(), bbox.z + offset.Z());
	}

	AABB operator+(const Vector3& offset, const AABB& bbox) {
		return bbox + offset;
	}
}