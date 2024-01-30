#pragma once

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "External/stb_image.h"

#include <cstdlib>
#include <iostream>

class RTW_Image {
public:
	RTW_Image() : data(nullptr) {}

	RTW_Image(const char* imageFilename) {
		// Loads image data from the specified file. If the RTW_IMAGES environment variable is
		// defined, looks only in that directory for the image file. If the image was not found,
		// searches for the specified image file first from the current directory, then in the
		// images/ subdirectory, then the _parent's_ images/ subdirectory, and then _that_
		// parent, on so on, for six levels up. If the image was not loaded successfully,
		// width() and height() will return 0.

		auto filename = std::string(imageFilename);

		// Hunt for the image file in some likely locations.
		if (Load(filename)) return;
		if (Load("images/" + filename)) return;
		if (Load("../images/" + filename)) return;
		if (Load("../../images/" + filename)) return;
		if (Load("../../../images/" + filename)) return;
		if (Load("../../../../images/" + filename)) return;
		if (Load("../../../../../images/" + filename)) return;
		if (Load("../../../../../../images/" + filename)) return;

		std::cerr << "ERROR: Could not load image file '" << imageFilename << "'.\n";
	}

	~RTW_Image() { STBI_FREE(data); }

	bool Load(const std::string filename) {
		// Loads image data from the given file name. Returns true if the load succeeded.
		auto n = m_bytesPerPixel; // Dummy out parameter: original components per pixel
		data = stbi_load(filename.c_str(), &m_imageWidth, &m_imageHeight, &n, m_bytesPerPixel);
		m_bytesPerScanline = m_imageWidth * m_bytesPerPixel;
		return data != nullptr;
	}

	int Width()  const { return (data == nullptr) ? 0 : m_imageWidth; }
	int Height() const { return (data == nullptr) ? 0 : m_imageHeight; }

	const unsigned char* PixelData(int x, int y) const {
		// Return the address of the three bytes of the pixel at x,y (or magenta if no data).
		static unsigned char magenta[] = { 255, 0, 255 };
		if (data == nullptr) return magenta;

		x = Clamp(x, 0, m_imageWidth);
		y = Clamp(y, 0, m_imageHeight);

		return data + y * m_bytesPerScanline + x * m_bytesPerPixel;
	}

private:
	const int m_bytesPerPixel = 3;
	unsigned char* data;
	int m_imageWidth, m_imageHeight;
	int m_bytesPerScanline;

	static int Clamp(int x, int low, int high)
	{
		// Return the value clamped to the range [low, high).
		if (x < low) return low;
		if (x < high) return x;
		return high - 1;
	}
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning (pop)
#endif