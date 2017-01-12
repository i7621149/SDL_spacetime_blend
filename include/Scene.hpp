#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>
#include <vector>
#include <string>
#include <QTime>

#include "Shader.hpp"

class Scene
{
public:
  Scene(int width, int height, float scale, int frame, std::string file_out, DrawMode draw_mode);
  ~Scene();

  void input();
  void update();
  void render();

  void readPixel(SDL_Surface *s, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b);
  Uint32 readPixel(SDL_Surface *s, int x, int y);
  void writePixel(SDL_Surface *s, int x, int y, Uint8 r, Uint8 g, Uint8 b);
  void writePixel(SDL_Surface *s, int x, int y, Uint32 c);

  int getWidth();
  int getHeight();
  int isActive();

  void saveFrame();

private:
  SDL_Window *m_window;
  SDL_Renderer *m_renderer;

  SDL_Surface *m_surface_out;
  SDL_Texture *m_texture;

  Uint32 *m_pixels;

  Shader m_shader;

  int m_width;
  int m_height;
  bool m_active;
  int m_frame;

  QTime m_time;
  int m_last_frame_time;

  std::string m_file_out;
};

#endif//__SCENE_HPP__
