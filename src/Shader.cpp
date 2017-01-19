#include "Shader.hpp"

#include <cmath>
#include <fstream>
#include <iostream>

#include "Shapes.hpp"

#define UNION Shapes::unionF
#define INTERSECT Shapes::intersectF
#define SUBTRACT Shapes::subtractF

Shader::Shader(int w, int h, int length, bool useDF, DrawMode draw_mode, ColorMode color_mode, int block_size):
  m_resolution(w, h)
{
  m_length = length;
  m_image1 = loadSurface("in/img1.png");
  m_image2 = loadSurface("in/img2.png");
  m_draw_mode = draw_mode;
  m_color_mode = color_mode;
  m_block_size = block_size;

  m_DF = nullptr;
  if(useDF)
  {
    m_DF = std::make_shared<DistanceField>(w, h);
  }

  std::ifstream config_file("config.txt");
  config_file >> m_a[0];
  config_file >> m_a[1];
  config_file >> m_a[2];
  config_file >> m_a[3];
  std::cout << std::endl << "Control variables:" << std::endl;
  for(int i=0; i<4; i++)
  {
    std::cout << "    a" << i << ":" << m_a[i] << std::endl;
  }
  std::cout << std::endl;
}

Shader::~Shader()
{
  SDL_FreeSurface(m_image1);
  SDL_FreeSurface(m_image2);
}

float Shader::func1(ngl::Vec2 uv)
{
  float result = 0;
  if(m_DF)
  {
    result = m_DF.get()->lookUp1(uv);
  }
  else
  {
    uv.m_y = 1 - uv.m_y;

    ngl::Vec2 pos = ngl::Vec2(uv.m_x * 10.0 - 5.0, uv.m_y * 10.0 - 3.0);

    float disk1 = 1.0 - (pos[0] - 1.0) * (pos[0] - 1.0) - pos[1] * pos[1];
    float disk2 = 1.0 - (pos[0] + 1.5) * (pos[0] + 1.5) - pos[1] * pos[1];
    float ddisk = UNION(disk1, disk2);

    result = ddisk;

    //float b = Shapes::box(uv-ngl::Vec2(0.5,0.5), ngl::Vec2(0.1, 0.1), 0);
    //return b;
    //uv -= ngl::Vec2(0.5, 0.5);
    //float c = Shapes::circle(uv, 0.2);
    //return c;
    //float l = Shapes::line(uv, ngl::Vec2(0.5, 0.3), ngl::Vec2(0.7, 0.8), 0.1);
    //return l;

    //uv = Shapes::translate(uv, ngl::Vec2(0.5, 0.5));
    //uv = Shapes::rotate(uv, 0.7854);
    //float result = Shapes::box(uv, ngl::Vec2(0.3, 0.3), 0.0);

    //return result;
  }
  return result;
}

float Shader::func2(ngl::Vec2 uv)
{
  float result = 0;
  if(m_DF)
  {
   result = m_DF.get()->lookUp2(uv);
  }
  else
  {
    uv.m_y = 1 - uv.m_y;

    ngl::Vec2 pos = ngl::Vec2(uv.m_x * 10.0 - 5.0, uv.m_y * 10.0 - 3.0);

    float bl1 = INTERSECT(INTERSECT(INTERSECT((pos[0] + 1.0), (0.0 - pos[0])), (pos[1] - 2.0)), (5.0 - pos[1]));
    float bl2 = INTERSECT(INTERSECT(INTERSECT((pos[1] - 3.0), (4.0 - pos[1])) , (pos[0] + 2.0)), (1.0 - pos[0]));

    float cross = UNION(bl1, bl2);

    result = cross;
  }
  return result;
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
  ngl::Vec3 c((float)r/255.0, (float)g/255.0, (float)b/255.0);

  convertFromRGB(&c);

  return c;
}

void Shader::colWrite(ngl::Vec2 uv, SDL_Surface *s, ngl::Vec3 c)
{
  int x = uv.m_x * s->w;
  int y = uv.m_y * s->h;
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;

  SDL_GetRGB(((Uint32 *)s->pixels)[x + y * s->w], s->format, &r, &g, &b);
  if(std::max(r, std::max(g, b)) < 1)
  {
    ((Uint32 *)s->pixels)[x + y * s->w] = SDL_MapRGB(s->format, c.m_r*255, c.m_g*255, c.m_b*255);
  }
}

