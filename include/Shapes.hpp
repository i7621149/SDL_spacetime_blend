#ifndef __SHAPES_HPP__
#define __SHAPES_HPP__

#include <ngl/Vec2.h>

struct Shapes
{
public:
  static float box(ngl::Vec2 position, ngl::Vec2 size, float radius);
  static float circle(ngl::Vec2 position, float radius);
  static float line(ngl::Vec2 position, ngl::Vec2 start, ngl::Vec2 end, float width);

  static ngl::Vec2 translate(ngl::Vec2 position, ngl::Vec2 t);
  static ngl::Vec2 rotate(ngl::Vec2 position, float angle);

  static float intersectF(float a, float b);
  static float unionF(float a, float b);
  static float subtractF(float a, float b);

  static ngl::Vec2 abs(ngl::Vec2 v);
  static ngl::Vec2 max(ngl::Vec2 a, float b);
  static float dot(ngl::Vec2 a, ngl::Vec2 b);
};

#endif//__SHAPES_HPP__
