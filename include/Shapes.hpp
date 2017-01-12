#ifndef __SHAPES_HPP__
#define __SHAPES_HPP__

#include <ngl/Vec2.h>

struct Shapes
{
  static float box(ngl::Vec2 position, ngl::Vec2 size, float radius);
  static float circle(ngl::Vec2 position, float radius);
};

#endif//__SHAPES_HPP__
