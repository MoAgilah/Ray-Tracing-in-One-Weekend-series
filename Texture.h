#pragma once

#include "Common.h"
#include "Perlin.h"
#include "RTW_STB_Image.h"

class Texture
{
public:
	virtual ~Texture() = default;

	virtual Colour Value(double u, double v, const Point3D& p) const = 0;
};

class SolidColour : public Texture
{
public:
	SolidColour(Colour c) : m_colourValue(c) {}
	SolidColour(double red, double green, double blue) : SolidColour(Colour(red, green, blue)) {}

	Colour Value(double u, double v, const Point3D& p) const override
	{
		return m_colourValue;
	}
private:
	Colour m_colourValue;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture(double _scale, std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd)
		: m_invScale(1.0 / _scale), m_even(_even), m_odd(_odd) {}

	CheckerTexture(double _scale, Colour c1, Colour c2)
		: m_invScale(1.0 / _scale), m_even(std::make_shared<SolidColour>(c1)), m_odd(std::make_shared<SolidColour>(c2)) {}

	virtual Colour Value(double u, double v, const Point3D& p) const override
	{
		auto xInteger = static_cast<int>(std::floor(m_invScale * p.X()));
		auto yInteger = static_cast<int>(std::floor(m_invScale * p.Y()));
		auto zInteger = static_cast<int>(std::floor(m_invScale * p.Z()));

		bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

		return isEven ? m_even->Value(u, v, p) : m_odd->Value(u, v, p);
	}
private:
	double m_invScale;
	std::shared_ptr<Texture> m_even;
	std::shared_ptr<Texture> m_odd;
};

class ImageTexture : public Texture
{
public:
	ImageTexture(const char* filename) : m_image(filename) {}

	virtual Colour Value(double u, double v, const Point3D& p) const override
	{
		// If we have no texture data, then return solid cyan as a debugging aid.
		if (m_image.Height() <= 0) return Colour(0, 1, 1);

		// Clamp input texture coordinates to [0,1] x [1,0]
		u = Interval(0, 1).clamp(u);
		v = 1.0 - Interval(0, 1).clamp(v);  // Flip V to image coordinates

		auto i = static_cast<int>(u * m_image.Width());
		auto j = static_cast<int>(v * m_image.Height());
		auto pixel = m_image.PixelData(i, j);

		auto colourScale = 1.0 / 255.0;
		return Colour(colourScale * pixel[0], colourScale * pixel[1], colourScale * pixel[2]);
	}
private:
	RTW_Image m_image;
};

class NoiseTexture : public Texture
{
public:
	NoiseTexture() {}

	NoiseTexture(double sc) : m_scale(sc) {}

	virtual Colour Value(double u, double v, const Point3D& p) const override
	{
		auto s = m_scale * p;
		return Colour(1, 1, 1) * 0.5 * (std::sin(s.Z() + 10 * m_noise.Turberlance(s)));
	}

private:
	double m_scale;
	Perlin m_noise;
};