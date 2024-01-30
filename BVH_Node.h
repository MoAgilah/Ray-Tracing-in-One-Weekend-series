#pragma once

#include <stdlib.h>

#include "AABB.h"
#include "Quad.h"
#include "Collidable.h"
#include "ColliablesList.h"
#include "texture.h"
#include "ConstrantMedium.h"

#include <algorithm>

class BVH_NODE : public Collidable
{
public:
	BVH_NODE(const CollidableList& list) : BVH_NODE(list.m_objects, 0, list.m_objects.size()) {}
	BVH_NODE(const std::vector<std::shared_ptr<Collidable>>& src_objects, std::size_t start, std::size_t end);
	virtual bool Collide(const Ray& inRay, Interval rayI, CollideRecord& record) const;
	virtual AABB BoundingBox() const override { return m_bbox; }
private:
	std::shared_ptr<Collidable> m_left;
	std::shared_ptr<Collidable> m_right;
	AABB m_bbox;

	static bool BoxCompare(
		const std::shared_ptr<Collidable> a, const std::shared_ptr<Collidable> b, int axisIndex
	) {
		return a->BoundingBox().axis(axisIndex).m_min < b->BoundingBox().axis(axisIndex).m_min;
	}

	static bool BoxXCompare(const std::shared_ptr<Collidable> a, const std::shared_ptr<Collidable> b) {
		return BoxCompare(a, b, 0);
	}

	static bool BoxYCompare(const std::shared_ptr<Collidable> a, const std::shared_ptr<Collidable> b) {
		return BoxCompare(a, b, 1);
	}

	static bool BoxZCompare(const std::shared_ptr<Collidable> a, const std::shared_ptr<Collidable> b) {
		return BoxCompare(a, b, 2);
	}
};

BVH_NODE::BVH_NODE(const std::vector<std::shared_ptr<Collidable>>& srcObjects, std::size_t start, std::size_t end)
{
	m_bbox = AABB();
	for (size_t objectIndex = start; objectIndex < end; objectIndex++)
		m_bbox = AABB(m_bbox, srcObjects[objectIndex]->BoundingBox());

	int axis = m_bbox.longest_axis();

	auto comparator = (axis == 0) ? BoxXCompare
		: (axis == 1) ? BoxYCompare
		: BoxZCompare;

	auto objects = srcObjects; // A modifiable array of the source scene objects

	size_t object_span = end - start;

	if (object_span == 1) {
		m_left = m_right = objects[start];
	}
	else if (object_span == 2) {
		if (comparator(objects[start], objects[start + 1])) {
			m_left = objects[start];
			m_right = objects[start + 1];
		}
		else {
			m_left = objects[start + 1];
			m_right = objects[start];
		}
	}
	else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + object_span / 2;
		m_left = std::make_shared<BVH_NODE>(objects, start, mid);
		m_right = std::make_shared<BVH_NODE>(objects, mid, end);
	}
}

bool BVH_NODE::Collide(const Ray& inRay, Interval rayI, CollideRecord& record) const
{
	if (!m_bbox.Collide(inRay, rayI))
		return false;

	bool hitLeft = m_left->Collide(inRay, rayI, record);
	bool hitRight = m_right->Collide(inRay, Interval(rayI.m_min, hitLeft ? record.m_t : rayI.m_max), record);

	return hitLeft || hitRight;
}

CollidableList GenerateRandomScene(Camera& cam)
{
	cam.m_background = Colour(0.70, 0.80, 1.00);

	CollidableList world;

	auto checker = std::make_shared<CheckerTexture>(0.32, Colour(.2, .3, .1), Colour(.9, .9, .9));
	world.Add(std::make_shared<Sphere>(Point3D(0, -1000, 0), 1000, std::make_shared<Lambertian>(checker)));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			auto chooseMat = RandomDouble();
			Point3D center(a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble());

			if ((center - Point3D(4, 0.2, 0)).Length() > 0.9)
			{
				std::shared_ptr<Material> sphereMaterial;

				if (chooseMat < 0.8)
				{
					// diffuse
					auto albedo = Colour::Random() * Colour::Random();
					sphereMaterial = std::make_shared<Lambertian>(albedo);
					world.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
				}
				else if (chooseMat < 0.95)
				{
					// metal
					auto albedo = Colour::Random(0.5, 1);
					auto fuzz = RandomDouble(0, 0.5);
					sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
					auto center2 = center + Vector3(0, RandomDouble(0, .5), 0);
					world.Add(make_shared<Sphere>(center, center2, 0.2, sphereMaterial));
				}
				else
				{
					// glass
					sphereMaterial = std::make_shared<Dielectric>(1.5);
					world.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
				}
			}
		}
	}

	auto material1 = std::make_shared<Dielectric>(1.5);
	world.Add(std::make_shared<Sphere>(Point3D(0, 1, 0), 1.0, material1));

	auto material2 = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(Point3D(-4, 1, 0), 1.0, material2));

	auto material3 = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
	world.Add(std::make_shared<Sphere>(Point3D(4, 1, 0), 1.0, material3));

	world = CollidableList(std::make_shared<BVH_NODE>(world));

	return world;
}

