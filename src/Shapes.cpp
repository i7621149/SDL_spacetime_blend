#include "Shapes.hpp"
#include <cmath>
#include <algorithm>

float Shapes::box(ngl::Vec2 position, ngl::Vec2 size, float radius)
{
	return -(max(abs(position)-size,0.0).length() - radius);
}

float Shapes::circle(ngl::Vec2 position, float radius)
{
	return radius - position.length();
}

float Shapes::line(ngl::Vec2 position, ngl::Vec2 start, ngl::Vec2 end, float width)
{
	/*
	 * float lineDist(vec2 p, vec2 start, vec2 end, float width)
{
	vec2 dir = start - end;
	float lngth = length(dir);
	dir /= lngth;
	vec2 proj = max(0.0, min(lngth, dot((start - p), dir))) * dir;
	return length( (start - p) - proj ) - (width / 2.0);
}
*/
	ngl::Vec2 dir = start-end;
	float len = dir.length();
	dir/= len;
	float d_product = dot((start - position), dir);
	float minimum = std::min(len, d_product);
	float maximum = std::max(0.0f, minimum);
	ngl::Vec2 proj(maximum * dir);
	return -(((start - position) - proj).length() - (width / 2.0));
}

ngl::Vec2 Shapes::translate(ngl::Vec2 position, ngl::Vec2 translation)
{
	return position - translation;
}

ngl::Vec2 Shapes::rotate(ngl::Vec2 position, float angle)
{
	float x = position.m_x * cos(angle) - position.m_y * sin(angle);
	float y = position.m_x * sin(angle) + position.m_y * cos(angle);
	return ngl::Vec2(x, y);
}

float Shapes::unionF(float a, float b)
{
  /*
  float max = a;
  if(a < b)
  {
    max = b;
  }
  return max;
  */
  return a + b + std::sqrt(a * a + b * b);
}

float Shapes::intersectF(float a, float b)
{
  /*
  float min = a;
  if(a > b)
  {
    min = b;
  }
  return min;
  */
  return a + b - std::sqrt(a * a + b * b);
}

float Shapes::subtractF(float a, float b)
{
  return intersectF(a, -b);
}

ngl::Vec2 Shapes::abs(ngl::Vec2 v)
{
	v.m_x = fabs(v.m_x);
	v.m_y = fabs(v.m_y);
	return v;
}

ngl::Vec2 Shapes::max(ngl::Vec2 a, float b)
{
	ngl::Vec2 result;
	result.m_x = a.m_x > b ? a.m_x : b;
	result.m_y = a.m_y > b ? a.m_y : b;
	return result;
}

float Shapes::dot(ngl::Vec2 a, ngl::Vec2 b)
{
	float c = a.m_x * b.m_x + a.m_y * b.m_y;
	return c;
}
