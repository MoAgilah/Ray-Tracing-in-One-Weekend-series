#pragma once

#include "Common.h"

class Perlin
{
public:
	Perlin()
	{
		m_ranVec = new Vector3[m_pointCount];
		for (int i = 0; i < m_pointCount; ++i)
			m_ranVec[i] = UnitVector(Vector3::Random(-1, 1));

		m_permX = PerlinGeneratePerm();
		m_permY = PerlinGeneratePerm();
		m_permZ = PerlinGeneratePerm();
	}
	~Perlin()
	{
		if (m_ranVec)
			delete[] m_ranVec;
		if (m_permX)
			delete[] m_permX;
		if (m_permY)
			delete[] m_permY;
		if (m_permZ)
			delete[] m_permZ;
	}

	double Noise(const Point3D& p) const
	{
		auto u = p.X() - floor(p.X());
		auto v = p.Y() - floor(p.Y());
		auto w = p.Z() - floor(p.Z());
		u = u * u * (3 - 2 * u);
		v = v * v * (3 - 2 * v);
		w = w * w * (3 - 2 * w);

		auto i = static_cast<int>(floor(p.X()));
		auto j = static_cast<int>(floor(p.Y()));
		auto k = static_cast<int>(floor(p.Z()));
		Vector3 c[2][2][2];

		for (int di = 0; di < 2; di++)
			for (int dj = 0; dj < 2; dj++)
				for (int dk = 0; dk < 2; dk++)
					c[di][dj][dk] = m_ranVec[
						m_permX[(i + di) & 255] ^
							m_permY[(j + dj) & 255] ^
							m_permZ[(k + dk) & 255]
					];

		return PerlinInterp(c, u, v, w);
	}

	double Turberlance(const Point3D& p, int depth = 7) const
	{
		auto accum = 0.0;
		auto temp_p = p;
		auto weight = 1.0;

		for (int i = 0; i < depth; i++)
		{
			accum += weight * Noise(temp_p);
			weight *= 0.5;
			temp_p *= 2;
		}

		return fabs(accum);
	}
private:
	static double PerlinInterp(Vector3 c[2][2][2], double u, double v, double w)
	{
		auto uu = u * u * (3 - 2 * u);
		auto vv = v * v * (3 - 2 * v);
		auto ww = w * w * (3 - 2 * w);
		auto accum = 0.0;

		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++) {
					Vector3 weightV(u - i, v - j, w - k);
					accum += (i * uu + (1 - i) * (1 - uu))
						* (j * vv + (1 - j) * (1 - vv))
						* (k * ww + (1 - k) * (1 - ww))
						* Dot(c[i][j][k], weightV);
				}

		return accum;
	}

	static int* PerlinGeneratePerm()
	{
		auto p = new int[m_pointCount];

		for (int i = 0; i < m_pointCount; i++)
			p[i] = i;

		return p;
	}

	static void Permute(int* p, int n)
	{
		for (int i = n - 1; i > 0; i--)
		{
			int target = RandomInt(0, i);
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}

	static const int m_pointCount = 256;
	Vector3* m_ranVec;
	int* m_permX;
	int* m_permY;
	int* m_permZ;
};

