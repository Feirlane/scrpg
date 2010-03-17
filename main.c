#include "SDL/SDL.h"
#include "gcore.c"
#include "units.c"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define _X 800
#define _Y 600
#define _DEPTH 32
#define FRICTION 0.9
#define _MAX_ENEMIES 10
#define _MAX_SHOTS 10

SDL_Surface *screen;
struct SC_Unit player;
struct SC_Unit enemies[_MAX_ENEMIES];
SDL_Rect shots[_MAX_SHOTS];
int lr,ud; /* Left(-1) Right(+1); Up(+1) Down(-1) movement */
int ns,fs,ls = 0; /* Number of Shots; First Shot; Last Shot */
int ne,fe,le =0; /* N enemies; Last Enemy; First Enemy */
struct SC_Unit *fee=NULL,*lee=NULL,*tmp=NULL;

double SC_sin (double x) {
    return 100*sin(x/50);
}
int events(SDL_Event event) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        if(ne < _MAX_ENEMIES-1){
                            tmp = lee;
                            lee = malloc(sizeof(struct SC_Unit));
                            lee->from = _X/2;
                            lee->x = _X/2;
                            lee->y = 10;
                            lee->img = SC_LoadImage(SC_Interceptor.path);
                            lee->move = SC_sin;
                            if(fee == NULL) {
                                printf("cfee\n");
                                lee->prev = NULL;
                                lee->next = NULL;
                                fee = lee;
                            } else {
                                lee->prev = tmp;
                                tmp->next = lee;
                            }
                            lee->next = NULL;

                            enemies[le] = SC_Interceptor;
                            enemies[le].from = _X/2;
                            enemies[le].img = SC_LoadImage(SC_Interceptor.path);
                            enemies[le].move = SC_sin;
                            ne++;
                            le = (le+1) % _MAX_ENEMIES;
                        }
                        break;
                    case SDLK_q:
                        return 1;
                        break;
                     case SDLK_LEFT:
                        lr--;
                        break;
                    case SDLK_UP:
                        ud--;
                        break;
                    case SDLK_RIGHT:
                        lr++;
                        break;
                    case SDLK_DOWN:
                        ud++;
                        break;
                    case SDLK_SPACE:
                        if(ns < _MAX_SHOTS-1){
                            shots[ls].x=player.x + player.img->w/6 - 5;
                            shots[ls].y=player.y+10;
                            shots[ls].w=10;
                            shots[ls].h=10;
                            ns++;
                            ls = (ls+1) % _MAX_SHOTS;
                        }
                        break;
                    default:
                        break;
               }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT:
                        lr++;
                        break;
                    case SDLK_UP:
                        ud++;
                        break;
                    case SDLK_RIGHT:
                        lr--;
                        break;
                    case SDLK_DOWN:
                        ud--;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                return 1;
                break;
            default:
                break;
        }
    }
    return 0;
}
void update() {
    struct SC_Unit *dummy;
    SDL_Rect src,dst;
    int i,j;
/*    int g;*/

    player.ay += (1.5*ud);
    player.ax += (1.5*lr);

    player.ax *= FRICTION;
    player.ay *= FRICTION;

    dst.x = player.x;
    dst.y = player.y;
    dst.w = player.img->w/3;
    dst.h = player.img->h;

    SDL_FillRect(screen,&dst,0x000);

    player.x += player.ax;
    player.y += player.ay;

    tmp = fee;
    while (tmp) {
        dst.x = tmp->x;
        dst.y = tmp->y;
        dst.w = tmp->img->w/3;
        dst.h = tmp->img->h;
        SDL_FillRect(screen,&dst,0x000000);
        tmp->x = tmp->from + tmp->move(tmp->y);
        tmp->y += 3;

        src.y=0;
        src.x=0;
        src.h=tmp->img->h;
        src.w=tmp->img->w/3;

        dst.x = tmp->x;
        dst.y = tmp->y;

        SDL_BlitSurface(tmp->img,&src,screen,&dst);

        dummy = tmp->next;
        if(dst.y > _Y) {
            if (tmp->prev == NULL) {
                fee = tmp->next;
                if (fee)
                    fee->prev = NULL;
            }
            if (tmp->next == NULL) {
                lee = tmp->prev;
                if(lee)
                    lee->next = NULL;
            } else {
                tmp->prev->next = tmp->next;
                tmp->next->prev = tmp->prev;
            }
            free(tmp);
            ne--;
        }
        tmp = dummy;
    }
    /*        for(i=fe;i!=le;i=(i+1)%_MAX_ENEMIES){
              dst.x = enemies[i].x;
              dst.y = enemies[i].y;
              dst.w = enemies[i].img->w/3;
              dst.h = enemies[i].img->h;
              SDL_FillRect(screen,&dst,0x000);


              g = enemies[i].x;
              enemies[i].x = enemies[i].from + enemies[i].move(enemies[i].y);
              enemies[i].y += 3;

              if(enemies[i].x > g)
              src.x=(enemies[i].img->w/3)*2;
              else if (enemies[i].x < g)
              src.x=enemies[i].img->w/3;
              else
                src.x=0;
            src.y=0;
            src.h=enemies[i].img->h;
            src.w=enemies[i].img->w/3;

            dst.x=enemies[i].x;
            dst.y=enemies[i].y;
            dst.h=enemies[i].img->h;
            dst.w=enemies[i].img->w/3;

            SDL_BlitSurface(enemies[i].img,&src,screen,&dst);

            if(dst.y > _Y) {
                fe = (fe+1)%_MAX_ENEMIES;
                ne--;
            }
        }
        */
    if (ns) {
        for(i=fs;i!=ls;i=(i+1)%_MAX_SHOTS){
            SDL_FillRect(screen,&shots[i],0x000000);
            shots[i].y -= 12;
            if(shots[i].y <= 0){
                ns--;
                fs = (fs+1) % _MAX_SHOTS;
            } else {
            SDL_FillRect(screen,&shots[i],0xFF0000);
            }
        }
    }
    tmp = fee;
    while(tmp) {
        dummy = tmp->next;
        if (((player.x + player.img->w/3 > tmp->x) && (player.x < tmp->x + tmp->img->w/3))){
            if(((player.y + player.img->h) > tmp->y) && (player.y < (tmp->y + tmp->img->h))){
                player.x = -200;
                player.y = -200;
            }
        }
/*    for(i=fe;i!=le;i=(i+1)%_MAX_ENEMIES){
        if (((player.x + player.img->w/3) > enemies[i].x) && (player.x < (enemies[i].x + enemies[i].img->w/3))){
            if (((player.y + player.img->h) > enemies[i].y) && (player.y < (enemies[i].y + enemies[i].img->h))) {
                player.x=-200;
                player.y=-200;
            }
        }
        */
        for(j=fs;j!=ls;j=(j+1)%_MAX_SHOTS){
            if(((shots[j].x + 10) > tmp->x) && (shots[j].x < (tmp->x + tmp->img->w/3))) {
                if (((shots[j].y + 10) > tmp->y) && (shots[j].y < (tmp->y + tmp->img->h))) {
                    dst.x = tmp->x;
                    dst.y = tmp->y;
                    dst.w = tmp->img->w/3;
                    dst.h = tmp->img->h;
                    SDL_FillRect(screen,&dst,0x000);
                    if (tmp->prev == NULL) {
                        fee = tmp->next;
                        if(fee)
                            fee->prev = NULL;
                    } 
                    if (tmp->next == NULL) {
                        lee = tmp->prev;
                        if (lee)
                            lee->next = NULL;
                    }
                    if (tmp->next)
                        tmp->prev->next = tmp->next;
                    if (tmp->prev);
                        tmp->next->prev = tmp->prev;
                    free(tmp);
                    ne--;
                }
            }
        }
        tmp = dummy;
    }
}

