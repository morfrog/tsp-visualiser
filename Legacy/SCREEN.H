#ifndef SCREEN_H
#define maxx 100
#define maxy 100
#define citysize 0.02

#if defined ( __386__ )
    #define _FAR
    #define _HUGE
#else
    #define _FAR    __far
    #define _HUGE   __huge
#endif

/* color presets */
#define BLACK 0
#define BLUE 1
#define BLACK2 2
#define BLUE2 3
#define RED 4
#define PURPLE 5
#define ORANGE 6
#define WHITE 7
#define GREY 8
#define LTBLUE 9
#define GREEN 10
#define CYAN 11
#define LTRED 12
#define PINK 13
#define YELLOW 14
#define LTWHITE 15


/* colours */
#define normal_col  RED
#define line_change BLUE
#define city_hi     YELLOW
#define city_in     CYAN
#define city_out    GREY
#define text_col    LTRED
#define line_col    GREEN
#define border_col  PURPLE

#define Text 0    // WinManager
#define Main 1    // WinManager

struct win
{
    int t; /* top */
    int l; /* left */
    int b; /* bottom */
    int r; /* right */
};

typedef struct win SWIN;

int _FAR InitScreen(int videomode);
int _FAR ShutDown( void );
SWIN _FAR *NewWin(int t,int l,int b,int r);
void _FAR SetWin(SWIN *w);
void _FAR SetTextWin(SWIN *w,int border);
void _FAR ResetWin(SWIN *w,int t,int l,int b,int r);
void _FAR FreeWin(SWIN *w);
void _FAR BorderWin();
void _FAR DrawLine(double x1,double y1,double x2,double y2,int color);
void _FAR DrawCity(char name[],double x,double y,double size,int color);
void _FAR DrawText(char string[],int start,int line,int color);
void _FAR TextWindow(int rows,int cols);
int _FAR SwapPage();
void _FAR WinManager(int which);
void _FAR ShowText(char string[],int start,int line,int color);
#endif
