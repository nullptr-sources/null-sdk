#pragma once
#include <cmath>

#include <utils/fast_operators.h>

struct vec2_t {
	float x{ }, y{ };

	vec2_t() = default;
	template <typename t> vec2_t(t _value) : x(_value), y(_value) { }
	template <typename t1, typename t2> vec2_t(t1 _x, t2 _y) : x(_x), y(_y) { }

	float length() const { return std::sqrt(std::pow(x, 2) + std::pow(y, 2)); }

	class_create_operators(vec2_t, -, { return vec2_t(-x, -y); });
	class_create_arithmetic_operators(vec2_t, +, { return vec2_t(x + a.x, y + a.y); }, { x += a.x; y += a.y; return *this; });
	class_create_arithmetic_operators(vec2_t, -, { return vec2_t(x - a.x, y - a.y); }, { x -= a.x; y -= a.y; return *this; });
	class_create_arithmetic_operators(vec2_t, *, { return vec2_t(x * a.x, y * a.y); }, { x *= a.x; y *= a.y; return *this; });
	class_create_arithmetic_operators(vec2_t, /, { return vec2_t(x / a.x, y / a.y); }, { x /= a.x; y /= a.y; return *this; });

	bool operator==(const vec2_t& a) const = default;
	class_create_logic_operators(vec2_t, <, { return x < a.x && y < a.y; }, { return x <= a.x && y <= a.y; });
	class_create_logic_operators(vec2_t, >, { return x > a.x && y > a.y; }, { return x >= a.x && y >= a.y; });
};