ngl::Vec3 Shader::mainImage(ngl::Vec2 coord, int frame)
{
  ngl::Vec2 uv = coord / m_resolution;

  //float t = ((float)frame * 0.2) - 10.0;
  float t = ((float)frame * 20.0/m_length) - 10.0;

  ngl::Vec3 color = shade(uv, t);

  return color;
}

ngl::Vec3 Shader::shade(ngl::Vec2 uv, float t)
{
  ngl::Vec3 s = ngl::Vec3(0.0, 0.0, 0.0);

  float a1 = m_a[0];
  float a2 = m_a[1];
  float a3 = m_a[2];
  float a4 = m_a[3];

  float zwarp = t;

  float cyl1 = INTERSECT(func1(uv), -zwarp);

  float cyl2 = INTERSECT(func2(uv), zwarp-1.0);

  float f1 = cyl1;
  float f2 = cyl2;
  float f3 = INTERSECT((t + 10.0), (10.0 - t));
  float r1 = pow((f1 / a2), 2.0) + pow((f2 / a3), 2.0);
  float r2 = 0.0;

  if (f3 >= 0.0)
  {
      r2 = pow((f3 / a4), 2.0);
  }

  float rr = 0.0;
  if(r1 >= 0.0)
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
      s = finalColor(uv, f1, f2);
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

  convertToRGB(&s);

  clampCol(&s);

  return s;
}

void Shader::clampCol(ngl::Vec3 *c)
{
  if(c->m_r <= 0) c->m_r = 0.0f;
  if(c->m_g <= 0) c->m_g = 0.0f;
  if(c->m_b <= 0) c->m_b = 0.0f;

  if(c->m_r >= 1) c->m_r = 1.0f;
  if(c->m_g >= 1) c->m_g = 1.0f;
  if(c->m_b >= 1) c->m_b = 1.0f;
}

void Shader::convertFromRGB(ngl::Vec3 *c)
{
  switch (m_color_mode) {
  case ColorMode::RGB:
    break;
  case ColorMode::HSV:
    *c = RGBtoHSV(*c);
    break;
  default:
    break;
  }
}

void Shader::convertToRGB(ngl::Vec3 *c)
{
  switch (m_color_mode) {
  case ColorMode::RGB:
    break;
  case ColorMode::HSV:
    *c = HSVtoRGB(*c);
    break;
  default:
    break;
  }
}

float Shader::min3(float a, float b, float c)
{
  return std::min(std::min(a, b), c);
}

float Shader::max3(float a, float b, float c)
{
  return std::max(std::max(a, b), c);
}

ngl::Vec3 Shader::RGBtoHSV(ngl::Vec3 c)
{
  float min, max, delta;
  float h, s, v;

  min = min3( c.m_r, c.m_g, c.m_b );
  max = max3( c.m_r, c.m_g, c.m_b );
  v = max;				// v

  delta = max - min;

  if( max != 0 )
    s = delta / max;		// s
  else {
    // r = g = b = 0
    v = 0;
    s = 0;
    h = 0;
    return ngl::Vec3(h, s, v);
  }

  if( c.m_r == max )
    h = ( c.m_g - c.m_b ) / delta;		// between yellow & magenta
  else if( c.m_g == max )
    h = 2 + ( c.m_b - c.m_r ) / delta;	// between cyan & yellow
  else
    h = 4 + ( c.m_r - c.m_g ) / delta;	// between magenta & cyan

  h *= 60;				// degrees
  if( h < 0 )
    h += 360;
  if( h > 360)
    h -= 360;
  h /= 360;
  return ngl::Vec3(h, s, v);
}

