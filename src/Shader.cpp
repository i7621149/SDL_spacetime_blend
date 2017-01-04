#include "Shader.hpp"

#include <cmath>

Shader::Shader(int w, int h):
  m_resolution(w, h)
{
  m_image1 = loadSurface("textures/balls.png");
  m_image2 = loadSurface("textures/wood.png");
}

Shader::~Shader()
{
  SDL_FreeSurface(m_image1);
  SDL_FreeSurface(m_image2);
}

float Shader::func1(ngl::Vec2 uv)
{
  uv.m_y = 1 - uv.m_y;

  ngl::Vec2 pos = ngl::Vec2(uv.m_x * 10.0 - 5.0, uv.m_y * 10.0 - 3.0);

  float disk1 = 1.0 - (pos[0] - 1.0) * (pos[0] - 1.0) - pos[1] * pos[1];
  float disk2 = 1.0 - (pos[0] + 1.5) * (pos[0] + 1.5) - pos[1] * pos[1];
  float ddisk = fUnion(disk1, disk2);

  return ddisk;
}

float Shader::func2(ngl::Vec2 uv)
{
  uv.m_y = 1 - uv.m_y;

  ngl::Vec2 pos = ngl::Vec2(uv.m_x * 10.0 - 5.0, uv.m_y * 10.0 - 3.0);

  float bl1 = fIntersect(fIntersect(fIntersect((pos[0] + 1.0), (0.0 - pos[0])), (pos[1] - 2.0)), (5.0 - pos[1]));
  float bl2 = fIntersect(fIntersect(fIntersect((pos[1] - 3.0), (4.0 - pos[1])) , (pos[0] + 2.0)), (1.0 - pos[0]));

  float cross = fUnion(bl1, bl2);

  return cross;
}

ngl::Vec3 Shader::col1(ngl::Vec2 uv)
{
  return colLookup(uv, m_image1);
}

ngl::Vec3 Shader::col2(ngl::Vec2 uv)
{
  return colLookup(uv, m_image2);
}

ngl::Vec3 Shader::colLookup(ngl::Vec2 uv, SDL_Surface *s)
{
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;

  int x = uv.m_x * s->w;
  int y = uv.m_y * s->h;

  SDL_GetRGB(((Uint32 *)s->pixels)[x + y * s->w], s->format, &r, &g, &b);

  return ngl::Vec3((float)r/255.0, (float)g/255.0, (float)b/255.0);
}

ngl::Vec3 Shader::mainImage(ngl::Vec2 coord, int frame)
{
  ngl::Vec2 uv = coord / m_resolution;

  float t = ((float)frame * m_speed) - 10.0;

  ngl::Vec3 color = shade(uv, t);

  return color;
}

ngl::Vec3 Shader::shade(ngl::Vec2 uv, float t)
{
  ngl::Vec3 s = ngl::Vec3(0.0, 0.0, 0.0);

  float a1 = 5.0;
  float a2 = 2.0;
  float a3 = 3.0;
  float a4 = 0.75;

  float zwarp = t;

  float cyl1 = fIntersect(func1(uv), -zwarp);

  float cyl2 = fIntersect(func2(uv), zwarp-1.0);

  float f1 = cyl1;
  float f2 = cyl2;
  float f3 = fIntersect((t + 10.0), (10.0 - t));
  float r1 = pow((f1 / a2), 2.0) + pow((f2 / a3), 2.0);
  float r2 = 0.0;

  if (f3 > 0.0)
  {
      r2 = pow((f3 / a4), 2.0);
  }

  float rr = 0.0;
  if(r1 > 0.0)
  {
      rr = r1 / (r1 + r2);
  }

  float d = 0.0;
  if (rr < 1.0)
  {
      d = a1 * pow((1.0 - rr), 3.0) / (1.0 + rr);
  }
  float bb2diskcross = f1 + f2 + sqrt(pow(f1 , 2.0) + pow(f2, 2.0)) + d;

  /*
  if(bb2diskcross >= 0)
  {
    s.m_r = 1.0;
    s.m_g = 1.0;
    s.m_b = 1.0;
  }
  else
  {
    s.m_r = 0.0;
    s.m_g = 0.0;
    s.m_b = 0.0;
  }
  */

  if(bb2diskcross >= 0)
  {
    // color distribution in 4D space-time using partition of unity
    if(f1 < 0.0 && f2 < 0.0)
    {
      s = colBlend(uv, f1, f2);
    }
    else
    {
      if(f1 >= 0.0)
      {
         s = col1(uv);
      }

      if(f2 >= 0.0)
      {
         s = col2(uv);
      }
    }
  }

  //clampCol(&s);

  return s;
}

float Shader::fUnion(float a, float b)
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

float Shader::fIntersect(float a, float b)
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

void Shader::clampCol(ngl::Vec3 *c)
{
  if(c->m_r < 0) c->m_r = 0;
  if(c->m_g < 0) c->m_g = 0;
  if(c->m_b < 0) c->m_b = 0;

  if(c->m_r > 1) c->m_r = 1;
  if(c->m_g > 1) c->m_g = 1;
  if(c->m_b > 1) c->m_b = 1;
}

SDL_Surface* Shader::loadSurface(std::string path)
{
  //Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
  if( loadedSurface == NULL )
  {
    printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
  }

  return loadedSurface;
}

ngl::Vec3 Shader::colBlend(ngl::Vec2 uv, float f1, float f2)
{
  //return ngl::Vec3(1.0, 0.0, 0.0);
  ngl::Vec3 total_c1(0.0, 0.0, 0.0);
  ngl::Vec3 total_c2(0.0, 0.0, 0.0);

  float total_w1 = 0;
  float total_w2 = 0;

  for(float y = 0; y < 1; y += 1/m_resolution.m_y)
  {
    for(float x = 0; x < 1; x += 1/m_resolution.m_x)
    {
      ngl::Vec2 current_uv(x, y);
      float dist_squared = (uv.m_x-x)*(uv.m_x-x) + (uv.m_y-y)*(uv.m_y-y);
      float weight = 1/(dist_squared * dist_squared);
      if(func1(current_uv) >= 0)
      {
        total_c1 += col1(current_uv) * weight;
        total_w1 += weight;
      }
      if(func2(current_uv) >= 0)
      {
        total_c2 += col2(current_uv) * weight;
        total_w2 += weight;
      }
    }
  }

  ngl::Vec3 c1 = total_c1 / total_w1;
  ngl::Vec3 c2 = total_c2 / total_w2;

  float fa1 = fabs(f1);
  float fa2 = fabs(f2);

  float fsum = fa1 + fa2;
  float w1 = fa2/fsum;
  float w2 = fa1/fsum;

  ngl::Vec3 s = c1 * w1 + c2 * w2;
  return s;
}
