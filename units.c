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
struct SC_Shot {
    SDL_Rect dst;
    struct SC_Shot* next;
    struct SC_Shot* prev;
};
char* SC_Interceptor = "data/img/interceptor.bmp";
