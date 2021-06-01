#include "main.h"

#define LINE_END        1   // Linux
#define FRAME_LINES     60  // Total lines in a "frame"
#define FRAME_LINE_SIZE 161 // Total number of chars on a single "frame" line

Timer Updatefps;
int Frame = 0;
Timer fps = {0, 0};

int main(void)
{
	SDL *sdl = malloc(sizeof(SDL));

#ifdef NXDK
	XVideoSetMode(WIDTH, HEIGHT, 16, REFRESH_DEFAULT);
#endif

	SDL_Init((SDL_INIT_VIDEO | SDL_INIT_EVENTS));

	sdl->window = SDL_CreateWindow("Bad Apple!!", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	sdl->windowSurface = SDL_GetWindowSurface(sdl->window);

	TTF_Init();
	TTF_Font *font = TTF_OpenFont(videoFont, 7);

	// Load up the video.
	FILE *video = fopen(videoFile, "r");

	fseek(video, 0, SEEK_END);
	int videoSize = ftell(video);
	fseek(video, 0, SEEK_SET);

	char *videoMem = malloc(videoSize);
	fread(videoMem, 1, videoSize, video);

	fclose(video);

	// Pre-render our text glyphs and populate their 'metrics'. Save some memory by not populating control characters.
	SDL_Color textColor = {0x80, 0x80, 0x80, 0xFF};
	Glyph textGlyphs[(128 - 32)];

	for (int i = 0; i < (128 - 32); i++) {
		textGlyphs[i].surface = TTF_RenderGlyph_Solid(font, i+32, textColor);
		TTF_GlyphMetrics(font, i+32, NULL, NULL, NULL, NULL, &textGlyphs[i].advance);
	}
	// ===

	int currentFrame = 0;
	int totalFrames = videoSize / (FRAME_LINE_SIZE * FRAME_LINES);

	// Now with audio!
	/*Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_Init(MIX_INIT_OGG);

	Mix_Music *audio = Mix_LoadMUS(music);

	Mix_VolumeMusic(MIX_MAX_VOLUME);
	Mix_PlayMusic(audio, 1);*/

	while (1) {
		// Clear the screen
		SDL_FillRect(sdl->windowSurface, NULL, 0);

		if (__builtin_expect((currentFrame < totalFrames), 1)) {
			memoryToSurface(sdl->windowSurface, &videoMem, &textGlyphs);
			displayFrames(sdl, font);

			if ((SDL_GetTicks() - fps.lastTime) < (1000 / FRAME_PER_SECOND)) {
				SDL_Delay(1000 / FRAME_PER_SECOND - (SDL_GetTicks() - fps.lastTime));
			}

			fps.lastTime = SDL_GetTicks();

			SDL_UpdateWindowSurface(sdl->window);
			Frame++;
			currentFrame++;
		} else {
			goto the_end;
		}

		while (SDL_PollEvent(&sdl->event)) {
			if (sdl->event.type == SDL_QUIT) {
				goto the_end;
			}
		}

	}

the_end:
	SDL_Quit();
	TTF_Quit();
	//Mix_Quit();
#ifdef NXDK
	XReboot();
#endif
	return 0;
}

void memoryToSurface(SDL_Surface *surface, char **video, Glyph *glyphs)
{
	for (int y = 0; y < FRAME_LINES; y++) {
		for (int x = 0; x < FRAME_LINE_SIZE - LINE_END; x++) {
			// With some rounding we end up with a 'perfect' scale down to 640x480.
			SDL_Rect location = {x * glyphs[(*video)[x] - 32].advance /*/ (float)1.0025*/, 
					y * glyphs[(*video)[x] - 32].surface->h / (float)1.125, 
					0, 0};
			SDL_BlitSurface(glyphs[(*video)[x] - 32].surface, NULL, surface, &location);
		}
		*video += FRAME_LINE_SIZE;
	}

	// Every 60 lines there's a blank line, we need to skip it.
	*video += LINE_END;
}

// TODO: Magic, idk
void displayFrames(SDL *sdl, TTF_Font *font)
{
	char fpsch[10] = "FPS:";
	int temptime;

	SDL_Color color = {0xFF, 0x00, 0x00, 0xFF};

	if (Updatefps.lastTime == 0) {
		Updatefps.lastTime = SDL_GetTicks();
		return;
	}

	temptime = SDL_GetTicks();
	Updatefps.time += temptime - Updatefps.lastTime;
	Updatefps.lastTime = temptime;

	if (Updatefps.time >= 1000) {
		Updatefps.time = 0;
		sprintf(fpsch, "FPS: %i", Frame);
		sdl->fpsSurface = TTF_RenderText_Solid(font, fpsch, color);
		Frame = 0;
	}

	if (sdl->fpsSurface == NULL) {
		sdl->fpsSurface = TTF_RenderText_Solid(font, "FPS: 0", color);
	}
	SDL_Rect location = {WIDTH - sdl->fpsSurface->w, 0, 0, 0};
	SDL_BlitSurface(sdl->fpsSurface, NULL, sdl->windowSurface, &location);
}
