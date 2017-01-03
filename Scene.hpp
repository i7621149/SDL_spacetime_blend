#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>
#include <vector>
#include <string>

class Scene
{
  public:
    Scene(int width, int height);
    ~Scene();

    void input();
    void update();
    void render();

    void openInput(std::string file_name);

    void readPixel(SDL_Surface *s, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b);
    Uint32 readPixel(SDL_Surface *s, int x, int y);
    void writePixel(SDL_Surface *s, int x, int y, Uint8 r, Uint8 g, Uint8 b);
    void writePixel(SDL_Surface *s, int x, int y, Uint32 c);

    int getWidth();
    int getHeight();
    int isActive();

    void shader(int x, int y);

  private:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;

    SDL_Surface *m_surface_in;
    SDL_Surface *m_surface_out;
    SDL_Texture *m_texture;

    Uint32 *m_pixels;

    int m_width;
    int m_height;
    bool m_active;
    int m_frame;
};
