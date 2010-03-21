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
#define FRICTION (double)0.9

SDL_Surface *screen;
SDL_Surface *background;
int bgy=0;
int lastRenderTick=0;
int lastBgRenderTick=0;
struct SC_Unit player;
int lr=0,ud=0,s; /* Left(-1) Right(+1); Up(+1) Down(-1) movement; s=shooting*/
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
    ls->dst.x = player.dst.x + player.dst.w/3 - ls->dst.w/2;
    ls->dst.y = player.dst.y - ls->dst.h;
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
    le->img = SC_LoadImage(SC_Interceptor);
    le->dst.x = _X/2;
    le->dst.y = 10;
    le->dst.w = le->img->w/3;
    le->dst.h = le->img->h;
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

    if((SDL_GetTicks() - lastEnemyTick) > 100) {
/*        makeEnemy((_X/2 + ((rand()%300) - 150))); */
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

    if (player.ax * player.ax < 0.1)
        player.ax = 0;
    if (player.ay * player.ay < 0.1)
        player.ay = 0;

    player.dst.x += player.ax;
    player.dst.y += player.ay;

    tmpe = fe;
    while (tmpe) {
        tmpe->dst.x = tmpe->from + tmpe->move(tmpe->dst.y);
        tmpe->dst.y += 3;

        dummye = tmpe->next;
        if(tmpe->dst.y > _Y) {
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
        tmps->dst.y -= 12;
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
        }
        tmps = dummys;
    }
    tmpe = fe;
    while(tmpe) {
        dummye = tmpe->next;
        if (((player.dst.x + player.dst.w > tmpe->dst.x) && (player.dst.x < tmpe->dst.x + tmpe->dst.w))){
            if(((player.dst.y + player.dst.h) > tmpe->dst.y) && (player.dst.y < (tmpe->dst.y + tmpe->dst.h))){
                player.dst.x = -200;
                player.dst.y = -200;
            }
        }
        tmps = fs;
        while(tmps){
            dummys = tmps->next;
            if(((tmps->dst.x + tmps->dst.w) > tmpe->dst.x) && (tmps->dst.x < (tmpe->dst.x + tmpe->dst.w))) {
                if (((tmps->dst.y + tmps->dst.h) > tmpe->dst.y) && (tmps->dst.y < (tmpe->dst.y + tmpe->dst.h))) {
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
    player.dst.x = _X/2;
    player.dst.y = _Y-100;
    player.dst.h = player.img->h;
    player.dst.w = player.img->w/3;
}
int render() {
    SDL_Rect src,dst;

    if((SDL_GetTicks() - lastRenderTick) > 20) {
        lastRenderTick = SDL_GetTicks();
        if(SDL_MUSTLOCK(screen))
            if (SDL_LockSurface(screen) < 0)
                return 1;

        if (((SDL_GetTicks() - lastBgRenderTick) > 40) && 0) {
            lastBgRenderTick = SDL_GetTicks();
            src.x=background->w/6;
            src.y=background->h - _Y + bgy;
            src.w=2*_X/3;
            src.h=_Y;

            bgy--;

            dst.x=_X/6;
            dst.y=0;
            dst.w=2*_X/3;
            dst.h=_Y;

            SDL_BlitSurface(background,&src,screen,&dst);
        }

        tmpe = fe;
        while(tmpe) {
            SC_DrawPartialSurface(tmpe->dst.x,tmpe->dst.y,tmpe->dst.x,tmpe->dst.y,tmpe->img->w,tmpe->img->h,background,screen);
            tmpe = tmpe->next;
        }

        update();

        src.x=(player.img->w/3) + (player.img->w/3*lr);
        src.y=0;
        src.w=player.img->w/3;
        src.h=player.img->h;

        if(!screen) {
            printf("NULL\n");
            return 0;
        }

        SDL_BlitSurface(player.img,&src,screen,&(player.dst));

        dst.x=5;
        dst.y=5;
        dst.h=5;
        dst.w=100;
        if(!points)
            SDL_FillRect(screen,&dst,0x000);
        dst.w=points;
        SDL_FillRect(screen,&dst,0xFF000);

        src.y=0;
        src.x=0;

        tmpe = fe;
        while(tmpe) {
            src.w=tmpe->img->w/3;
            src.h=tmpe->img->h;
            SDL_BlitSurface(tmpe->img,&src,screen,&(tmpe->dst));
            tmpe = tmpe->next;
        }

        tmps = fs;
        while(tmps) {
            src.w=tmps->dst.w;
            src.h=tmps->dst.h;
            SDL_FillRect(screen,&(tmps->dst),0xFF0000);
            tmps = tmps->next;
        }

        SDL_Flip(screen);

        if (SDL_MUSTLOCK(screen))
            SDL_UnlockSurface(screen);
    }
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

    background = SC_LoadImage("data/img/map.bmp");

    SC_DrawSurface(0,0,background,screen);

    srand(time(NULL));

    while(!quit){

        quit = quit | events(event) | render();

        SDL_Delay(12);
    }
    atexit(SDL_Quit);

    return 0;
}
