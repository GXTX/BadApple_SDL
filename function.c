/*
This function lib edit by KosWu.
KosWu's Blog: blog.koswu.com
*/
/*头文件声明*/
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include "common.h"
#include "error.h"
/*全局变量*/
SDL_Window  *Window;
SDL_Surface *Screen = NULL; //屏幕表面
SDL_Surface *FpsCount;
SDL_Event Event; //事件类型
SDL_Color Fontcolor;
TTF_Font *Font;
SDL_Surface *Point;
Timer Fps;
Timer Updatefps;
int Frame = 0;
TTF_Font *FPSFont;

/*文件变量*/

/*函数定义*/
char *itoa(int num, char *str, int radix);
void Init(void);
void Error(int errorcode);
SDL_Surface *Print_Text(char *text);
void CopyToSurface(int x, int y, SDL_Surface *source, SDL_Surface *target, SDL_Rect *cli);
void SetTextColor(unsigned short r, unsigned short g, unsigned short b);
void Flip_Screen(void);
void Set_ScreenColor(unsigned short r, unsigned short g, unsigned short b);
void PrintFPS(void);
void Clean_Up(void);

/*错误退出函数*/
void Error(int errorcode)
{
	if (errorcode < 0x10)
	{
		Clean_Up();
		exit(errorcode);
	}
	SetTextColor(0xFF, 0, 0);
	SDL_Surface *error;
	char errormessage[50] = "Program has an error!QAQ";
	char errorchar[10];
	error = Print_Text(errormessage);
	SDL_FillRect(Screen, &(Screen->clip_rect), SDL_MapRGB(Screen->format, 0xFF, 0xFF, 0xFF));
	CopyToSurface((WIDTH - error->w) / 2, (HEIGHT - error->h) / 2 - error->h - 20, error, Screen, NULL);
	SDL_FreeSurface(error);
	strcpy(errormessage, "Program will exit after 10s");
	error = Print_Text(errormessage);
	CopyToSurface((WIDTH - error->w) / 2, (HEIGHT - error->h) / 2, error, Screen, NULL);
	SDL_FreeSurface(error);
	strcpy(errormessage, "Error Code：0x");
	itoa(errorcode, errorchar, 16);
	strcat(errormessage, errorchar);
	error = Print_Text(errormessage);
	CopyToSurface((WIDTH - error->w) / 2, (HEIGHT - error->h) / 2 + error->h + 20, error, Screen, NULL);
	SDL_FreeSurface(error);
	if (SDL_UpdateWindowSurface(Window) < 0)
	{
		Error(ERROR_FLIPSCREEN);
	}
	SDL_Delay(10000);
	SDL_Quit();
	exit(errorcode);
}

/*初始化*/
void Init(void)
{
	/*窗口位置居中*/
	putenv("SDL_VIDEO_WINDOW_POS");
	putenv("SDL_VIDEO_CENTERED=1");
	/*初始化SDL所有子系统*/
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		Error(ERROR_INIT_SDL);
	}
	/*初始化字体系统*/
	if (TTF_Init() < 0)
	{
		Error(ERROR_INIT_FONTLIB);
	}
	/*初始化声音系统*/
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
	{
		Error(ERROR_INIT_MIXLIB);
	}
	if (Mix_Init(MIX_INIT_MP3) < 0)
	{
		Error(ERROR_INIT_MIXLIB);
	}
	/*初始化屏幕表面*/
	Window = SDL_CreateWindow("Bad Apple!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	Screen = SDL_GetWindowSurface(Window);
	if (Screen == NULL)
	{
		exit(ERROR_INIT_SDL);
	}
	/*启动计时器*/
	Fps.last_time = SDL_GetTicks();
}

/*渲染文字*/
SDL_Surface *Print_Text(char *text)
{
	SDL_Surface *textface;
	textface = TTF_RenderUTF8_Solid(Font, text, Fontcolor);
	if (textface == NULL)
	{
		Error(ERROR_PRINTTEXT);
	}
	return textface;
}

/*设置字体颜色*/
void SetTextColor(unsigned short r, unsigned short g, unsigned short b)
{
	Fontcolor.r = r;
	Fontcolor.g = g;
	Fontcolor.b = b;
}

/*拷贝至表面*/
void CopyToSurface(int x, int y, SDL_Surface *source, SDL_Surface *target, SDL_Rect *cli)
{
	SDL_Rect location;
	location.x = x;
	location.y = y;
	SDL_BlitSurface(source, cli, target, &location);
}

/*加载音乐*/
Mix_Music *Load_Music(char *path)
{
	Mix_Music *music = NULL;
	music = Mix_LoadMUS(path);
	if (music == NULL)
	{
		Error(ERROR_LOADMUSIC);
	}
	return music;
}

/*刷新屏幕*/
void Flip_Screen(void)
{
	if ((SDL_GetTicks() - Fps.last_time) < (1000 / FRAME_PER_SECOND))
	{
		SDL_Delay(1000 / FRAME_PER_SECOND - (SDL_GetTicks() - Fps.last_time));
	}
	Fps.last_time = SDL_GetTicks();
	if (SDL_UpdateWindowSurface(Window) < 0)
	{
		Error(ERROR_FLIPSCREEN);
	}
	Frame++;
}

void Set_ScreenColor(unsigned short r, unsigned short g, unsigned short b) //设置背景色
{
	SDL_FillRect(Screen, &(Screen->clip_rect), SDL_MapRGB(Screen->format, r, g, b));
}

/*打印FPS帧率*/
void PrintFPS(void)
{
	char fpsch[10] = "FPS:";
	char temp[4];
	int temptime;
	if (Updatefps.last_time == 0)
	{
		Updatefps.last_time = SDL_GetTicks();
		return;
	}
	temptime = SDL_GetTicks();
	Updatefps.time += temptime - Updatefps.last_time;
	Updatefps.last_time = temptime;
	if (Updatefps.time >= 1000)
	{
		Updatefps.time = 0;
		itoa(Frame, temp, 10);
		strcat(fpsch, temp);
		FpsCount = TTF_RenderText_Solid(FPSFont, fpsch, Fontcolor);
		if (FpsCount == NULL)
		{
			Error(ERROR_PRINTTEXT);
		}
		Frame = 0;
	}
	if (FpsCount == NULL)
	{
		FpsCount = TTF_RenderText_Solid(FPSFont, "FPS:0", Fontcolor);
		if (FpsCount == NULL)
		{
			Error(ERROR_PRINTTEXT);
		}
	}
	CopyToSurface(WIDTH - FpsCount->w, 0, FpsCount, Screen, NULL);
}

/*底层函数*/
char *itoa(int num, char *str, int radix)
{
	//num：int型原数,str:需转换成的string，radix,原进制，
	/* 索引表 */
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum;
	/* 中间变量 */
	int i = 0, j, k;
	/* 确定unum的值 */
	if (radix == 10 && num < 0)
	{
		/* 十进制负数 */
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else
		unum = (unsigned)num; /* 其他情况 */ /* 逆序 */
	do
	{
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0'; /* 转换 */
	if (str[0] == '-')
		k = 1; /* 十进制负数 */
	else
		k = 0; /* 将原来的“/2”改为“/2.0”，保证当num在16~255之间，radix等于16时，也能得到正确结果 */
	for (j = k; j < (i - 1) / 2.0 + k; j++)
	{
		num = str[j];
		str[j] = str[i - j - 1 + k];
		str[i - j - 1 + k] = num;
	}
	return str;
}
