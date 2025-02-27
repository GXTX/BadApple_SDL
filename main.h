#ifdef NXDK
    #include <SDL.h>
    #include <SDL_ttf.h>
    //#include <SDL_mixer.h>
    #include <hal/debug.h>
    #include <hal/video.h>
    #include <hal/xbox.h>
#else
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_ttf.h"
    //#include "SDL2/SDL_mixer.h"
#endif

#define WIDTH 640
#define HEIGHT 480
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
	SDL_Surface  *fpsSurface;
	SDL_Event     event;
} SDL;

typedef struct Glyph {
	SDL_Surface *surface;
	int advance;
} Glyph;

void memoryToSurface(SDL_Surface *surface, char **video, Glyph *glyphs);
void displayFrames(SDL *sdl, TTF_Font *font);
