#include <stdbool.h>
#include <stdlib.h>
#ifdef NXDK
#define WIDTH 720
#define HEIGHT 480
#else
#define WIDTH 800
#define HEIGHT 640
#endif
#define BPP 32
#define FRAME_PER_SECOND 30

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
