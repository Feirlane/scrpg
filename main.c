#include "SDL/SDL.h"
#include "gcore.c"
#include "units.c"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define _X 800
#define _Y 600
#define _DEPTH 32
#define FRICTION 0.9

SDL_Surface *screen;
struct SC_Unit player;
int lr,ud,s; /* Left(-1) Right(+1); Up(+1) Down(-1) movement; s=shooting*/
struct SC_Unit *fe=NULL, *le=NULL,*tmpe=NULL;
int lastEnemyTick = 0;
struct SC_Shot *fs=NULL, *ls=NULL, *tmps=NULL;
int lastShotTick = 0;
int points=0;

double SC_sin (double x) {
    return 100*sin(x/50);
}
void makeShot() {
    tmps = ls;
    ls = malloc(sizeof(struct SC_Shot));
    ls->dst.w = 8;
    ls->dst.h = 8;
    ls->dst.x = player.x + player.img->w/6 - ls->dst.w/2;
    ls->dst.y = player.y - ls->dst.h;
    if (!fs) {
        ls->next = NULL;
        ls->prev = NULL;
        fs = ls;
    } else {
        ls->prev = tmps;
        tmps->next = ls;
    }
    ls->next = NULL;
}

void makeEnemy(int from) {
    tmpe = le;
    le = malloc(sizeof(struct SC_Unit));
    le->from = from;
    le->x = _X/2;
    le->y = 10;
    le->img = SC_LoadImage(SC_Interceptor);
    le->move = SC_sin;
    if(fe == NULL) {
        le->prev = NULL;
        le->next = NULL;
        fe = le;
    } else {
        le->prev = tmpe;
        tmpe->next = le;
    }
    le->next = NULL;
}
 
int events(SDL_Event event) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        makeEnemy(_X/2);
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
                        s++;
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
                    case SDLK_SPACE:
                        s--;
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
    struct SC_Unit *dummye;
    struct SC_Shot *dummys;
    SDL_Rect src,dst;

    if((SDL_GetTicks() - lastEnemyTick) > 500) {
        makeEnemy((_X/2 + ((rand()%300) - 150)));
        lastEnemyTick = SDL_GetTicks();
    }

    if(s && ((SDL_GetTicks() - lastShotTick) > 100)) {
        makeShot();
        lastShotTick = SDL_GetTicks();
    }

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

    tmpe = fe;
    while (tmpe) {
        dst.x = tmpe->x;
        dst.y = tmpe->y;
        dst.w = tmpe->img->w/3;
        dst.h = tmpe->img->h;
        SDL_FillRect(screen,&dst,0x000000);
        tmpe->x = tmpe->from + tmpe->move(tmpe->y);
        tmpe->y += 3;

        src.y=0;
        src.x=0;
        src.h=tmpe->img->h;
        src.w=tmpe->img->w/3;

        dst.x = tmpe->x;
        dst.y = tmpe->y;

        SDL_BlitSurface(tmpe->img,&src,screen,&dst);

        dummye = tmpe->next;
        if(dst.y > _Y) {
            if ((tmpe == fe) && (tmpe == le)) {
                fe = NULL;
                le = NULL;
            } else if (tmpe == fe) {
                fe = tmpe->next;
                fe->prev = NULL;
            } else if (tmpe == le) {
                le = tmpe->prev;
                le->next = NULL;
            } else {
                tmpe->prev->next = tmpe->next;
                tmpe->next->prev = tmpe->prev;
            }
            free(tmpe);
        }
        tmpe = dummye;
    }
    tmps = fs;
    while(tmps) {
        dummys = tmps->next;
        SDL_FillRect(screen,&tmps->dst,0x000000);
        tmps->dst.y -= 12;
        dst = tmps->dst;
        if(tmps->dst.y <= 0){
            if ((tmps == fs) && (tmps == ls)) {
                fs = NULL;
                ls = NULL;
            } else if (tmps == fs) {
                fs = tmps->next;
                fs->prev = NULL;
            } else if (tmps == ls) {
                ls = tmps->prev;
                ls->next = NULL;
            } else {
                tmps->prev->next = tmps->next;
                tmps->next->prev = tmps->prev;
            }
            free(tmps);
        } else {
            SDL_FillRect(screen,&dst,0xFF0000);
        }
        tmps = dummys;
    }
    tmpe = fe;
    while(tmpe) {
        dummye = tmpe->next;
        if (((player.x + player.img->w/3 > tmpe->x) && (player.x < tmpe->x + tmpe->img->w/3))){
            if(((player.y + player.img->h) > tmpe->y) && (player.y < (tmpe->y + tmpe->img->h))){
                player.x = -200;
                player.y = -200;
            }
        }
        tmps = fs;
        while(tmps){
            dummys = tmps->next;
            if(((tmps->dst.x + tmps->dst.w) > tmpe->x) && (tmps->dst.x < (tmpe->x + tmpe->img->w/3))) {
                if (((tmps->dst.y + tmps->dst.h) > tmpe->y) && (tmps->dst.y < (tmpe->y + tmpe->img->h))) {
                    dst.x = tmpe->x;
                    dst.y = tmpe->y;
                    dst.w = tmpe->img->w/3;
                    dst.h = tmpe->img->h;
                    SDL_FillRect(screen,&dst,0x000);
                    if ((tmpe == fe) && (tmpe == le)) {
                        fe = NULL;
                        le = NULL;
                    } else if (tmpe == fe) {
                        fe = tmpe->next;
                        fe->prev = NULL;
                    } else if (tmpe == le) {
                        le = tmpe->prev;
                        le->next = NULL;
                    } else {
                        tmpe->prev->next = tmpe->next;
                        tmpe->next->prev = tmpe->prev;
                    }
                    free(tmpe);
                    points+=2;
                    break;
                }
            }
            tmps=dummys;
        }
        tmpe = dummye;
    }
    points %= 100;
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
    dst.x=5;
    dst.y=5;
    dst.h=5;
    dst.w=100;
    if(!points)
        SDL_FillRect(screen,&dst,0x000);
    dst.w=points;
    SDL_FillRect(screen,&dst,0x0F0);
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

    srand(time(NULL));

    while(!quit){

        update();

        quit = quit | events(event) | render();;

        SDL_Delay(12);
    }
    atexit(SDL_Quit);

    return 0;
}
