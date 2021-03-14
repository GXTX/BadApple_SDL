#include "main.h"

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

	TTF_Font *font = TTF_OpenFont(videoFont, 9);

	FILE *video = fopen(videoFile, "r");

	// Push the entire 'video' into a buffer in memory.
	char *video_mem = malloc((1024 * 1000 * 32));
	fread(video_mem, 1, (1024 * 1000 * 32), video);
	fclose(video);

	// Used to move the pointer location of the video.
	uint32_t pointer_location = 0x00;

	// Pre-render our text glyphs and populate their 'metrics'.
	SDL_Color text_color = {0x80, 0x80, 0x80, 0xFF};
	Glyphs glyphs[96];

	for (int i = 32; i < 128; i++) {
		glyphs[i-32].surface = TTF_RenderGlyph_Solid(font, i, text_color);
		TTF_GlyphMetrics(font, i, NULL, NULL, NULL, NULL, &glyphs[i-32].advance);
	}
	// ===

	Timer fps = {SDL_GetTicks(), 0};

	// Now with audio!
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_Init(MIX_INIT_OGG);

	Mix_Music *audio = Mix_LoadMUS(music);

	Mix_VolumeMusic(MIX_MAX_VOLUME);
	Mix_PlayMusic(audio, 1);

	while (1) {
		// Clear the screen
		SDL_FillRect(sdl->windowSurface, NULL, 0);

		file_to_surface(sdl, video_mem, &pointer_location, &glyphs);
		PrintFPS(sdl, font);

		if ((SDL_GetTicks() - fps.lastTime) < (1000 / FRAME_PER_SECOND)) {
			SDL_Delay(1000 / FRAME_PER_SECOND - (SDL_GetTicks() - fps.lastTime));
		}

		fps.lastTime = SDL_GetTicks();

		SDL_UpdateWindowSurface(sdl->window);
		Frame++;

		while (SDL_PollEvent(&sdl->event)) {
			if (sdl->event.type == SDL_QUIT) {
				goto the_end;
			}
		}

	}

the_end:
	return 0;
}

void file_to_surface(SDL *sdl, char *video, uint32_t *loc, Glyphs *glyphs)
{
	char *buffer = malloc(200);

	for (uint8_t y = 0; y < 0x3C; y++) {
		memcpy(buffer, (video + *loc), 0x9F);
		*loc += 0xA2;
#ifdef NXDK
		// Don't waste time drawing lines we can't even see, we lose about 20 lines on Xbox with 720x480.
		if (y > 40)
			continue;
#endif
		for (uint8_t x = 0; x < 0x9F; x++) {
			SDL_Rect location = {x * glyphs[(uint8_t)buffer[x]-32].advance, y * glyphs[(uint8_t)buffer[x]-32].surface->h, 0, 0};
			SDL_BlitSurface(glyphs[(uint8_t)buffer[x]-32].surface, NULL, sdl->windowSurface, &location);
		}
	}

	free(buffer);

	// Every 60 lines there's a blank line, we need to skip it.
	*loc += 0x02;
}

void PrintFPS(SDL *sdl, TTF_Font *font)
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
		sdl->fpsCount = TTF_RenderText_Solid(font, fpsch, color);
		Frame = 0;
	}

	if (sdl->fpsCount == NULL) {
		sdl->fpsCount = TTF_RenderText_Solid(font, "FPS:0", color);
	}
	SDL_Rect location = {WIDTH - sdl->fpsCount->w, 0, 0, 0};
	SDL_BlitSurface(sdl->fpsCount, NULL, sdl->windowSurface, &location);
}
