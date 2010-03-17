struct SC_Unit {
    char* path;
    SDL_Surface* img;
    int hp;
    int ap;
    double x,y;
    double ax,ay;
    double center;
    double from;
    double (*move)(double);
    struct SC_Unit* next;
    struct SC_Unit* prev;
};

struct SC_Unit SC_Interceptor = {
    "data/img/interceptor.bmp",
    NULL,
    100, 10, 0, 0, 0, 0, 0, 0, NULL};
/*   hp, ap, x, y,ax,ay,cnt,frm,move*/
