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

#define FRAME_PER_SECOND 30

#ifdef NXDK
	static const char *videoFont  = "D:\\resource\\consola.ttf";
	//static const char *music = "D:\\resource\\Badapple.ogg";
	static const char *videoFile = "D:\\resource\\AscPic.txt";
#else
	static const char *videoFont  = "resource/consola.ttf";
	//static const char *music = "resource/Badapple.mp3";
	static const char *videoFile = "resource/AscPic.txt";
#endif

typedef struct Timer {
	uint32_t lastTime;
	int time;
} Timer;

typedef struct SDL{
    SDL_Window   *window;
    SDL_Surface  *windowSurface;
    SDL_Surface  *fpsCount;
    SDL_Event     event;
    SDL_Surface  *video;   // ??
} SDL;

typedef struct MIX {
    Mix_Music *bgm;
    Mix_Chunk *chunk;
} MIX;

Timer Updatefps;

// Why global?
int Frame = 0;

void update_screen(SDL *sdl, Timer *fps);
void file_to_surface(SDL *sdl, TTF_Font *font, FILE **video);
void PrintFPS(SDL *sdl, TTF_Font *font);
void CopyToSurface(int x, int y, SDL_Surface *source, SDL_Surface *target, SDL_Rect *cli);
