#include <stdbool.h>
#include <stdlib.h>
#ifndef WIDTH
  #define WIDTH 920
  #define HEIGHT 720
  #define BPP 32
  #define FRAME_PER_SECOND 30
#endif
typedef struct Location
{
	int x;
	int y;
} Location;
typedef struct Timer
{
	int last_time;
	int time;
	bool running;
} Timer;
