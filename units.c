struct SC_Unit {
    SDL_Surface* img;
    SDL_Rect dst;
    int hp;
    int ap;
    double ax,ay; /* Speed vector (ax,ay) */
    double from;
    double (*move)(double);
    struct SC_Unit* next;
    struct SC_Unit* prev;
};
struct SC_Shot {
    SDL_Surface* img;
    SDL_Rect dst;
    double ax,ay; /* Speed vector (ax,ay) */
    struct SC_Shot* next; /* Next Shot */
    struct SC_Shot* prev; /* Previous Shot */
};

struct SC_Unit SC_Interceptor;
struct SC_Shot shot;
