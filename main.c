/*头文件声明*/
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "common.h"
#include "error.h"

/*内部类型声明*/
void Load_File (void);
/*外部函数声明*/
extern void Init (void);//初始化SDL
extern SDL_Surface * Load_Image(const char path[]);//优化加载图片
extern void CopyToSurface(int x,int y,SDL_Surface * source,SDL_Surface * target,SDL_Rect * cli);//复制表面
extern void Error (int errorcode);//错误报告
extern SDL_Surface *Print_Text(char * text);//渲染文字
extern void SetTextColor (unsigned short r,unsigned short g,unsigned short b);//设置字体颜色
extern Mix_Music* Load_Music (const char * path);//加载音乐
extern void GetMouseLocation (void);//加载鼠标位置
extern Mix_Chunk *Load_Chunk (const char * path);//加载音效
extern void PrintMouse(void);//打印鼠标位置
extern void Flip_Screen(void);//刷新屏幕
extern void Set_ScreenColor (unsigned short r,unsigned short g,unsigned short b);//设置背景色
extern void PrintFPS (void);//打印FPS

/*内部函数声明*/
void Clean_Up(void);
void Load_File (void);
void Print_Ascii(void);

/*全局变量声明*/
extern SDL_Surface* Screen;//屏幕表面
extern SDL_Surface* Point;
extern SDL_Event Event;//事件类型
extern Location Mouse;
extern SDL_Color Fontcolor;
extern TTF_Font * Font;
extern Timer Fps;
extern int Frame;
extern TTF_Font * FPSFont;

/*文件变量声明*/
FILE * AscPic;
char Ascch[200]="ddd";
SDL_Surface *AscFace;
Mix_Music* bgm;
TTF_Font * Default_Font;

/*主函数*/
int main (int argv,char* args[])
{
	/*变量*/
	bool quit=false;
	//SDL_Surface *message=NULL;
	/*加载环境*/
	Init();
	/*加载背景颜色*/
	Set_ScreenColor(0x3F,0x3F,0x3F);
	SetTextColor (0xFF,0xFF,0xFF);
	/*加载文件*/
	Load_File();
	/*刷新屏幕*/
	Print_Ascii();
	Set_ScreenColor(0x3F,0x3F,0x3F);
	Print_Ascii();
	Flip_Screen();
	/*播放音乐*/
	if (Mix_PlayMusic(bgm,1)<0)
	{
		return 1;
	}
	/*保持屏幕存在*/
	while (quit==false)
	{
		Set_ScreenColor(0x3F,0x3F,0x3F);
		Print_Ascii();
		PrintFPS();
		//GetMouseLocation();
		//PrintMouse();
		Flip_Screen();
		play:while (SDL_PollEvent(&Event))
		{
			if (Event.type==SDL_KEYDOWN&&Event.key.keysym.sym == SDLK_SPACE)
			{
				if (Mix_PausedMusic()!=1)
				{
					Mix_PauseMusic();
					Set_ScreenColor(0x3F,0x3F,0x3F);
                    Flip_Screen();
					while (quit==false)
					{
						while (SDL_PollEvent(&Event))
						{
							if (Event.type==SDL_KEYDOWN&&Event.key.keysym.sym == SDLK_SPACE)
							{
								Mix_ResumeMusic();
								goto play;
							}
							if (Event.type==SDL_QUIT)
							{
							    quit=true;
                            }
						}
						if (Mix_PausedMusic()!=1)
						{
							break;
						}
					}
				}
			}
			if (Event.type==SDL_QUIT)
			{
				quit=true;
			}
		}
	}
	/*清理*/
	Clean_Up();
	return 0;
}
void Load_File (void)
{
	Font=TTF_OpenFont("consola.ttf",9);
	FPSFont=TTF_OpenFont("consola.ttf",20);
	AscPic=fopen("AscPic.txt","r");
	bgm=Load_Music("Badapple.mp3");
	if (AscPic==NULL)
	{
		Error(0x9F);
	}
}
void Clean_Up(void)
{
	fclose (AscPic);
	Mix_FreeMusic(bgm);
	SDL_FreeSurface (Point);
	SDL_FreeSurface (AscFace);
	TTF_CloseFont(Font);
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
	TTF_Quit();
}
void Print_Ascii(void)
{
	int count=0,temp;
	while (count<60){
	if (fgets (Ascch,199,AscPic)==NULL)
	{
		SDL_Delay(5000);
		Clean_Up();
		exit (0);
	}
	Ascch[160]='\0';
	AscFace=TTF_RenderText_Solid(Font,Ascch,Fontcolor);
	if (AscFace==NULL)
	{
		Error (ERROR_PRINTTEXT);
	}
	CopyToSurface(40,count*AscFace->h,AscFace,Screen,NULL);
	SDL_FreeSurface(AscFace);
	count++;
	}
	temp=fgetc (AscPic);
	temp=fgetc (AscPic);
	if (temp==EOF)
	{
		SDL_Delay(5000);
		Clean_Up();
		exit (0);
	}
}
