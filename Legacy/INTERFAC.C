#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <graph.h>
#include <direct.h>
#include "sys.h"
#include "screen.h"
#include "mouse.h"

#define TITLEHEIGHT 15
#define MSGBOXTOP 320
#define FILESWIDTH 120
#define VIEWXPOS 350 
#define VIEWBOTTOM 180
#define METHODWIDTH 120

#define CURRENT 0
#define SET     1
#define WHERE   2

#define title   0 
#define files   1   
#define fview   2
#define solver  3
#define msgbox  4
#define help    5
#define method  6
#define init    7
#define savebtn 8
#define quitbtn 9
#define gobtn   10
#define helpbtn 11
#define current 12

int ScrAsMouseX(int m)
{
    return 37683200+m;
}

int SameExtension(char a[],char b[])
{
    return ((a[0]==b[0]) &&
             (a[1]==b[1]) &&
             (a[2]==b[2]));
}

char *Extension(char *filename)
// * returns 3 char extension if exists, else returns NULL * // 
{
    char *buffer=salloc(4,sizeof(char));
    int x,y,len=strlen(filename);
    buffer="   \0";
    
    for (x=0;x<len;x++) {
        if (filename[x]=='.') break;
    }
    if (x==len) return NULL;
    ++x;
    for (y=x;y<len;y++)
        buffer[y-x]=filename[y];
    return buffer;
}

char **GetDirectory(char *dir,char *extension,int *numfiles)
{
    DIR *dirp;
    char **ret;
    struct dirent *direntp;
    int count=0,pos=0;

    dirp = opendir( dir );
    if( dirp != NULL ) {
      for(;;) {
        direntp = readdir( dirp );
        if( direntp == NULL ) break;
        if ( SameExtension( extension,Extension(direntp->d_name)) )
            ++count;
      }
      closedir( dirp );
    }

    ret=allocate_N_by_N_array(count,13,sizeof(char),"Directory");

    dirp = opendir( dir );
    if( dirp != NULL ) {
      for(;;) {
        direntp = readdir( dirp );
        if( direntp == NULL ) {
            printf("Assertion Failure: GetDirectory\n"); exit(0);
        }
        if ( SameExtension( extension,Extension(direntp->d_name)) )
        {
            ret[pos]=strcpy(ret[pos],direntp->d_name);
            ++pos;
        }
        if (pos==count) break;
      }
      closedir( dirp );
    }

    *numfiles=count;
    return (char **)ret;

}

void DrawList(char **list,int size,int chosen)
{
    int x;
    for (x=0;x<size;x++)
        if (x!=chosen)
            DrawText(list[x],1,x+1,LTRED);
        else
            DrawText(list[x],1,x+1,YELLOW);
}

int WinManager(int mode,int win,int x,int y)
// * mode in {CURRENT,SET,WHERE}
// * SET sets the current window, CURRENT returns the current window,
// * WHERE returns which window is at x,y - NULL if isn't one.
// * WinManager(SET,text1,0,0);
// * current=WinManager(CURRENT,NULL,0,0);
// * mousewin=WinManager(WHERE,NULL,23,56);
{
    static int initialised=0;
    static SWIN *wtitle;
    static SWIN *wfiles;
    static SWIN *wfview;
    static SWIN *wsolver;
    static SWIN *wmsgbox;
    static SWIN *whelp;
    static SWIN *wmethod;
    static SWIN *winit;
    static SWIN *wcurrent;
    static int currenti;
    static char **map;

    if (!(initialised))
    {
        wtitle=NewWin(1,1,639,TITLEHEIGHT);
        wfiles=NewWin(1,TITLEHEIGHT+2,FILESWIDTH,MSGBOXTOP-2);
        wfview=NewWin(FILESWIDTH+2,TITLEHEIGHT+2,VIEWXPOS,VIEWBOTTOM);
        wmsgbox=NewWin(1,MSGBOXTOP,639,349);
        whelp=NewWin(352,TITLEHEIGHT+2,639,VIEWBOTTOM);
        wsolver=NewWin(FILESWIDTH+2,VIEWBOTTOM+2,FILESWIDTH+2+METHODWIDTH,MSGBOXTOP-2);
        wmethod=NewWin(FILESWIDTH+4+METHODWIDTH,VIEWBOTTOM+2,FILESWIDTH+4+(METHODWIDTH*2),MSGBOXTOP-2);
        winit=NewWin(FILESWIDTH+6+(METHODWIDTH*2),VIEWBOTTOM+2,FILESWIDTH+6+(METHODWIDTH*3),MSGBOXTOP-2);
        wcurrent=title;
        initialised=1;
        SetMousePos(wcurrent->t+1,wcurrent->l+1);
        BorderWin(wfiles,border_col);
        BorderWin(wfview,border_col);
        BorderWin(wmsgbox,border_col);
        BorderWin(wsolver,border_col);
        BorderWin(whelp,border_col);
        BorderWin(wmethod,border_col);
        BorderWin(winit,border_col);    
    }
    else
    {
        switch (mode) {
            case SET :
                currenti = win;
                switch (win) {
                    case title  : SetTextWin(wtitle); wcurrent=wtitle; break;
                    case files  : SetTextWin(wfiles); wcurrent=wfiles; break;
                    case fview  : SetWin(wfview); wcurrent=wfview;break;
                    case solver : SetTextWin(wsolver); wcurrent=wsolver; break;
                    case msgbox : SetTextWin(wmsgbox); wcurrent=wmsgbox; break;
                    case help   : SetTextWin(whelp);   wcurrent=whelp; break;
                    case method : SetTextWin(wmethod); wcurrent=wmethod; break;
                    case init   : SetTextWin(winit);   wcurrent=winit; break;
                    default     : SetWin(wcurrent); break;                    
                } break;
            case CURRENT :
                return currenti; break;
            case WHERE :
                if       (y>MSGBOXTOP) return msgbox;
                else if (y<TITLEHEIGHT) return title;
                else if (x<FILESWIDTH) return files;
                else if (y<VIEWBOTTOM) // * help or view * //
                {
                    if (x>VIEWXPOS)
                        return help;
                    else
                        return fview;
                }
                else
                {
                    if (x>FILESWIDTH+6+(METHODWIDTH*3))
                        return quitbtn;
                    else if (x>FILESWIDTH+4+(METHODWIDTH*2))
                        return init;
                    else if (x>FILESWIDTH+2+METHODWIDTH)
                        return method;
                    else
                        return solver;
                } 
                break;
        }
    }
    return -1; // * should never be made use of * //
}

