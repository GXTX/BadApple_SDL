#include "main.h"

int main(void)
{
	SDL *sdl = malloc(sizeof(SDL));

#ifdef NXDK
	XVideoSetMode(WIDTH, HEIGHT, 16, REFRESH_DEFAULT);
#endif

	if (SDL_Init((SDL_INIT_VIDEO | SDL_INIT_EVENTS)) < 0) {
		//printf("%s\n", SDL_GetError());
		goto the_end;
	}

	sdl->window = SDL_CreateWindow("Bad Apple!!", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	sdl->windowSurface = SDL_GetWindowSurface(sdl->window);

	if (sdl->windowSurface == NULL) {
		//printf("%s\n", SDL_GetError());
		goto the_end;
	}

	if (TTF_Init() < 0) {
		//printf("%s\n", TTF_GetError());
		goto the_end;
	}

	TTF_Font *font = TTF_OpenFont(videoFont, 9);

	FILE *video = fopen(videoFile, "r");
	if (video == NULL) {
		//printf("Unable to open file: 0x%X\n", errno);
		goto the_end;
	}

	char *video_mem = malloc((1024 * 1000 * 8));
	fread(video_mem, 1, (1024 * 1000 * 8), video);
	fclose(video);

	uint32_t pointer_location = 0x00;

	Timer fps = {SDL_GetTicks(), 0};

	// Now with audio!
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_Init(MIX_INIT_OGG);

	Mix_Music *audio = Mix_LoadMUS(music);

	Mix_VolumeMusic(MIX_MAX_VOLUME);
	Mix_PlayMusic(audio, 1);

	while (1) {
		// Clear the screen
		SDL_FillRect(sdl->windowSurface, &(sdl->windowSurface->clip_rect), 0);

		file_to_surface(sdl, font, video_mem, &pointer_location);
		PrintFPS(sdl, font);

		update_screen(sdl, &fps);

		while (SDL_PollEvent(&sdl->event)) {
			if (sdl->event.type == SDL_QUIT) {
				goto the_end;
			}
		}

	}

the_end:
	//Clean_Up();
	return 0;
}

void update_screen(SDL *sdl, Timer *fps)
{
	if ((SDL_GetTicks() - fps->lastTime) < (1000 / FRAME_PER_SECOND)) {
		SDL_Delay(1000 / FRAME_PER_SECOND - (SDL_GetTicks() - fps->lastTime));
	}

	fps->lastTime = SDL_GetTicks();

	SDL_UpdateWindowSurface(sdl->window);

	Frame++;
}

void CopyToSurface(int x, int y, SDL_Surface *source, SDL_Surface *target, SDL_Rect *cli)
{
	SDL_Rect location = {x, y, 0, 0};
	SDL_BlitSurface(source, cli, target, &location);
}

void file_to_surface(SDL *sdl, TTF_Font *font, char *video, uint32_t *loc)
{
	int count = 0;
	char *buffer = malloc(200);

	SDL_Color color = {0x80, 0x80, 0x80, 0xFF};

	while (count < 60) {
		memcpy(buffer, (video + *loc), 0x9F);
		*loc += 0xA2;

		*(buffer+160) = '\0';

		sdl->video = TTF_RenderText_Solid(font, buffer, color);
		if (sdl->video != NULL) {
			CopyToSurface(0, count * sdl->video->h, sdl->video, sdl->windowSurface, NULL);
		}
		SDL_FreeSurface(sdl->video);

		count++;
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
		if (sdl->fpsCount == NULL) {
			//Error(ERROR_PRINTTEXT);
		}
		Frame = 0;
	}

	if (sdl->fpsCount == NULL) {
		sdl->fpsCount = TTF_RenderText_Solid(font, "FPS:0", color);
		if (sdl->fpsCount == NULL) {
			//Error(ERROR_PRINTTEXT);
		}
	}
	CopyToSurface(WIDTH - sdl->fpsCount->w, 0, sdl->fpsCount, sdl->windowSurface, NULL);
}
