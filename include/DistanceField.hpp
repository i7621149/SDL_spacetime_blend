#ifndef __DISTANCE_FIELD_HPP__
#define __DISTANCE_FIELD_HPP__

#include <vector>
#include <string>
#include <SDL2/SDL.h>

#include <ngl/Vec2.h>

struct Point
{
  Point(double dx, double dy);

  double m_dx, m_dy;

  double distSq();
};

class DistanceField
{
public:
  DistanceField(int w, int h);
  int m_w;
  int m_h;

  float lookUp1(ngl::Vec2 uv);
  float lookUp2(ngl::Vec2 uv);

private:
  int calcInf();

  void put(std::vector<Point> &grid, int x, int y, Point value);
  Point get(std::vector<Point> &grid, int x, int y);
  void compare( std::vector<Point> &grid, Point &p, int x, int y, int offsetx, int offsety );

  void createGrid(std::vector<double> &output, SDL_Surface *img);

  void generateDF(std::vector<Point> &grid);

  void mergeGrids(std::vector<double> &output);

  SDL_Surface *loadSurface(std::string path);

  const int INF;
  const Point INSIDE;
  const Point EMPTY;

  SDL_Surface *m_img1;
  SDL_Surface *m_img2;

  std::vector<Point> m_grid1;
  std::vector<Point> m_grid2;
  std::vector<double> m_SDF1;
  std::vector<double> m_SDF2;
};

#endif//__DISTANCE_FIELD_HPP__
