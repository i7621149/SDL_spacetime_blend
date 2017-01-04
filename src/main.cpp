#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Scene.hpp"

int main(int argc, char *argv[]){
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

	Scene scene(100, 100);

	while(scene.isActive())
  {
		scene.input();
    scene.update();
    scene.render();
    scene.saveFrame();

		/* turns out SDL_WaitEvent just does an SDL_PollEvent and then an SDL_Delay(10) if there is nothing to do! */
		SDL_Delay(10);
	}

	//SDL_FreeSurface( img );
	SDL_Quit();
	IMG_Quit();
	return 0;
}
