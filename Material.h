#pragma once

#include "Common.h"
#include "texture.h"

struct CollideRecord;

class Material
{
public:
	virtual Colour Emitted(double u, double v, const Point3D& p) const
	{
		return Colour(0, 0, 0);
	}

	virtual bool Scatter(const Ray& inRay, const CollideRecord& record, Colour& attenuation, Ray& scatteredRay) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const Colour& albedo) : m_albedo(std::make_shared<SolidColour>(albedo)) {}
	Lambertian(std::shared_ptr<Texture> a) : m_albedo(a) {}

	virtual bool Scatter(const Ray& inRay, const CollideRecord& record, Colour& attenuation, Ray& scatteredRay) const override
	{
		auto scatter_direction = record.m_normal + RandomUnitVector();

		// Catch degenerate scatter direction
		if (scatter_direction.NearZero())
			scatter_direction = record.m_normal;

		scatteredRay = Ray(record.m_point, scatter_direction, inRay.Time());
		attenuation = m_albedo->Value(record.m_u, record.m_v, record.m_point);
		return true;
	}

private:
	std::shared_ptr<Texture> m_albedo;
};

class Metal : public Material
{
public:
	Metal(const Colour& albedo, double fuzz) : m_albedo(albedo), m_fuzz(fuzz < 1 ? fuzz : 1) {}

	virtual bool Scatter(const Ray& inRay, const CollideRecord& record, Colour& attenuation, Ray& scatteredRay) const override
	{
		Vector3 reflected = Reflect(UnitVector(inRay.Direction()), record.m_normal);
		scatteredRay = Ray(record.m_point, reflected + m_fuzz * RandomInUnitSphere(), inRay.Time());
		attenuation = m_albedo;
		return (Dot(scatteredRay.Direction(), record.m_normal) > 0);
	}

private:
	Colour m_albedo;
	double m_fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(double index_of_refraction) : m_indexOfRefraction(index_of_refraction) {}

	virtual bool Scatter(const Ray& rayIn, const CollideRecord& record, Colour& attenuation, Ray& scatteredRay) const override
	{
		attenuation = Colour(1.0, 1.0, 1.0);
		double refractionRatio = record.m_frontFace ? (1.0 / m_indexOfRefraction) : m_indexOfRefraction;

		/*
			A pratical issue is that when the rayt is in the material with the higher refractive index,
			there is no solution to Snell's law used in the refraction function.
			If we refer back to Snell's law and the derivation of sinθ′:
				sinθ′=η / η′⋅sinθ
			If the ray is inside glass and outside is air (η=1.5 and η′=1.0):
				sinθ′=1.5 / 1.0⋅sinθ
			The value of sinθ′ cannot be greater than 1. So, if,
				1.5 / 1.0⋅sinθ > 1.0
			If a solution doesn't exist, the glass cannot refract and must reflect
				if (refraction_ratio * sin_theta > 1.0)
					Must Reflect
				else
					Can Refract
			We can solve for sin_theta using the trigonometric qualities:
					   _______
				sinθ= √1−cos2θ
			and
				cosθ = R * n
		*/
		Vector3 unitDirection = UnitVector(rayIn.Direction());
		double cos_theta = fmin(Dot(-unitDirection, record.m_normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannotRefract = refractionRatio * sin_theta > 1.0;
		Vector3 direction;

		if (cannotRefract || Reflectance(cos_theta, refractionRatio) > RandomDouble())
			direction = Reflect(unitDirection, record.m_normal);
		else
			direction = Refract(unitDirection, record.m_normal, refractionRatio);

		scatteredRay = Ray(record.m_point, direction, rayIn.Time());
		return true;
	}

private:
	static double Reflectance(double cosine, double refIdx)
	{
		//use Schlick's approximation for reflectance
		auto r0 = (1 - refIdx) / (1 + refIdx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}

	double m_indexOfRefraction; // Index of Refraction
};

class DiffuseLight : public Material
{
public:
	DiffuseLight(std::shared_ptr<Texture> a) : m_emit(a) {}
	DiffuseLight(Colour c) : m_emit(std::make_shared<SolidColour>(c)) {}

	virtual bool Scatter(const Ray& rayIn, const CollideRecord& record, Colour& attenuation, Ray& scatteredRay) const override
	{
		return false;
	}

	virtual Colour Emitted(double u, double v, const Point3D& p) const override
	{
		return m_emit->Value(u, v, p);
	}

private:
	std::shared_ptr<Texture> m_emit;
};

class Isotropic : public Material
{
public:
	Isotropic(Colour c) : m_albedo(std::make_shared<SolidColour>(c)) {}
	Isotropic(std::shared_ptr<Texture> a) : m_albedo(a) {}

	virtual bool Scatter(const Ray& rayIn, const CollideRecord& record, Colour& attenuation, Ray& scatteredRay) const override
	{
		scatteredRay = Ray(record.m_point, RandomUnitVector(), rayIn.Time());
		attenuation = m_albedo->Value(record.m_u, record.m_v, record.m_point);
		return true;
	}

private:
	std::shared_ptr<Texture> m_albedo;
};