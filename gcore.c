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

void SC_DrawSurface(double x, double y, SDL_Surface* image, SDL_Surface* screen) {
    SDL_Rect src,dst;

    src.x=0;
    src.y=0;
    src.w=image->w;
    src.h=image->h;

    dst.x=x;
    dst.y=y;
    dst.w=image->w;
    dst.h=image->h;

    SDL_BlitSurface(image,&src,screen,&dst);
}

void SC_DrawPartialSurface(double srcx, double srcy, double x, double y, double w, double h, SDL_Surface* image, SDL_Surface* screen) {
    SDL_Rect src,dst;

    src.x=srcx;
    src.y=srcy;
    src.w=w;
    src.h=h;

    dst.x=x;
    dst.y=y;
    dst.w=w;
    dst.h=h;

    SDL_BlitSurface(image,&src,screen,&dst);
}
