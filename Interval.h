#pragma once

class Interval {
public:
	double m_min, m_max;

	Interval() : m_min(+g_infinity), m_max(-g_infinity) {} // Default Interval is empty

	Interval(double _min, double _max) : m_min(_min), m_max(_max) {}

	Interval(const Interval& a, const Interval& b)
		: m_min(fmin(a.m_min, b.m_min)), m_max(fmax(a.m_max, b.m_max)) {}

	double size() const {
		return m_max - m_min;
	}

	Interval expand(double delta) const {
		auto padding = delta / 2;
		return Interval(m_min - padding, m_max + padding);
	}

	bool contains(double x) const {
		return m_min <= x && x <= m_max;
	}

	bool surrounds(double x) const {
		return m_min < x && x < m_max;
	}

	double clamp(double x) const {
		if (x < m_min) return m_min;
		if (x > m_max) return m_max;
		return x;
	}
};

const static Interval empty = Interval(+g_infinity, -g_infinity);
const static Interval universe = Interval(-g_infinity, +g_infinity);

namespace {
	Interval operator+(const Interval& ival, double displacement) {
		return Interval(ival.m_min + displacement, ival.m_max + displacement);
	}

	Interval operator+(double displacement, const Interval& ival) {
		return ival + displacement;
	}
}