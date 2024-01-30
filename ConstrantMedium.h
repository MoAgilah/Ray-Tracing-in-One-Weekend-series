#pragma once

#include "Common.h"

#include "Collidable.h"
#include "Material.h"
#include "Texture.h"

class ConstantMedium : public Collidable
{
public:
	ConstantMedium(std::shared_ptr<Collidable> b, double d, std::shared_ptr<Texture> a)
		: m_boundary(b), m_negInvDensity(-1/d), m_phaseFunction(std::make_shared<Isotropic>(a))
	{}

	ConstantMedium(std::shared_ptr<Collidable> b, double d, Colour c)
		: m_boundary(b), m_negInvDensity(-1 / d), m_phaseFunction(std::make_shared<Isotropic>(c))
	{

	}

	virtual bool Collide(const Ray& inRay, Interval rayI, CollideRecord& record) const
	{
		// Print occasional samples when debugging. To enable, set enableDebug true.
		const bool enableDebug = false;
		const bool debugging = enableDebug && RandomDouble() < 0.00001;

		CollideRecord rec1, rec2;

		if (!m_boundary->Collide(inRay, universe, rec1))
			return false;

		if (!m_boundary->Collide(inRay, Interval(rec1.m_t + 0.0001, g_infinity), rec2))
			return false;

		if (debugging) std::clog << "\nray_tmin=" << rec1.m_t << ", ray_tmax=" << rec2.m_t << '\n';

		if (rec1.m_t < rayI.m_min) rec1.m_t = rayI.m_min;
		if (rec2.m_t > rayI.m_max) rec2.m_t = rayI.m_max;

		if (rec1.m_t >= rec2.m_t)
			return false;

		if (rec1.m_t < 0)
			rec1.m_t = 0;

		auto ray_length = inRay.Direction().Length();
		auto distance_inside_boundary = (rec2.m_t - rec1.m_t) * ray_length;
		auto hit_distance = m_negInvDensity * log(RandomDouble());

		if (hit_distance > distance_inside_boundary)
			return false;

		record.m_t = rec1.m_t + hit_distance / ray_length;
		record.m_point = inRay.At(record.m_t);

		if (debugging) {
			std::clog << "hit_distance = " << hit_distance << '\n'
				<< "rec.t = " << record.m_t << '\n'
				<< "rec.p = " << record.m_point << '\n';
		}

		record.m_normal = Vector3(1, 0, 0);  // arbitrary
		record.m_frontFace = true;     // also arbitrary
		record.m_pMaterial = m_phaseFunction;

		return true;
	}

	virtual AABB BoundingBox() const override { return m_boundary->BoundingBox(); }
private:
	std::shared_ptr<Collidable> m_boundary;
	double m_negInvDensity;
	std::shared_ptr<Material> m_phaseFunction;
};
