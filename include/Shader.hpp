#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <ngl/Vec3.h>
#include <ngl/Vec2.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Shader
{
public:
  Shader(int w, int h);
  ~Shader();

  ngl::Vec3 mainImage(ngl::Vec2 coord, int frame);


private:
  float func1(ngl::Vec2 uv);
  float func2(ngl::Vec2 uv);

  ngl::Vec3 col1(ngl::Vec2 uv);
  ngl::Vec3 col2(ngl::Vec2 uv);

  ngl::Vec3 colLookup(ngl::Vec2 uv, SDL_Surface *s);

  ngl::Vec3 shade(ngl::Vec2 uv, float t);

  float fUnion(float a, float b);
  float fIntersect(float a, float b);

  void clampCol(ngl::Vec3 *c);

  SDL_Surface *loadSurface(std::string path);

  ngl::Vec3 colBlend(ngl::Vec2 uv, float f1, float f2);

  ngl::Vec2 m_resolution;

  float m_speed = 0.2;

  SDL_Surface *m_image1;
  SDL_Surface *m_image2;
};

#endif//__SHADER_HPP__
