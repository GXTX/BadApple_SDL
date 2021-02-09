#include "main.h"

int main(void)
{
	SDL *sdl = malloc(sizeof(SDL));
	//MIX *mix = malloc(sizeof(MIX));

	Timer fps = {0, 0};
	FILE *video;

#ifdef NXDK
	XVideoSetMode(WIDTH, HEIGHT, 16, REFRESH_DEFAULT);
#endif

	if (SDL_Init((SDL_INIT_VIDEO | SDL_INIT_EVENTS)) < 0) {
		//printf("%s\n", SDL_GetError());
		goto the_end;
	}

	sdl->Window = SDL_CreateWindow("Bad Apple!!", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	sdl->Surface = SDL_GetWindowSurface(sdl->Window);

	if (sdl->Surface == NULL) {
		//printf("%s\n", SDL_GetError());
		goto the_end;
	}

	if (TTF_Init() < 0) {
		//printf("%s\n", TTF_GetError());
		goto the_end;
	}

	fps.last_time = SDL_GetTicks();

	TTF_Font *font = TTF_OpenFont(video_font, 9);

	video = fopen(video_file, "r");
	if (video == NULL) {
		//printf("Unable to open file: 0x%X\n", errno);
		goto the_end;
	}

	while (1) {
		// Clear the screen
		SDL_FillRect(sdl->Surface, &(sdl->Surface->clip_rect), 0);

		file_to_surface(sdl, font, &video);
		PrintFPS(sdl, font);

		update_screen(sdl, &fps);

		while (SDL_PollEvent(&sdl->Event)) {
			if (sdl->Event.type == SDL_QUIT) {
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
	if ((SDL_GetTicks() - fps->last_time) < (1000 / FRAME_PER_SECOND)) {
		SDL_Delay(1000 / FRAME_PER_SECOND - (SDL_GetTicks() - fps->last_time));
	}

	fps->last_time = SDL_GetTicks();

	if (SDL_UpdateWindowSurface(sdl->Window) < 0) {
		//printf("%s\n", SDL_GetError());
		//goto the_end;
	}

	Frame++;
}

void file_to_surface(SDL *sdl, TTF_Font *font, FILE **video)
{
	int count = 0;
	char buffer[200] = "ddd";

	SDL_Color color = {0x80, 0x80, 0x80, 0xFF};

	while (count < 60) {
		if (fgets(buffer, 199, *video) == NULL) {
			SDL_Delay(5000);
			//goto the_end;
		}

		buffer[160] = '\0';

		sdl->video = TTF_RenderText_Solid(font, buffer, color);
		if (sdl->video == NULL){
			//printf("%s\n", SDL_GetError());
			//goto the_end;
		}

		CopyToSurface(0, count * sdl->video->h, sdl->video, sdl->Surface, NULL);
		SDL_FreeSurface(sdl->video);

		count++;
	}

	// To keep sync with the base video we need to read 2 characters every "frame"
	// otherwise there is a rolling shutter effect.
	int temp;
	temp = fgetc(*video);
	temp = fgetc(*video);
	if (temp == EOF) {
		SDL_Delay(5000);
		//goto the_end;
	}
}

void CopyToSurface(int x, int y, SDL_Surface *source, SDL_Surface *target, SDL_Rect *cli)
{
	SDL_Rect location = {x, y, 0, 0};
	SDL_BlitSurface(source, cli, target, &location);
}

void PrintFPS(SDL *sdl, TTF_Font *font)
{
	char fpsch[10] = "FPS:";
	int temptime;

	SDL_Color color = {0xFF, 0x00, 0x00, 0xFF};

	if (Updatefps.last_time == 0) {
		Updatefps.last_time = SDL_GetTicks();
		return;
	}

	temptime = SDL_GetTicks();
	Updatefps.time += temptime - Updatefps.last_time;
	Updatefps.last_time = temptime;

	if (Updatefps.time >= 1000) {
		Updatefps.time = 0;

		sprintf(fpsch, "FPS: %i", Frame);
		sdl->FpsCount = TTF_RenderText_Solid(font, fpsch, color);
		if (sdl->FpsCount == NULL) {
			//Error(ERROR_PRINTTEXT);
		}
		Frame = 0;
	}

	if (sdl->FpsCount == NULL) {
		sdl->FpsCount = TTF_RenderText_Solid(font, "FPS:0", color);
		if (sdl->FpsCount == NULL) {
			//Error(ERROR_PRINTTEXT);
		}
	}

	CopyToSurface(WIDTH - sdl->FpsCount->w, 0, sdl->FpsCount, sdl->Surface, NULL);
}