CollidableList TwoSpheres(Camera& cam)
{
	cam.m_background = Colour(0.70, 0.80, 1.00);

	CollidableList world;

	auto checker = std::make_shared<CheckerTexture>(0.8, Colour(.2, .3, .1), Colour(.9, .9, .9));

	world.Add(std::make_shared<Sphere>(Point3D(0, -10, 0), 10, std::make_shared<Lambertian>(checker)));
	world.Add(std::make_shared<Sphere>(Point3D(0, 10, 0), 10, std::make_shared<Lambertian>(checker)));

	world = CollidableList(std::make_shared<BVH_NODE>(world));

	return world;
}

CollidableList Earth(Camera& cam)
{
	cam.m_lookfrom = Point3D(0, 0, 9);
	cam.m_defocusAngle = 0;
	cam.m_background = Colour(0.70, 0.80, 1.00);

	CollidableList world;

	auto earthTexture = std::make_shared<ImageTexture>("earthmap.jpg");
	auto earthSurface = std::make_shared<Lambertian>(earthTexture);
	world.Add(std::make_shared<Sphere>(Point3D(0, 0, 0), 2, earthSurface));

	world = CollidableList(std::make_shared<BVH_NODE>(world));

	return world;
}

CollidableList TwoPerlinSpheres(Camera& cam)
{
	cam.m_defocusAngle = 0;
	cam.m_background = Colour(0.70, 0.80, 1.00);

	CollidableList world;

	auto pertext = std::make_shared<NoiseTexture>(4);
	world.Add(std::make_shared<Sphere>(Point3D(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
	world.Add(std::make_shared<Sphere>(Point3D(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

	world = CollidableList(std::make_shared<BVH_NODE>(world));

	return world;
}

CollidableList Quads(Camera& cam)
{
	cam.m_verticalFOV = 80;
	cam.m_lookfrom = Point3D(0, 0, 9);
	cam.m_defocusAngle = 0;
	cam.m_background = Colour(0.70, 0.80, 1.00);

	CollidableList world;

	// Materials
	auto left_red = std::make_shared<Lambertian>(Colour(1.0, 0.2, 0.2));
	auto back_green = std::make_shared<Lambertian>(Colour(0.2, 1.0, 0.2));
	auto right_blue = std::make_shared<Lambertian>(Colour(0.2, 0.2, 1.0));
	auto upper_orange = std::make_shared<Lambertian>(Colour(1.0, 0.5, 0.0));
	auto lower_teal = std::make_shared<Lambertian>(Colour(0.2, 0.8, 0.8));

	// Quads
	world.Add(std::make_shared<Quad>(Point3D(-3, -2, 5), Vector3(0, 0, -4), Vector3(0, 4, 0), left_red));
	world.Add(std::make_shared<Quad>(Point3D(-2, -2, 0), Vector3(4, 0, 0), Vector3(0, 4, 0), back_green));
	world.Add(std::make_shared<Quad>(Point3D(3, -2, 1), Vector3(0, 0, 4), Vector3(0, 4, 0), right_blue));
	world.Add(std::make_shared<Quad>(Point3D(-2, 3, 1), Vector3(4, 0, 0), Vector3(0, 0, 4), upper_orange));
	world.Add(std::make_shared<Quad>(Point3D(-2, -3, 5), Vector3(4, 0, 0), Vector3(0, 0, -4), lower_teal));

	world = CollidableList(std::make_shared<BVH_NODE>(world));

	return world;
}

CollidableList SimpleLights(Camera& cam)
{
	cam.m_lookfrom = Point3D(26, 3, 6);
	cam.m_lookat = Point3D(0, 2, 0);
	cam.m_defocusAngle = 0;
	cam.m_background = Colour(0.0, 0.0, 0.0);

	CollidableList world;

	auto pertext = std::make_shared<NoiseTexture>(4);
	world.Add(std::make_shared<Sphere>(Point3D(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
	auto difflight = std::make_shared<DiffuseLight>(Colour(4, 4, 4));
	world.Add(std::make_shared<Sphere>(Point3D(0, 7, 0), 2, difflight));
	world.Add(std::make_shared<Quad>(Point3D(3, 1, -2), Vector3(2, 0, 0), Vector3(0, 2, 0), difflight));
	world.Add(std::make_shared<Sphere>(Point3D(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

	world = CollidableList(std::make_shared<BVH_NODE>(world));

	return world;
}

CollidableList CornellBox(Camera& cam)
{
	cam.m_lookfrom = Point3D(278, 278, -800);
	cam.m_lookat = Point3D(278, 278, 0);
	cam.m_defocusAngle = 0;
	cam.m_verticalFOV = 40;
	cam.m_samplesPerPixel = 200;
	cam.m_background = Colour(0.0, 0.0, 0.0);

	CollidableList world;

	auto red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
	auto white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	auto green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
	auto light = std::make_shared<DiffuseLight>(Colour(15, 15, 15));

	world.Add(std::make_shared<Quad>(Point3D(555, 0, 0), Vector3(0, 555, 0), Vector3(0, 0, 555), green));
	world.Add(std::make_shared<Quad>(Point3D(0, 0, 0), Vector3(0, 555, 0), Vector3(0, 0, 555), red));
	world.Add(std::make_shared<Quad>(Point3D(343, 554, 332), Vector3(-130, 0, 0), Vector3(0, 0, -105), light));
	world.Add(std::make_shared<Quad>(Point3D(0, 0, 0), Vector3(555, 0, 0), Vector3(0, 0, 555), white));
	world.Add(std::make_shared<Quad>(Point3D(555, 555, 555), Vector3(-555, 0, 0), Vector3(0, 0, -555), white));
	world.Add(std::make_shared<Quad>(Point3D(0, 0, 555), Vector3(555, 0, 0), Vector3(0, 555, 0), white));

	std::shared_ptr<Collidable> box1 = Box(Point3D(0, 0, 0), Point3D(165, 330, 165), white);
	box1 = std::make_shared<RotateY>(box1, 15);
	box1 = std::make_shared<Translate>(box1, Vector3(265, 0, 295));
	world.Add(box1);

	std::shared_ptr<Collidable> box2 = Box(Point3D(0, 0, 0), Point3D(165, 165, 165), white);
	box2 = make_shared<RotateY>(box2, -18);
	box2 = make_shared<Translate>(box2, Vector3(130, 0, 65));
	world.Add(box2);

	world = CollidableList(std::make_shared<BVH_NODE>(world));

	return world;
}

CollidableList CornellSmoke(Camera& cam)
{
	cam.m_lookfrom = Point3D(278, 278, -800);
	cam.m_lookat = Point3D(278, 278, 0);
	cam.m_defocusAngle = 0;
	cam.m_verticalFOV = 40;
	cam.m_samplesPerPixel = 200;
	cam.m_background = Colour(0.0, 0.0, 0.0);

	CollidableList world;

	auto red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
	auto white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	auto green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
	auto light = std::make_shared<DiffuseLight>(Colour(15, 15, 15));

	world.Add(std::make_shared<Quad>(Point3D(555, 0, 0), Vector3(0, 555, 0), Vector3(0, 0, 555), green));
	world.Add(std::make_shared<Quad>(Point3D(0, 0, 0), Vector3(0, 555, 0), Vector3(0, 0, 555), red));
	world.Add(std::make_shared<Quad>(Point3D(343, 554, 332), Vector3(-130, 0, 0), Vector3(0, 0, -105), light));
	world.Add(std::make_shared<Quad>(Point3D(0, 0, 0), Vector3(555, 0, 0), Vector3(0, 0, 555), white));
	world.Add(std::make_shared<Quad>(Point3D(555, 555, 555), Vector3(-555, 0, 0), Vector3(0, 0, -555), white));
	world.Add(std::make_shared<Quad>(Point3D(0, 0, 555), Vector3(555, 0, 0), Vector3(0, 555, 0), white));

	std::shared_ptr<Collidable> box1 = Box(Point3D(0, 0, 0), Point3D(165, 330, 165), white);
	box1 = std::make_shared<RotateY>(box1, 15);
	box1 = std::make_shared<Translate>(box1, Vector3(265, 0, 295));

	std::shared_ptr<Collidable> box2 = Box(Point3D(0, 0, 0), Point3D(165, 165, 165), white);
	box2 = make_shared<RotateY>(box2, -18);
	box2 = make_shared<Translate>(box2, Vector3(130, 0, 65));

	world.Add(std::make_shared<ConstantMedium>(box1, 0.01, Colour(0, 0, 0)));
	world.Add(std::make_shared<ConstantMedium>(box2, 0.01, Colour(1, 1, 1)));

	world = CollidableList(std::make_shared<BVH_NODE>(world));

	return world;
}

CollidableList FinalScene(Camera& cam, int imageWidth, int samplesPerPixel, int maxDepth)
{
	CollidableList boxes1;
	auto ground = std::make_shared<Lambertian>(Colour(0.48, 0.83, 0.53));

	int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = RandomDouble(1, 101);
			auto z1 = z0 + w;

			boxes1.Add(Box(Point3D(x0, y0, z0), Point3D(x1, y1, z1), ground));
		}
	}

	CollidableList world;
	world.Add(std::make_shared<BVH_NODE>(boxes1));

	auto light = std::make_shared<DiffuseLight>(Colour(7, 7, 7));
	world.Add(std::make_shared<Quad>(Point3D(123, 554, 147), Vector3(300, 0, 0), Vector3(0, 0, 265), light));

	auto center1 = Point3D(400, 400, 200);
	auto center2 = center1 + Vector3(30, 0, 0);
	auto sphere_material = std::make_shared<Lambertian>(Colour(0.7, 0.3, 0.1));
	world.Add(std::make_shared<Sphere>(center1, center2, 50, sphere_material));

	world.Add(std::make_shared<Sphere>(Point3D(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
	world.Add(std::make_shared<Sphere>(
		Point3D(0, 150, 145), 50, std::make_shared<Metal>(Colour(0.8, 0.8, 0.9), 1.0)
	));

	auto boundary = std::make_shared<Sphere>(Point3D(360, 150, 145), 70, std::make_shared<Dielectric>(1.5));
	world.Add(boundary);
	world.Add(make_shared<ConstantMedium>(boundary, 0.2, Colour(0.2, 0.4, 0.9)));
	boundary = std::make_shared<Sphere>(Point3D(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
	world.Add(std::make_shared<ConstantMedium>(boundary, .0001, Colour(1, 1, 1)));

	auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.jpg"));
	world.Add(std::make_shared<Sphere>(Point3D(400, 200, 400), 100, emat));
	auto pertext = std::make_shared<NoiseTexture>(0.1);
	world.Add(std::make_shared<Sphere>(Point3D(220, 280, 300), 80, std::make_shared<Lambertian>(pertext)));

	CollidableList boxes2;
	auto white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.Add(std::make_shared<Sphere>(Point3D::Random(0, 165), 10, white));
	}

	world.Add(std::make_shared<Translate>(
		std::make_shared<RotateY>(
			std::make_shared<BVH_NODE>(boxes2), 15),
		Vector3(-100, 270, 395)
		)
	);

	cam.m_aspectRatio = 1.0;
	cam.m_imageWidth = imageWidth;
	cam.m_samplesPerPixel = samplesPerPixel;
	cam.m_maxDepth = maxDepth;
	cam.m_background = Colour(0, 0, 0);

	cam.m_verticalFOV = 40;
	cam.m_lookfrom = Point3D(478, 278, -600);
	cam.m_lookat = Point3D(278, 278, 0);
	cam.m_vup = Vector3(0, 1, 0);

	cam.m_defocusAngle = 0;

	return world;
}