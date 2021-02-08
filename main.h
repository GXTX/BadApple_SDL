#ifdef NXDK
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <hal/debug.h>
#include <hal/video.h>
#else
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#endif

#include <errno.h>
#include <stdlib.h>

#ifdef NXDK
#define WIDTH 720
#define HEIGHT 480
#else
#define WIDTH 800
#define HEIGHT 640
#endif

#define BPP 32
#define FRAME_PER_SECOND 30

typedef struct Location {
	int x;
	int y;
} Location;

typedef struct Timer {
	int last_time;
	int time;
	//bool running;
} Timer;

typedef struct SDL{
    SDL_Window  *Window;
    SDL_Surface *Surface;
    SDL_Surface *FpsCount;
    SDL_Event    Event;
    SDL_Color    FontColor;
    SDL_Surface *Point;     // ??
    SDL_Surface *PrintText; // ??
    SDL_Surface *video;   // ??
} SDL;

typedef struct TTF{
    TTF_Font *Font;
    TTF_Font *FpsFont;
    TTF_Font *DefaultFont;
} TTF;

typedef struct MIX {
    Mix_Music *bgm;
    Mix_Chunk *chunk;
} MIX;

Timer Updatefps;

// Why global?
int Frame = 0;

void init_files(TTF *ttf, MIX *mix, FILE **video);
void init_backends(SDL *sdl, TTF *ttf, MIX *mix, Timer *fps);
void update_screen(SDL *sdl, Timer *fps);
void file_to_surface(SDL *sdl, TTF_Font *font, FILE **video);

void PrintFPS(SDL *sdl, TTF_Font *font);

//void Set_ScreenColor(SDL_Surface *surface, uint16_t r, uint16_t g, uint16_t b);

char *itoa(int num, char *str, int radix);
void Init(void);
void CopyToSurface(int x, int y, SDL_Surface *source, SDL_Surface *target, SDL_Rect *cli);
void SetTextColor(unsigned short r, unsigned short g, unsigned short b);
void Load_File(void);
