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
    *ls = shot;
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

void makeEnemy(int from,struct SC_Unit unit) {
    tmpe = le;
    le = malloc(sizeof(struct SC_Unit));
    *le = unit;
    le->from = from;
    le->dst.y = 10;
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
                        makeEnemy(_X/2,SC_Interceptor);
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

    if((SDL_GetTicks() - lastEnemyTick) > 200) {
        makeEnemy((_X/2 + ((rand()%300) - 150)), SC_Interceptor);
        lastEnemyTick = SDL_GetTicks();
    }

    if(s && ((SDL_GetTicks() - lastShotTick) > 100)) {
        makeShot();
        lastShotTick = SDL_GetTicks();
    }

    player.ay += (2.5*ud);
    player.ax += (2.5*lr);

    player.ax *= FRICTION;
    player.ay *= FRICTION;

    if (player.ax * player.ax < 0.01)
        player.ax = 0;
    if (player.ay * player.ay < 0.01)
        player.ay = 0;

    player.dst.x += player.ax;
    player.dst.y += player.ay;

    if(player.dst.x > (_X - player.dst.w))
        player.dst.x = _X - player.dst.w;
    if(player.dst.x < 0)
        player.dst.x = 0;
    if(player.dst.y < 0)
        player.dst.y = 0;
    if(player.dst.y > (_Y - player.dst.h))
        player.dst.y = _Y - player.dst.h;

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
    shot.img = SC_LoadImage("data/img/shot.bmp");

    shot.dst.w = shot.img->w;
    shot.dst.h = shot.img->h;
}
void init() {
    SC_Interceptor.img = SC_LoadImage("data/img/interceptor.bmp");
    SC_Interceptor.dst.x = 0;
    SC_Interceptor.dst.y = 0;
    SC_Interceptor.dst.w = SC_Interceptor.img->w;
    SC_Interceptor.dst.h = SC_Interceptor.img->h;
    SC_Interceptor.hp = 100;
    SC_Interceptor.ap = 1;
    SC_Interceptor.from = 0;
    SC_Interceptor.next = NULL;
    SC_Interceptor.prev = NULL;
    SC_Interceptor.move = SC_sin;
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
            src.x=0;
            src.y=0;
            src.w=_X;
            src.h=_Y;

            bgy++;

            dst.x=0;
            dst.y=0 + bgy;
            dst.w=_X;
            dst.h=_Y;

            SDL_BlitSurface(background,&src,screen,&dst);

        }

        tmpe = fe;
        while(tmpe) {
            SC_DrawPartialSurface(tmpe->dst.x,tmpe->dst.y,tmpe->dst.x,tmpe->dst.y,tmpe->img->w,tmpe->img->h,background,screen);
            tmpe = tmpe->next;
        }
        tmps = fs;
        while(tmps) {
            SC_DrawPartialSurface(tmps->dst.x,tmps->dst.y,tmps->dst.x,tmps->dst.y,tmps->dst.w,tmps->dst.h,background,screen);
            tmps = tmps->next;
        }

        SC_DrawPartialSurface(player.dst.x,player.dst.y,player.dst.x,player.dst.y,player.dst.w,player.dst.h,background,screen);

        update();

        src.x=(player.img->w/3) + (player.img->w/3*lr);
        src.y=0;
        src.w=player.img->w/3;
        src.h=player.img->h;

        if(!screen) {
            printf("Screen not found! WTF!!\n");
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
            SDL_BlitSurface(tmps->img,&src,screen,&(tmps->dst));
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
    printf("Initializing Units...");
    init();
    printf("\tOK\n");

    background = SC_LoadImage("data/img/map.bmp");

    SC_DrawSurface(background->h - _Y,0,background,screen);

    srand(time(NULL));

    while(!quit){

        quit = quit | events(event) | render();

        SDL_Delay(12);
    }
    atexit(SDL_Quit);

    return 0;
}