void Msg(char *string,int color)
{
    WinManager(SET,msgbox,0,0);
    DrawText(string,1,2,color);
}

void main()
{
    int pos=320;
    MouseStatus m;
    char buffer[40];
    char **directory;
    char **tmp;
    int size,x,filenum=0,solvernum=0,at=title,last=title;
    char **messages=allocate_N_by_N_array(12,50,sizeof(char),"Messages");
    strcpy(messages[0],"Title Bar                                   ");
    strcpy(messages[1],"Choose A File                               ");
    strcpy(messages[2],"View of File                                ");
    strcpy(messages[3],"Choose a TSP method                         ");
    strcpy(messages[4],"This is the message box                     ");
    strcpy(messages[5],"Help on this program                        ");
    strcpy(messages[6],"Choose an 'TSP method' method               ");
    strcpy(messages[7],"Choose an 'TSP method' initialisation method");
    strcpy(messages[8],"Saves the current map                       ");
    strcpy(messages[9],"Press to quit the program                   ");
    strcpy(messages[10],"Run the chosen methods                     ");
    strcpy(messages[11],"Press for extended help                    ");    
    
    directory=GetDirectory( getcwd( NULL, 0 ),"TSP",&size );
    printf("%d files found\n",size);
    
    InitScreen(16);
    _setactivepage(0);
    _setvisualpage(1);
    
    
    WinManager(SET,title,0,0);
    DrawText("Visualisation Tool For The Travelling Salesman Problem - "
             "Drew 1997",
            1,1,GREEN);

    WinManager(SET,files,0,0);
    DrawList(directory,size,filenum);
    
    WinManager(SET,solver,0,0);
    tmp=allocate_N_by_N_array(3,13,sizeof(char),"Directory");
    strcpy(tmp[0],"Insertion");
    strcpy(tmp[1],"Optimisation");
    strcpy(tmp[2],"Annealing");
    DrawList(tmp,3,solvernum);
    
    Msg("This is the message box!",GREEN);
    
    _setvisualpage(0);
    SetMousePage(0);
    ResetMouse();
    SetMousePos(0,0);
    ShowMouse();

    UseMouseCur(CROSS);
    do
    {
        if (MouseMoved())
        {
            MouseStat(&m);
            at=WinManager(WHERE,NULL,m.xpos,m.ypos);
            if (at!=last)
            {
                last=at;
                if (at==fview) 
                {
                    sprintf(buffer,"View of %s                         ",
                        directory[filenum]);
                    Msg(buffer,GREEN);
                }
                else if (at==init)
                {
                    sprintf(buffer,"%s Initialisation Method           ",
                        tmp[solvernum]);
                    Msg(buffer,GREEN);
                }
                else if (at==method)
                {
                   sprintf(buffer,"%s Operation Method           ",
                        tmp[solvernum]);
                    Msg(buffer,GREEN);
                }
                else
                    Msg(messages[at],GREEN);
            }
        }
    }
    while (!(kbhit()));
    getch();
}

/*
     (MouseStat(&m));
        if (MouseMoved())
        {
            if ((current==files) ) // && (!(MouseAtWin()==files)))
            {
                strcpy(buffer,"Select A Filename            ");
                SetTextWin(msgbox);
                DrawText(buffer,1,2,GREEN);
                current=files;
            }
            else
            {
                strcpy(buffer,"Whatever                     ");
                SetTextWin(msgbox);
                DrawText(buffer,1,2,GREEN);
                current=title;
            }
        }

        */
