#include "DistanceField.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

Point::Point(double dx, double dy) :
  m_dx(dx),
  m_dy(dy)
{

}

double Point::distSq()
{
  return (m_dx * m_dx + m_dy * m_dy);
}

DistanceField::DistanceField(int w, int h) :
  m_w(w),
  m_h(h),
  INF(calcInf()),
  INSIDE(INF, INF),
  EMPTY(0.0, 0.0),
  m_grid1(w*h, INSIDE),
  m_grid2(w*h, INSIDE),
  m_SDF1(w*h, 0.0),
  m_SDF2(w*h, 0.0)
{
  std::cout << "loading images for signed distance field creation" << std::endl;
  m_img1 = loadSurface("in/img1.png");
  m_img2 = loadSurface("in/img2.png");
  std::cout << "complete!" << std::endl;
  std::cout << "creating signed-distance field from " << "in/img1.png" << std::endl;
  createGrid(m_SDF1, m_img1);
  std::cout << "complete!" << std::endl;
  std::fill(m_grid1.begin(), m_grid1.end(), INSIDE);
  std::fill(m_grid2.begin(), m_grid2.end(), INSIDE);
  std::cout << "creating signed-distance field from " << "in/img2.png" << std::endl;
  createGrid(m_SDF2, m_img2);
  std::cout << "complete!" << std::endl;
}

float DistanceField::lookUp1(ngl::Vec2 uv)
{
  return m_SDF1[(int)(m_w*uv.m_x) + (int)(m_h*uv.m_y)*m_w];
}

float DistanceField::lookUp2(ngl::Vec2 uv)
{
  return m_SDF2[m_w*uv.m_x + m_h*uv.m_y*m_w];
}

int DistanceField::calcInf()
{
  return std::max(m_w, m_h) + 1;
}

void DistanceField::put(std::vector<Point> &grid, int x, int y, Point value)
{
  grid[x + y*m_w] = value;
}

Point DistanceField::get(std::vector<Point> &grid, int x, int y)
{
  return grid[x + y*m_w];
}

void DistanceField::compare( std::vector<Point> &grid, Point &p, int x, int y, int offsetx, int offsety )
{
  if(x+offsetx >= 0 && x+offsetx < m_w && y+offsety >= 0 && y+offsety < m_h)
  {
    Point other = get( grid, x+offsetx, y+offsety );
    other.m_dx += offsetx;
    other.m_dy += offsety;

    if (other.distSq() < p.distSq())
        p = other;
  }
}

void DistanceField::createGrid(std::vector<double> &output, SDL_Surface *img)
{
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;
  //pass0 - initialise grids
  for(int y = 0; y < m_h; y++)
  {
    for(int x = 0; x < m_w; x++)
    {
      float uv_x = (float)x/(float)m_w;
      float uv_y = (float)y/(float)m_h;
      int img_x = uv_x * img->w;
      int img_y = uv_y * img->h;
      SDL_GetRGB(((Uint32*)img->pixels)[img_x + img_y*img->w], img->format, &r, &g, &b);
      Uint8 max_c = std::max(r, std::max(g, b));
      if(max_c > 0)
      {
        put(m_grid1, x, y, INSIDE);
        put(m_grid2, x, y, EMPTY);
      }
      else
      {
        put(m_grid1, x, y, EMPTY);
        put(m_grid2, x, y, INSIDE);
      }
    }
  }
  generateDF(m_grid1);
  generateDF(m_grid2);
  mergeGrids(output);
}

void DistanceField::generateDF(std::vector<Point> &grid)
{
  // Pass 0
  for (int y=0;y<m_h;y++)
  {
    for (int x=0;x<m_w;x++)
    {
      Point p = get( grid, x, y );
      compare( grid, p, x, y, -1,  0 );
      compare( grid, p, x, y,  0, -1 );
      compare( grid, p, x, y, -1, -1 );
      compare( grid, p, x, y,  1, -1 );
      put( grid, x, y, p );
    }

    for (int x=m_w-1;x>=0;x--)
    {
      Point p = get( grid, x, y );
      compare( grid, p, x, y, 1, 0 );
      put( grid, x, y, p );
    }
  }

  // Pass 1
  for (int y=m_h-1;y>=0;y--)
  {
    for (int x=m_w-1;x>=0;x--)
    {
      Point p = get( grid, x, y );
      compare( grid, p, x, y,  1,  0 );
      compare( grid, p, x, y,  0,  1 );
      compare( grid, p, x, y, -1,  1 );
      compare( grid, p, x, y,  1,  1 );
      put( grid, x, y, p );
    }

    for (int x=0;x<m_w;x++)
    {
      Point p = get( grid, x, y );
      compare( grid, p, x, y, -1, 0 );
      put( grid, x, y, p );
    }
  }
}

void DistanceField::mergeGrids(std::vector<double> &output)
{
  for(int y = 0; y < m_h; y++)
  {
    for(int x = 0; x < m_w; x++)
    {
      double dist1 = ( sqrt( (double)get( m_grid1, x, y ).distSq() ) );
      double dist2 = ( sqrt( (double)get( m_grid2, x, y ).distSq() ) );
      output[x + y*m_w] = (dist1 - dist2) * 10 / INF;
    }
  }
}

SDL_Surface* DistanceField::loadSurface(std::string path)
{
  //Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
  if( loadedSurface == NULL )
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    exit(EXIT_FAILURE);
  }
  /*
  if(loadedSurface->w != m_w || loadedSurface->h != m_h)
  {
    printf("Image %s does not match input resolution!\n", path.c_str());
    printf("input resolution is %d x %d!\n", m_w, m_h);
    exit(EXIT_FAILURE);
  }
  */
  return loadedSurface;
}
