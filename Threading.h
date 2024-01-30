#pragma once

#include "Camera.h"
#include "BVH_Node.h"
#include "Image.h"

#include <future>
#include <thread>

struct Chunk
{
	unsigned int start;
	unsigned int end;
};

enum Solution
{
	ThreadRanges,
	Atomic
};

#define ASYNC
//#define THREAD

struct ThreadSetup
{
	unsigned int numThreads;
	unsigned int length;
	unsigned int chunkSize;
	std::vector<Chunk> threadRanges;
	void SetupThreadRanges();

#if defined(ASYNC)
	std::vector<std::future<void>> futures;
	void AsyncThreadRanges(Camera& cam, Image& bmp, CollidableList& world);
	void AsyncAtomic(Camera& cam, Image& bmp, CollidableList& world);
#elif defined(THREAD)
	std::vector<std::thread> threads;
	void ThreadRanges(Camera& cam, Image& bmp, CollidableList& world);
	void ThreadAtomic(Camera& cam, Image& bmp, CollidableList& world);
#endif
};

void ThreadSetup::SetupThreadRanges()
{
	for (unsigned int i = 0; i < numThreads; i++)
	{
		unsigned int start = i * chunkSize;
		unsigned int end = (i == numThreads - 1) ? length : start + chunkSize;
		threadRanges.push_back({ start, end });
	}
}

#if defined(ASYNC)
void ThreadSetup::AsyncThreadRanges(Camera& cam, Image& bmp, CollidableList& world)
{
	for (std::size_t i(0); i < numThreads; ++i)
		futures.emplace_back(std::async(std::launch::async, [=, &cam, &bmp, &world]()
			{
				for (unsigned int index = threadRanges[i].start; index < threadRanges[i].end; index++)
				{
					std::size_t i = index % cam.m_imageWidth;
					std::size_t j = index / cam.m_imageWidth;

					Colour pixelColour(0, 0, 0);
					for (int sample = 0; sample < cam.m_samplesPerPixel; ++sample)
					{
						Ray ray = cam.GetRay(i, j);
						pixelColour += RayColour(ray, world, cam.m_maxDepth, cam.m_background);
					}

					bmp.SetIColour(IColour(pixelColour.X(), pixelColour.Y(), pixelColour.Z()), i, j, cam.m_samplesPerPixel);
				}
			}));
}

void ThreadSetup::AsyncAtomic(Camera& cam, Image& bmp, CollidableList& world)
{
	volatile std::atomic<std::size_t> count(0);
	for (int i = 0; i < numThreads; ++i)
		futures.emplace_back(
			std::async(std::launch::async, [=, &cam, &bmp, &world, &count]()
				{
					while (true)
					{
						std::size_t index = count++;
						if (index >= length)
							break;
						std::size_t i = index % cam.m_imageWidth;
						std::size_t j = index / cam.m_imageWidth;

						Colour pixelColour(0, 0, 0);
						for (int sample = 0; sample < cam.m_samplesPerPixel; ++sample)
						{
							Ray ray = cam.GetRay(i, j);
							pixelColour += RayColour(ray, world, cam.m_maxDepth, cam.m_background);
						}

						bmp.SetIColour(IColour(pixelColour.X(), pixelColour.Y(), pixelColour.Z()), i, j, cam.m_samplesPerPixel);
					}
				}));
}
#elif defined(THREAD)
void ThreadSetup::ThreadRanges(Camera& cam, Image& bmp, CollidableList& world)
{
	for (std::size_t i(0); i < numThreads; ++i)
		threads.push_back(std::thread([=, &cam, &bmp, &world]()
			{
				for (unsigned int index = threadRanges[i].start; index < threadRanges[i].end; index++)
				{
					std::size_t i = index % cam.m_imageWidth;
					std::size_t j = index / cam.m_imageWidth;

					Colour pixelColour(0, 0, 0);
					for (int sample = 0; sample < cam.m_samplesPerPixel; ++sample)
					{
						Ray ray = cam.GetRay(i, j);
						pixelColour += RayColour(ray, world, cam.m_maxDepth, cam.m_background);
					}

					bmp.SetIColour(IColour(pixelColour.X(), pixelColour.Y(), pixelColour.Z()), i, j, cam.m_samplesPerPixel);
				}
			}));
}

void ThreadSetup::ThreadAtomic(Camera& cam, Image& bmp, CollidableList& world)
{
	volatile std::atomic<int> count = 0;

	for (int i = 0; i < numThreads; ++i)
		threads.push_back(std::thread([=, &cam, &bmp, &world, &count]() {
			while (true)
			{
				std::size_t index = count++;
				if (index >= length)
					break;
				std::size_t i = index % cam.m_imageWidth;
				std::size_t j = index / cam.m_imageWidth;

				Colour pixelColour(0, 0, 0);
				for (int sample = 0; sample < cam.m_samplesPerPixel; ++sample)
				{
					Ray ray = cam.GetRay(i, j);
					pixelColour += RayColour(ray, world, cam.m_maxDepth, cam.m_background);
				}

				bmp.SetIColour(IColour(pixelColour.X(), pixelColour.Y(), pixelColour.Z()), i, j, cam.m_samplesPerPixel);
			}
			}));
}
#endif