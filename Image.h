#pragma once

#include "Common.h"

#include <vector>
#include <iostream>
#include <fstream>

struct IColour
{
	double r, g, b;
	IColour()
		: r(0), g(0), b(0) {}

	IColour(double r, double g, double b)
		: r(r), g(g), b(b) {}

	~IColour() {}
};

inline double linear_to_gamma(double linearComponent)
{
	return sqrt(linearComponent);
}

class Image
{
public:
	Image(int width, int height)
		: m_width(width), m_height(height), m_pixels(std::vector<IColour>((std::size_t)width* height))
	{}

	IColour GetIColour(int x, int y) const { return m_pixels[(std::size_t)y * m_width + x]; };
	void SetIColour(const IColour& colour, int x, int y, int samplesPerPixel)
	{
		auto r = colour.r;
		auto g = colour.g;
		auto b = colour.b;

		// Using gamma correction for accurate colour intensity
		// By dividing the colour by the number of samples  and gamma correct for gamma=2.0
		// Gamma 2 = 1/gamma, or in our simple case 1/2, which is just square-root:
		auto scale = 1.0 / samplesPerPixel;
		r *= scale;
		g *= scale;
		b *= scale;

		// Apply the linear to gamma transform.
		r = linear_to_gamma(r);
		g = linear_to_gamma(g);
		b = linear_to_gamma(b);

		auto index = (int) y * m_width + x;
		static const Interval intensity(0.000, 0.999);
		m_pixels[index].r = intensity.clamp(r);
		m_pixels[index].g = intensity.clamp(g);
		m_pixels[index].b = intensity.clamp(b);
	};

	void Export(const char* path)
	{
		std::ofstream ofs(path, std::ios_base::out | std::ios_base::binary);
		if (!ofs.is_open())
		{
			std::cout << "\nERROR: File could not be opened\n";
			return;
		}

		std::cout << "\nBeginning creation of file\n";

		unsigned char bmpPad[3] = { 0,0,0 };
		const int paddingAmont = ((4 - (m_width * 3) % 4) % 4);

		const int fileHeaderSize = 14;
		const int informationHeaderSize = 40;
		const int fileSize = fileHeaderSize + informationHeaderSize + m_width * m_height * 3 + paddingAmont * m_height;

		unsigned char fileHeader[fileHeaderSize] = { 0 };
		// File type
		fileHeader[0] = 'B';
		fileHeader[1] = 'M';
		// File size
		fileHeader[2] = fileSize;
		fileHeader[3] = fileSize >> 8;
		fileHeader[4] = fileSize >> 16;
		fileHeader[5] = fileSize >> 24;
		// Reserved 1 (not used)
		fileHeader[6] = 0;
		fileHeader[7] = 0;
		// Reserved 2 (not used)
		fileHeader[8] = 0;
		fileHeader[9] = 0;
		// Pixel data offset
		fileHeader[10] = fileHeaderSize + informationHeaderSize;
		fileHeader[11] = 0;
		fileHeader[12] = 0;
		fileHeader[13] = 0;

		unsigned char informationHeader[informationHeaderSize] = { 0 };

		// Header size
		informationHeader[0] = informationHeaderSize;
		informationHeader[1] = 0;
		informationHeader[2] = 0;
		informationHeader[3] = 0;
		// Image width
		informationHeader[4] = m_width;
		informationHeader[5] = m_width >> 8;
		informationHeader[6] = m_width >> 16;
		informationHeader[7] = m_width >> 24;
		// Image Height
		informationHeader[8] = m_height;
		informationHeader[9] = m_height >> 8;
		informationHeader[10] = m_height >> 16;
		informationHeader[11] = m_height >> 24;
		// Planes
		informationHeader[12] = 1;
		informationHeader[13] = 0;
		// Bits per pixel (RGB)
		informationHeader[14] = 24;
		informationHeader[15] = 0;
		// Compression (No compression)
		informationHeader[16] = 0;
		informationHeader[17] = 0;
		informationHeader[18] = 0;
		informationHeader[19] = 0;
		// Image Size (No compression)
		informationHeader[20] = 0;
		informationHeader[21] = 0;
		informationHeader[22] = 0;
		informationHeader[23] = 0;
		// X pixels per meter (unspecified)
		informationHeader[24] = 0;
		informationHeader[25] = 0;
		informationHeader[26] = 0;
		informationHeader[27] = 0;
		// Y pixels per meter (unspecified)
		informationHeader[28] = 0;
		informationHeader[29] = 0;
		informationHeader[30] = 0;
		informationHeader[31] = 0;
		// Total IColours (Color palette not used)
		informationHeader[32] = 0;
		informationHeader[33] = 0;
		informationHeader[34] = 0;
		informationHeader[35] = 0;
		// Important IColours (Generally Ignored)
		informationHeader[36];
		informationHeader[37];
		informationHeader[38];
		informationHeader[39];

		ofs.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
		ofs.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				auto colour = GetIColour(x, y);
				unsigned char r = static_cast<unsigned char>(Clamp(colour.r, 0.0, 0.999) * 255.0);
				unsigned char g = static_cast<unsigned char>(Clamp(colour.g, 0.0, 0.999) * 255.0);
				unsigned char b = static_cast<unsigned char>(Clamp(colour.b, 0.0, 0.999) * 255.0);

				unsigned char pixelIColour[] = { b, g, r };

				ofs.write(reinterpret_cast<char*>(pixelIColour), 3);
			}

			ofs.write(reinterpret_cast<char*>(bmpPad), paddingAmont);
		}

		ofs.close();

		std::cout << "Done\n";
	}
private:
	int m_width;
	int m_height;
	std::vector<IColour> m_pixels;
};
