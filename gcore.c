#include <stdio.h>
#include "SDL/SDL.h"

SDL_Surface* SC_InitVideo(int x, int y, int depth) {
	SDL_Surface *surface;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Init video error\n");
		return NULL;
	}
	surface = SDL_SetVideoMode(x,y,depth, SDL_DOUBLEBUF | SDL_ANYFORMAT);
	if (surface == NULL) {
		printf("Unable to set video mode:%s\n",SDL_GetError());
		return NULL;
	}
	return surface;
}

SDL_Surface* SC_LoadImage(char* filename) {
	SDL_Surface *temp;
	SDL_Surface *image;

	temp = SDL_LoadBMP(filename);

	if (temp == NULL) {
		printf("Unable to load BMP:%s\n",SDL_GetError());
		return NULL;
	}
	image = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	SDL_SetColorKey(image,SDL_SRCCOLORKEY,0xFF00FF);

	return image;
}
