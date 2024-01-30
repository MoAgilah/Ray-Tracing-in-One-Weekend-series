#pragma once

#include "Common.h"
#include "ColliablesList.h"
#include "Image.h"
#include "Colour.h"

#include <omp.h>
#define OMP

class Camera
{
public:
	Camera(){}

	void Initialize()
	{
		m_imageHeight = static_cast<int>(m_imageWidth / m_aspectRatio);
		m_imageHeight = (m_imageHeight < 1) ? 1 : m_imageHeight;

		m_center = m_lookfrom;

		// Determine viewport dimensions.
		auto theta = DegreesToRadians(m_verticalFOV);
		auto h = tan(theta/2);
		auto viewportHeight = 2 * h * m_focusDist;
		auto viewportWidth = viewportHeight * (static_cast<double>(m_imageWidth)/ m_imageHeight);

		// Calculate the u,v,w unit basis vectors for the camera coordinate frame.
		m_w = UnitVector(m_lookfrom - m_lookat);
		m_u = UnitVector(Cross(m_vup, m_w));
		m_v = Cross(m_w, m_u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		Vector3 viewportU = viewportWidth * m_u;    // Vector across viewport horizontal edge
		Vector3 viewportV = viewportHeight * m_v;  // Vector down viewport vertical edge

		// Calculate the horizontal and vertical delta vectors to the next pixel.
		m_pixelDeltaU = viewportU / m_imageWidth;
		m_pixelDeltaV = viewportV / m_imageHeight;

		// Calculate the location of the upper left pixel.
		auto viewportUpperLeft = m_center - (m_focusDist * m_w) - viewportU/2 - viewportV/2;
		m_pixel00Loc = viewportUpperLeft + 0.5 * (m_pixelDeltaU + m_pixelDeltaV);

		// Calculate the camera defocus disk basis vectors.
		auto defocusRadius = m_focusDist * tan(DegreesToRadians(m_defocusAngle / 2));
		m_defocusDiskU = m_u * defocusRadius;
		m_defocusDiskV = m_v * defocusRadius;
	}

	Ray GetRay(int i, int j) const
	{
		// Get a randomly-sampled camera ray for the pixel at location i,j, originating from
		// the camera defocus disk.
		auto pixelCenter = m_pixel00Loc + (i * m_pixelDeltaU) + (j * m_pixelDeltaV);
		auto pixelSample = pixelCenter + PixelSampleSquare();

		auto rayOrigin = (m_defocusAngle <= 0) ? m_center : DefocusDiskSample();
		auto rayDirection = pixelSample - rayOrigin;
		auto rayTime = RandomDouble();

		return Ray(rayOrigin, rayDirection, rayTime);
	}

	Vector3 PixelSampleSquare() const {
		// Returns a random point in the square surrounding a pixel at the origin.
		auto px = -0.5 + RandomDouble();
		auto py = -0.5 + RandomDouble();
		return (px * m_pixelDeltaU) + (py * m_pixelDeltaV);
	}

	Point3D DefocusDiskSample() const
	{
		// Returns a random point in the camera defocus disk.
		auto p = RandomInUnitDisk();
		return m_center + (p[0] * m_defocusDiskU) + (p[1] * m_defocusDiskV);
	}

	void Render(const CollidableList& world, Image& bmp)
	{
#if defined(OMP)
#pragma omp parallel
		{
#pragma omp for collapse(2) nowait
#endif
			for (int j = 0; j < m_imageHeight; ++j)
			{
				//std::clog << "\rScanlines remaining: " << (m_imageHeight - j) << ' ' << std::flush;
				for (int i = 0; i < m_imageWidth; ++i)
				{
					Colour pixelColour(0, 0, 0);
					for (int sample = 0; sample < m_samplesPerPixel; ++sample) {
						Ray r = GetRay(i, j);
						pixelColour += RayColour(r, world, m_maxDepth, m_background);
					}
					bmp.SetIColour(IColour(pixelColour.X(), pixelColour.Y(), pixelColour.Z()), i, j, m_samplesPerPixel);
				}
			}
#if defined(OMP)
		}
#endif
	}

	int NumberOfPixels() { return m_imageHeight * m_imageWidth; }
public:
	double m_aspectRatio = 1.0;				// Ratio of image width over height
	int	   m_imageHeight;					// Rendered image height in pixel count
	int    m_imageWidth = 100;				// Rendered image width in pixel count
	int    m_samplesPerPixel = 10;			// Count of random samples for each pixel
	int    m_maxDepth = 10;					// Maximum number of ray bounces into scene
	Colour m_background;					// Scene background color
	double m_verticalFOV = 90;				// Vertical view angle (field of view)
	Point3D m_lookfrom = Point3D(0, 0, -1);	// Point camera is looking from
	Point3D m_lookat = Point3D(0, 0, 0);	// Point camera is looking at
	Vector3 m_vup = Vector3(0, 1, 0);		// Camera-relative "up" direction

	double m_defocusAngle = 0;				// Variation angle of rays through each pixel
	double m_focusDist = 10;				// Distance from camera lookfrom point to plane of perfect focus
private:

	Point3D	  m_center;			// Camera center
	Point3D   m_pixel00Loc;	// Location of pixel 0, 0
	Vector3   m_pixelDeltaU;	// Offset to pixel to the right
	Vector3   m_pixelDeltaV;	// Offset to pixel below
	Vector3   m_u, m_v, m_w;	// Camera frame basis vectors
	Vector3   m_defocusDiskU;	// Defocus disk horizontal radius
	Vector3   m_defocusDiskV;	// Defocus disk vertical radius
};