void loadPlayer() {
    player.img = SC_LoadImage("data/img/wraith.bmp");
    player.hp = 100;
    player.ap = 10;
    player.x = _X/2;
    player.y = _Y-100;
}
int render() {
    SDL_Rect src,dst;

    if(SDL_MUSTLOCK(screen))
        if (SDL_LockSurface(screen) < 0)
            return 1;

    src.x=(player.img->w/3) + (player.img->w/3*lr);
    src.y=0;
    src.w=player.img->w/3;
    src.h=player.img->h;

    dst.x=player.x;
    dst.y=player.y;
    dst.w=player.img->w/3;
    dst.h=player.img->h;

    if(!screen) {
        printf("NULL\n");
        return 0;
    }

    SDL_BlitSurface(player.img,&src,screen,&dst);
    SDL_Flip(screen);

    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);
    return 0;
}

int main() {
    SDL_Event event;
    int quit=0;

    printf("Initializing Screen...");
    screen = SC_InitVideo(_X,_Y,_DEPTH);
    SDL_FillRect(screen,NULL,0x000000);
    printf("\tOK\n");
    printf("Loading Player...");
    loadPlayer();
    printf("\tOK\n");
    while(!quit){

        update(enemies);

        quit = quit | events(event) | render();;

        SDL_Delay(12);
    }
    atexit(SDL_Quit);

    return 0;
}
