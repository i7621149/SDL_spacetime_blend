#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <vector>

#include <ngl/Vec3.h>
#include <ngl/Vec2.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

enum class DrawMode
{
	WHITE,
	BLOCK_COLOR,
	SIMPLE_BLEND,
	SIMPLE_CLOSEST
};

enum class ColorMode
{
	RGB,
	HSV
};

class Shader
{
public:
  Shader(int w, int h, DrawMode draw_mode, ColorMode color_mode, int block_size);
  ~Shader();

  ngl::Vec3 mainImage(ngl::Vec2 coord, int frame);

private:
  float func1(ngl::Vec2 uv);
  float func2(ngl::Vec2 uv);

  ngl::Vec3 col1(ngl::Vec2 uv);
  ngl::Vec3 col2(ngl::Vec2 uv);

  ngl::Vec3 colLookup(ngl::Vec2 uv, SDL_Surface *s);

  ngl::Vec3 shade(ngl::Vec2 uv, float t);

  //float fUnion(float a, float b);
  //float fIntersect(float a, float b);

  void clampCol(ngl::Vec3 *c);
  void convertFromRGB(ngl::Vec3 *c);
  void convertToRGB(ngl::Vec3 *c);

  float min3(float a, float b, float c);
  float max3(float a, float b, float c);

  ngl::Vec3 RGBtoHSV(ngl::Vec3 c);
  ngl::Vec3 HSVtoRGB(ngl::Vec3 c);

  SDL_Surface *loadSurface(std::string path);

  ngl::Vec3 finalColor(ngl::Vec2 uv, float f1, float f2);

  ngl::Vec3 colBlock(ngl::Vec2 uv, float f1, float f2);
  ngl::Vec3 colBlend(ngl::Vec2 uv, float f1, float f2);
  ngl::Vec3 colBlend2(ngl::Vec2 uv, float f1, float f2);
  ngl::Vec3 colClosest(ngl::Vec2 uv, float f1, float f2);
  //ngl::Vec3 colLast(ngl::Vec2 uv, float f1, float f2);

  ngl::Vec2 m_resolution;

  float m_speed = 0.2;

  SDL_Surface *m_image1;
  SDL_Surface *m_image2;

  DrawMode m_draw_mode;
  ColorMode m_color_mode;

  int m_block_size;
};

#endif//__SHADER_HPP__
