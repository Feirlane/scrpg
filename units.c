struct SC_Unit {
    SDL_Surface* img;
    SDL_Rect dst;
    int hp;
    int ap;
    double ax,ay;
    double center;
    double from;
    double (*move)(double);
    struct SC_Unit* next;
    struct SC_Unit* prev;
};
struct SC_Shot {
    SDL_Surface* img;
    SDL_Rect dst;
    struct SC_Shot* next;
    struct SC_Shot* prev;
};

struct SC_Unit SC_Interceptor;
struct SC_Shot shot;