ngl::Vec3 Shader::HSVtoRGB(ngl::Vec3 c)
{
  c.m_x *= 360;
  int i;
  float f, p, q, t;
  float r, g, b;

  if( c.m_y == 0 ) {
    // achromatic (grey)
    r = g = b = c.m_z;
    return ngl::Vec3(r, g, b);
  }

  c.m_x /= 60;			// sector 0 to 5
  i = floor( c.m_x );
  f = c.m_x - i;			// factorial part of h
  p = c.m_z * ( 1 - c.m_y );
  q = c.m_z * ( 1 - c.m_y * f );
  t = c.m_z * ( 1 - c.m_y * ( 1 - f ) );

  switch( i ) {
    case 0:
      r = c.m_z;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = c.m_z;
      b = p;
      break;
    case 2:
      r = p;
      g = c.m_z;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = c.m_z;
      break;
    case 4:
      r = t;
      g = p;
      b = c.m_z;
      break;
    default:		// case 5:
      r = c.m_z;
      g = p;
      b = q;
      break;
  }
  return ngl::Vec3(r, g, b);
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

ngl::Vec3 Shader::finalColor(ngl::Vec2 uv, float f1, float f2)
{
  ngl::Vec3 s;

  ngl::Vec3 c1 = col1(uv);
  ngl::Vec3 c2 = col2(uv);
  ngl::Vec3 zero(0,0,0);

  if(c1 == zero || c2 == zero)
  {
    switch (m_draw_mode)
    {
    case DrawMode::WHITE:
      if(c1 == zero) c1 = ngl::Vec3(1,1,1);
      if(c2 == zero) c2 = ngl::Vec3(1,1,1);
      break;
    case DrawMode::BLOCK_COLOR:
      colBlock(uv, f1, f2, &c1, &c2);
      break;
    case DrawMode::SIMPLE_BLEND:
      colBlend(uv, f1, f2, &c1, &c2);
      break;
    case DrawMode::SIMPLE_CLOSEST:
      colClosest(uv, f1, f2, &c1, &c2);
      break;
    default:
      break;
    }
    colWrite(uv, m_image1, c1);
    colWrite(uv, m_image2, c2);
  }

  float fa1 = fabs(f1);
  float fa2 = fabs(f2);

  float fsum = fa1 + fa2;
  float w1 = fa2/fsum;
  float w2 = fa1/fsum;

  s = c1 * w1 + c2 * w2;
  return s;
}

void Shader::colBlock(ngl::Vec2 uv, float f1, float f2, ngl::Vec3 *c1, ngl::Vec3 *c2)
{
  *c1 = col1(uv);
  *c2 = col2(uv);
}

void Shader::colBlend(ngl::Vec2 uv, float f1, float f2, ngl::Vec3 *c1, ngl::Vec3 *c2)
{
  //return ngl::Vec3(1.0, 0.0, 0.0);
  ngl::Vec3 total_c1(0.0, 0.0, 0.0);
  ngl::Vec3 total_c2(0.0, 0.0, 0.0);

  float total_w1 = 0;
  float total_w2 = 0;
  float result1 = func1(uv);
  float result2 = func2(uv);
  if(result1 < 0 || result2 < 0)
  {
    for(float y = 0; y < 1; y += m_block_size/m_resolution.m_x)//1/m_image_resolution.m_y)
    {
      for(float x = 0; x < 1; x += m_block_size/m_resolution.m_y)//1/m_image_resolution.m_x)
      {
        ngl::Vec2 current_uv(x, y);
        float dist_squared = (uv.m_x-x)*(uv.m_x-x) + (uv.m_y-y)*(uv.m_y-y);
        float weight = 1/(dist_squared * dist_squared);
        if(result1<0 && func1(current_uv) >= 0)
        {
          ngl::Vec3 c = col1(current_uv);
          total_c1 += c * weight;
          total_w1 += weight;
        }
        if(result2<0 && func2(current_uv) >= 0)
        {
          ngl::Vec3 c = col2(current_uv);
          total_c2 += c * weight;
          total_w2 += weight;
        }
      }
    }
    if(result1 < 0) *c1 = total_c1 / total_w1;
    if(result2 < 0) *c2 = total_c2 / total_w2;
  }
}

void Shader::colClosest(ngl::Vec2 uv, float f1, float f2, ngl::Vec3 *c1, ngl::Vec3 *c2)
{
  float smallest_dist_squared1 = 10000;
  ngl::Vec2 closest_uv1;
  float smallest_dist_squared2 = 10000;
  ngl::Vec2 closest_uv2;

  for(float y = 0; y < 1; y += m_block_size/m_resolution.m_x)//1/m_image_resolution.m_y)
  {
    for(float x = 0; x < 1; x += m_block_size/m_resolution.m_y)//1/m_image_resolution.m_x)
    {
      ngl::Vec2 current_uv(x, y);
      float dist_squared = (uv.m_x-x)*(uv.m_x-x) + (uv.m_y-y)*(uv.m_y-y);
      if(func1(current_uv) >= 0 && dist_squared < smallest_dist_squared1)
      {
        smallest_dist_squared1 = dist_squared;
        closest_uv1 = current_uv;
      }
      if(func2(current_uv) >= 0 && dist_squared < smallest_dist_squared2)
      {
        smallest_dist_squared2 = dist_squared;
        closest_uv2 = current_uv;
      }
    }
  }
  *c1 = col1(closest_uv1);
  *c2 = col2(closest_uv2);
}
