#pragma once

#include "Vector3.h"
#include "Collidable.h"
#include "Material.h"

#include <iostream>

Colour RayColour(const Ray& rayIn, const Collidable& world, int depth, Colour background)
{
	CollideRecord record;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return Colour(0, 0, 0);

	if (!world.Collide(rayIn, Interval(0.001, g_infinity), record))
		return background;

	/*
		For a sphere, the outward 'N'ormal is in the direction of the hit point minus the center :
			Co--->P--->P-C - where C is sphere center
		Pick a random point S inside this unit sphere and send a ray from hit point P
		to the random point S (this is the vector S-P) using the rejection method
			Point3D target = rec.p + rec.normal + random_in_unit_sphere(); <- Diffuse method 1
		Pick a random point on the unit sphere by picking a random point in the unit sphere, and then normalizing it
			Point3D target = rec.p + rec.normal + random_unit_vector();	 <- Diffuse method 2
		a uniform scatter direction for all angles from the hit point, no dependency on the angle of the normal
			Point3D target = rec.p + random_in_hemisphere(rec.normal);
			return 0.5 * ray_colour(Ray(rec.p, target - rec.p), world, depth-1);
	*/
	Ray scatteredRay;
	Colour attenuation;
	Colour colorFromEmission = record.m_pMaterial->Emitted(record.m_u, record.m_v, record.m_point);

	if (!record.m_pMaterial->Scatter(rayIn, record, attenuation, scatteredRay))
		return colorFromEmission;

	Colour colorFromScatter = attenuation * RayColour(scatteredRay, world, depth - 1, background);

	return colorFromEmission + colorFromScatter;
}

void WriteColour(std::ostream& out, Colour pixelColour, int samplesPerPixel)
{
	auto r = pixelColour.X();
	auto g = pixelColour.Y();
	auto b = pixelColour.Z();

	// Using gamma correction for accurate colour intensity
	// By dividing the colour by the number of samples  and gamma correct for gamma=2.0
	// Gamma 2 = 1/gamma, or in our simple case 1/2, which is just square-root:
	auto scale = 1.0 / samplesPerPixel;
	r = std::sqrt(scale * r);
	g = std::sqrt(scale * g);
	b = std::sqrt(scale * b);

	// Write the translated [0,255] value of each Colour component.
	out << static_cast<int>(256 * Clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * Clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * Clamp(b, 0.0, 0.999)) << '\n';
}
