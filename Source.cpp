#include "Common.h"

#include "Threading.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>

//#define THREADRANGES
//#define THREADATOMIC
//#define ASYNCTHREADRANGES
#define ASYNCATOMIC

int main()
{
	CollidableList world;

	Camera cam;
	cam.m_aspectRatio = 16.0 / 9.0;
	cam.m_imageWidth = 1200;
	cam.m_samplesPerPixel = 50;
	cam.m_maxDepth = 50;
	cam.m_verticalFOV = 20;

	cam.m_lookfrom = Point3D(13, 2, 3);
	cam.m_lookat = Point3D(0, 0, 0);
	cam.m_vup = Vector3(0, 1, 0);
	cam.m_defocusAngle = 0.6;
	cam.m_focusDist = 10.0;

	switch (1)
	{
	case 1: world = GenerateRandomScene(cam); break;
	case 2: world = TwoSpheres(cam);    break;
	case 3: world = Earth(cam);    break;
	case 4: world = TwoPerlinSpheres(cam); break;
	case 5: world = Quads(cam); break;
	case 6: world = SimpleLights(cam); break;
	case 7: world = CornellBox(cam); break;
	case 8: world = CornellSmoke(cam);      break;
	case 9: world = FinalScene(cam, 1200, 10000, 50); break;
	default:world = FinalScene(cam, 400, 250, 4); break;
	}

	cam.Initialize();

	Image bmp(cam.m_imageWidth, cam.m_imageHeight);

	ThreadSetup setup;

	setup.numThreads = std::thread::hardware_concurrency();
	setup.length = cam.NumberOfPixels();
	setup.chunkSize = setup.length / setup.numThreads;
	setup.SetupThreadRanges();

	auto start = std::chrono::system_clock::now();
	std::cout << "Beginning setting of scanline data.\n";

#if defined(THREAD) && defined(THREADRANGES)
	setup.ThreadRanges(cam, bmp, world);
#elif defined(THREAD) && defined(THREADATOMIC)
	setup.ThreadAtomic(cam, bmp, world);
#elif defined(ASYNC) && defined(ASYNCTHREADRANGES)
	setup.AsyncThreadRanges(cam, bmp, world);
#elif defined(ASYNC) && defined(ASYNCATOMIC)
	setup.AsyncAtomic(cam, bmp, world);
#endif

#if defined(THREAD)
	for (auto& thread : setup.threads) {
		thread.join();
	}
#elif defined(ASYNC)
	for (auto& future : setup.futures)
		future.wait();
#endif

#if defined(OMP)
	cam.Render(world, bmp);
#endif

	std::cerr << "\nDone.\n";
	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

	bmp.Export((std::to_string(cam.m_samplesPerPixel) + "Samples.bmp").c_str());

	system("pause");
	return EXIT_SUCCESS;
}