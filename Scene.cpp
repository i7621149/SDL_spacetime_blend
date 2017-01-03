#include "Scene.hpp"

Scene::Scene(int width, int height)
{
	m_window = SDL_CreateWindow("SDL Window", /* The first parameter is the window title */
	                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                            width, height,
	                            SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, 0);
  SDL_RenderSetLogicalSize(m_renderer, width, height);

  m_texture = SDL_CreateTexture(m_renderer,
                                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                width, height);

  m_pixels = new Uint32[width * height];
  m_surface_in = NULL;
  m_surface_out = SDL_CreateRGBSurfaceFrom(m_pixels,
                                           width, height,
                                           32, 0,
                                           0x00FF0000,
                                           0x0000FF00,
                                           0x000000FF,
                                           0xFF000000);

  m_width = width;
  m_height = height;
  m_active = true;
	m_frame = 0;
}

Scene::~Scene()
{
  delete [] m_pixels;
  SDL_DestroyTexture(m_texture);
  SDL_FreeSurface(m_surface_in);
  SDL_FreeSurface(m_surface_out);
  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
}

void Scene::input()
{
  SDL_Event incomingEvent;
		/* check incoming events */
		while(SDL_PollEvent(&incomingEvent)){
			switch(incomingEvent.type){
			case SDL_QUIT:
				/* if user closes program, then brake refresh loop by setting go = 0 */
				m_active = 0;
				break;
			}
		}
}

void Scene::update()
{
  SDL_LockTexture(m_texture, NULL,
                  &m_surface_out->pixels,
                  &m_surface_out->pitch);

  // draw to pixels
	for(int y = 0; y < m_height; y++)
	{
		for(int x = 0; x < m_width; x++)
		{
			shader(x, y);
		}
	}
  SDL_UnlockTexture(m_texture);
}

void Scene::render()
{
  /* draw graphics */
	SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
	/* clear the entire screen to our selected colour */
	SDL_RenderClear(m_renderer);
	SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
	/* this tells the renderer to actually show its contents to the screen. */
	SDL_RenderPresent(m_renderer);
	m_frame++;
}

void Scene::openInput(std::string file_name)
{
  m_surface_in = IMG_Load(file_name.c_str());
}

void Scene::readPixel(SDL_Surface *s, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b)
{
  SDL_GetRGB(((Uint32 *)s->pixels)[x + y * s->w], s->format, r, g, b);
}

Uint32 Scene::readPixel(SDL_Surface *s, int x, int y)
{
  return ((Uint32 *)s->pixels)[x + y * s->w];
}

void Scene::writePixel(SDL_Surface *s, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
  Uint32 pixel = SDL_MapRGB(s->format, r, g, b);
  ((Uint32 *)s->pixels)[x + y * s->w] = pixel;
}

void Scene::writePixel(SDL_Surface *s, int x, int y, Uint32 c)
{
  ((Uint32 *)s->pixels)[x + y * s->w] = c;
}

int Scene::getWidth()
{
  return m_width;
}

int Scene::getHeight()
{
  return m_height;
}

int Scene::isActive()
{
  return m_active;
}

void Scene::shader(int x, int y)
{
	float t = m_frame / 10.0;
}
