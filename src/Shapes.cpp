#include "Shapes.hpp"
#include <cmath>
#include <algorithm>

float Shapes::box(ngl::Vec2 position, ngl::Vec2 size, float radius)
{
	//size -= vec2(radius);
	size -= ngl::Vec2(radius, radius);
	//vec2 d = abs(p) - size;
	position.m_x = fabs(position.m_x);
	position.m_y = fabs(position.m_x);
	ngl::Vec2 d = position - size;
	//return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;
	float l = d.length();
	return std::min(std::max(d.m_x, d.m_y), 0.0f) + l - radius;
}

float Shapes::circle(ngl::Vec2 position, float radius)
{
	return position.length() - radius;
}
