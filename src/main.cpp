#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Scene.hpp"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

DrawMode getDrawMode(std::string str);
ColorMode getColorMode(std::string str);

int main(int argc, char *argv[])
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
		/* Something went very wrong in the initialisation, all we can do is exit */
		perror("Error, could not initialise SDL");
		return -1;
	}
	/* Following is from https://www.libsdl.org/projects/SDL_image/docs/SDL_image.pdf p10
	   load support for the JPG and PNG image formats */
	int flags=IMG_INIT_JPG|IMG_INIT_PNG;
	int initted=IMG_Init(flags);
	if((initted&flags) != flags)
  {
		printf("failed to initialise IMG\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		return -2;
	}

	int width = 512;
	int height = 512;
	int frame = 50;

	// these can be user controlled
	int n_frames = 100;
	int block_size = 1;
	std::string file_out;
	DrawMode draw_mode = DrawMode::SIMPLE_BLEND;
	ColorMode color_mode = ColorMode::RGB;

  int c;
  opterr = 0;
  while ((c = getopt (argc, argv, "o:d:c:b:n:w:h:")) != -1)
  switch (c)
  {
  case 'o':
    file_out = optarg;
    break;
  case 'd':
    draw_mode = getDrawMode(optarg);
    break;
  case 'c':
    color_mode = getColorMode(optarg);
    break;
  case 'b':
    block_size = std::stoi(optarg);
    break;
  case 'n':
    n_frames = std::stoi(optarg);
    break;
  case 'w':
    width = std::stoi(optarg);
    break;
  case 'h':
    height = std::stoi(optarg);
    break;
  case '?':
    if (optopt == 'o')
      fprintf (stderr, "Option -%c requires a file prefix argument.\n", optopt);
    else if (optopt == 'd')
      fprintf (stderr, "Option -%c requires a draw mode argument.\n", optopt);
    else if (optopt == 'c')
      fprintf (stderr, "Option -%c requires a color mode argument.\n", optopt);
    else if (optopt == 'b')
      fprintf (stderr, "Option -%c requires a block size argument.\n", optopt);
    else if (optopt == 'n')
      fprintf (stderr, "Option -%c requires a frame number argument.\n", optopt);
    else if (optopt == 'w')
      fprintf (stderr, "Option -%c requires a width int argument.\n", optopt);
    else if (optopt == 'h')
      fprintf (stderr, "Option -%c requires a height int argument.\n", optopt);
    else if (isprint (optopt))
      fprintf (stderr, "Unknown option `-%c'.\n", optopt);
    else
      fprintf (stderr,
               "Unknown option character `\\x%x'.\n",
               optopt);
    return 1;
  default:
    abort ();
  }

  if(file_out.empty())
  {
    printf("\nNo Output File Specified.\n\n  !!!!!!!!!!!!!!!!!!!!!!!!\n  IMAGES WILL NOT BE SAVED\n  !!!!!!!!!!!!!!!!!!!!!!!!\n\nUse '-o filename' to write image files out\n\n");
  }
  if(draw_mode == DrawMode::SIMPLE_BLEND || draw_mode == DrawMode::SIMPLE_CLOSEST)
  {
    printf("You have chosen an expensive colour blend operation. It is recommended you preview the output first using '-d white'\n\n");
  }
  printf("---------------------\n");
  for (int index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);

  Scene scene(width, height, n_frames, 512.0/std::min(width, height), true, frame, file_out, draw_mode, color_mode, block_size);

	while(scene.isActive())
	{
		scene.input();
    scene.update();
    scene.render();
    if(!file_out.empty())
    {
      scene.saveFrame();
    }

		/* turns out SDL_WaitEvent just does an SDL_PollEvent and then an SDL_Delay(10) if there is nothing to do! */
		SDL_Delay(10);
	}

	//SDL_FreeSurface( img );
	SDL_Quit();
	IMG_Quit();
	return 0;
}

DrawMode getDrawMode(std::string str)
{
	DrawMode d = DrawMode::WHITE;
	if(str == "white")
	{
		d = DrawMode::WHITE;
	}
	else if(str == "block")
	{
		d = DrawMode::BLOCK_COLOR;
	}
	else if(str == "blend")
	{
		d = DrawMode::SIMPLE_BLEND;
	}
	else if(str == "closest")
	{
		d = DrawMode::SIMPLE_CLOSEST;
	}
	return d;
}

ColorMode getColorMode(std::string str)
{
	ColorMode c = ColorMode::RGB;
	if(str == "rgb")
	{
		c = ColorMode::RGB;
	}
	else if(str == "hsv")
	{
		c = ColorMode::HSV;
	}
	return c;
}
