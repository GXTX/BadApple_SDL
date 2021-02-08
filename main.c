#include "main.h"

/*主函数*/
int main(void)
{
	SDL *sdl = malloc(sizeof(SDL));
	TTF *ttf = malloc(sizeof(TTF));
	MIX *mix = malloc(sizeof(MIX));

	Timer *fps;
	FILE *video;

#ifdef NXDK
	XVideoSetMode(WIDTH, HEIGHT, BPP, REFRESH_DEFAULT);
#endif

	init_backends(sdl, ttf, mix, fps);

	init_files(ttf, mix, &video);

	while (1) {
		// Clear the screen
		SDL_FillRect(sdl->Surface, &(sdl->Surface->clip_rect), 
			SDL_MapRGB(sdl->Surface->format, 0x3F, 0x3F, 0x3F));

		file_to_surface(sdl, ttf->Font, &video);

		PrintFPS(sdl, ttf->Font);
		//PrintFPS();
		update_screen(sdl, fps);

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

void init_backends(SDL *sdl, TTF *ttf, MIX *mix, Timer *fps)
{
	if (SDL_Init((SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) < 0) {
		printf("%s\n", SDL_GetError());
		//goto the_end;
	}

	sdl->Window = SDL_CreateWindow("Bad Apple!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	sdl->Surface = SDL_GetWindowSurface(sdl->Window);
	if (sdl->Surface == NULL) {
		printf("%s\n", SDL_GetError());
		//goto the_end;
	}

	if (TTF_Init() < 0) {
		printf("%s\n", TTF_GetError());
		//goto the_end;
	}

	fps->last_time = SDL_GetTicks();
}

void init_files(TTF *ttf, MIX *mix, FILE **video)
{
	static const char *font  = "resource/consola.ttf";
	static const char *music = "resource/Badapple.mp3";
	static const char *video_file = "resource/AscPic.txt";

	ttf->Font = TTF_OpenFont(font, 9);
	ttf->FpsFont = TTF_OpenFont(font, 20);

	*video = fopen(video_file, "r");
	if (*video == NULL) {
		printf("Unable to open file: 0x%X\n", errno);
		//goto the_end;
	}

	//bgm = Load_Music("resource/Badapple.mp3");
}

void update_screen(SDL *sdl, Timer *fps)
{
	if ((SDL_GetTicks() - fps->last_time) < (1000 / FRAME_PER_SECOND)) {
		SDL_Delay(1000 / FRAME_PER_SECOND - (SDL_GetTicks() - fps->last_time));
	}

	fps->last_time = SDL_GetTicks();

	if (SDL_UpdateWindowSurface(sdl->Window) < 0) {
		printf("%s\n", SDL_GetError());
		//goto the_end;
	}

	Frame++;
}

void file_to_surface(SDL *sdl, TTF_Font *font, FILE **video)
{
	int count = 0;
	int temp;

	char buffer[200] = "ddd";

	while (count < 60) {
		if (fgets(buffer, 199, *video) == NULL) {
			SDL_Delay(5000);
			//goto the_end;
		}

		buffer[160] = '\0';

		sdl->video = TTF_RenderText_Solid(font, buffer, sdl->FontColor);
		if (sdl->video == NULL){
			printf("%s\n", SDL_GetError());
			//goto the_end;
		}

		CopyToSurface(0, count * sdl->video->h, sdl->video, sdl->Surface, NULL);
		SDL_FreeSurface(sdl->video);

		count++;
	}

    // Why?
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
	//location.x = x;
	//location.y = y;
	SDL_BlitSurface(source, cli, target, &location);
}

void PrintFPS(SDL *sdl, TTF_Font *font)
{
	char fpsch[10] = "FPS:";
	char temp[4];
	int temptime;

	if (Updatefps.last_time == 0) {
		Updatefps.last_time = SDL_GetTicks();
		return;
	}

	temptime = SDL_GetTicks();
	Updatefps.time += temptime - Updatefps.last_time;
	Updatefps.last_time = temptime;

	if (Updatefps.time >= 1000) {
		Updatefps.time = 0;
		itoa(Frame, temp, 10);
		strcat(fpsch, temp);
		sdl->FpsCount = TTF_RenderText_Solid(font, fpsch, sdl->FontColor);
		if (sdl->FpsCount == NULL) {
			//Error(ERROR_PRINTTEXT);
		}
		Frame = 0;
	}

	if (sdl->FpsCount == NULL) {
		sdl->FpsCount = TTF_RenderText_Solid(font, "FPS:0", sdl->FontColor);
		if (sdl->FpsCount == NULL) {
			//Error(ERROR_PRINTTEXT);
		}
	}

	CopyToSurface(WIDTH - sdl->FpsCount->w, 0, sdl->FpsCount, sdl->Surface, NULL);
}

char *itoa(int num, char *str, int radix)
{
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint32_t unum;

	int i = 0, j, k;

	if (radix == 10 && num < 0) {
		unum = (uint32_t)-num;
		str[i++] = '-';
	} else {
		unum = (uint32_t)num;
	}

	do {
		str[i++] = index[unum % (uint32_t)radix];
		unum /= radix;
	} while (unum);

	str[i] = '\0';

	if (str[0] == '-') {
		k = 1;
	} else {
		k = 0;
	}

	for (j = k; j < (i - 1) / 2.0 + k; j++) {
		num = str[j];
		str[j] = str[i - j - 1 + k];
		str[i - j - 1 + k] = num;
	}

	return str;
}
