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
#define VIEWBOTTOM 178
#define METHODWIDTH 120
#define BTNHEIGHT ((MSGBOXTOP-VIEWBOTTOM) / 5)
#define BTNSTART (FILESWIDTH+8+(METHODWIDTH*3))

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
#define newbtn  8
#define savebtn 9
#define rndbtn  10
#define gobtn   11
#define quitbtn 12
#define current 13

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
    static SWIN *wnewbtn;
    static SWIN *wsavebtn;
    static SWIN *wrndbtn;
    static SWIN *wgobtn;
    static SWIN *wquitbtn;
    static int currenti;
    static char **map;

    if (!(initialised))
    {
        wtitle=NewWin(1,1,639,TITLEHEIGHT);
        wfiles=NewWin(1,TITLEHEIGHT+2,FILESWIDTH,MSGBOXTOP-2);
        wfview=NewWin(FILESWIDTH+2,TITLEHEIGHT+2,VIEWXPOS,VIEWBOTTOM);
        wmsgbox=NewWin(1,MSGBOXTOP,BTNSTART-2,349);
        whelp=NewWin(352,TITLEHEIGHT+2,639,VIEWBOTTOM);
        wsolver=NewWin(FILESWIDTH+2,VIEWBOTTOM+2,FILESWIDTH+2+METHODWIDTH,MSGBOXTOP-2);
        wmethod=NewWin(FILESWIDTH+4+METHODWIDTH,VIEWBOTTOM+2,FILESWIDTH+4+(METHODWIDTH*2),MSGBOXTOP-2);
        winit=NewWin(FILESWIDTH+6+(METHODWIDTH*2),VIEWBOTTOM+2,FILESWIDTH+6+(METHODWIDTH*3),MSGBOXTOP-2);
        wnewbtn=NewWin(BTNSTART,VIEWBOTTOM+2,639,VIEWBOTTOM+BTNHEIGHT);
        wsavebtn=NewWin(BTNSTART,VIEWBOTTOM+2+(BTNHEIGHT),639,VIEWBOTTOM+(BTNHEIGHT*2));
        wrndbtn=NewWin(BTNSTART,VIEWBOTTOM+2+(BTNHEIGHT*2),639,VIEWBOTTOM+(BTNHEIGHT*3));
        wgobtn=NewWin(BTNSTART,VIEWBOTTOM+2+(BTNHEIGHT*3),639,VIEWBOTTOM+(BTNHEIGHT*5));
        wquitbtn=NewWin(BTNSTART,MSGBOXTOP,639,349);
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
        BorderWin(wnewbtn,border_col);
        BorderWin(wsavebtn,border_col);
        BorderWin(wrndbtn,border_col);
        BorderWin(wgobtn,border_col);
        BorderWin(wquitbtn,border_col);
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
                    case newbtn : SetTextWin(wnewbtn); wcurrent=wnewbtn; break;
                    case savebtn : SetTextWin(wsavebtn); wcurrent=wsavebtn; break;
                    case rndbtn : SetTextWin(wrndbtn); wcurrent=wrndbtn; break;
                    case gobtn : SetTextWin(wgobtn); wcurrent=wgobtn; break;
                    case quitbtn : SetTextWin(wquitbtn); wcurrent=wquitbtn; break;
                    default     : SetWin(wcurrent); break;                    
                } break;
            case CURRENT :
                return currenti; break;
            case WHERE :
                if       (y>MSGBOXTOP)
                    if (x>BTNSTART)
                        currenti=quitbtn;
                    else
                        currenti=msgbox;
                else if (y<TITLEHEIGHT) currenti=title;
                else if (x<FILESWIDTH) currenti=files;
                else if (y<VIEWBOTTOM) // * help or view * //
                {
                    if (x>VIEWXPOS)
                        currenti=help;
                    else
                        currenti=fview;
                }
                else
                {
                    if (x>FILESWIDTH+6+(METHODWIDTH*3)) // * is a button * //
                    {
                        if (y>VIEWBOTTOM+2+(BTNHEIGHT*3))
                            currenti=gobtn;
                        else if (y>VIEWBOTTOM+2+(BTNHEIGHT*2))
                            currenti=rndbtn;
                        else if (y>VIEWBOTTOM+2+(BTNHEIGHT))
                            currenti=savebtn;
                        else
                            currenti=newbtn;
                    }
                    else if (x>FILESWIDTH+4+(METHODWIDTH*2))
                        currenti=init;
                    else if (x>FILESWIDTH+2+METHODWIDTH)
                        currenti=method;
                    else
                        currenti=solver;
                }
                return currenti; 
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
    char **messages=allocate_N_by_N_array(14,50,sizeof(char),"Messages");
    strcpy(messages[0],"Title Bar                                   ");
    strcpy(messages[1],"Choose A File                               ");
    strcpy(messages[2],"View of File                                ");
    strcpy(messages[3],"Choose a TSP method                         ");
    strcpy(messages[4],"This is the message box                     ");
    strcpy(messages[5],"Help on this program                        ");
    strcpy(messages[6],"Choose an 'TSP method' method               ");
    strcpy(messages[7],"Choose an 'TSP method' initialisation method");
    strcpy(messages[8],"Creates a new random map                    ");
    strcpy(messages[9],"Saves the current map                       ");
    strcpy(messages[10],"Randomizes the map                         ");
    strcpy(messages[11],"Start with the current settings            ");    
    strcpy(messages[12],"Quit the program                           ");

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
    
    WinManager(SET,newbtn,0,0);
    DrawText("New",4,2,GREEN);

    WinManager(SET,savebtn,0,0);
    DrawText("Save",4,2,GREEN);
    
    WinManager(SET,rndbtn,0,0);
    DrawText("Randomize",1,3,GREEN);

    WinManager(SET,quitbtn,0,0);
    DrawText("Quit",4,2,GREEN);

    WinManager(SET,gobtn,0,0);
    DrawText("GO!",4,2,GREEN);

    _setvisualpage(0);
    SetMousePage(0);
    ResetMouse();
    SetMousePos(0,0);
    ShowMouse();

    UseMouseCur(CROSS);
    do
    {
        MouseStat(&m);
        if (ButtonDown(&m))
        {
            if (WinManager(CURRENT,NULL,0,0)==quitbtn)
            {
                ShutDown();
                exit(1);
            
            }
        }
        if (MouseMoved())
        {
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
