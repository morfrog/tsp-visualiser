#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <graph.h>
#include <direct.h>
#include "sys.h"
#include "file.h"
#include "screen.h"
#include "path.h"
#include "problem.h"
#include "insert.h"
#include "optim.h"
#include "anneal.h"
#include "mouse.h"

#define TITLEHEIGHT 15
#define MSGBOXTOP 320
#define FILESWIDTH 120
#define VIEWXPOS 350 
#define VIEWBOTTOM 178
#define METHODWIDTH 120
#define BTNHEIGHT ((MSGBOXTOP-VIEWBOTTOM) / 5)
#define BTNSTART (FILESWIDTH+8+(METHODWIDTH*3))
#define INPUTWIDTH 200
#define INPUTHEIGHT 150


#define CURRENT 0
#define SET     1
#define WHERE   2
#define BORDER  3
#define CLEAR   4

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

static int number_of_files;
static int number_of_solvers=3;
static int number_of_methods=8;
static int number_of_inits=3;
static int current_solver=0;

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

void DrawList(char *list[],int size,int chosen)
{
    int x;
    for (x=0;x<size;x++)
        if (x!=chosen)
            DrawText(list[x],1,x+1,LTRED);
        else
            DrawText(list[x],1,x+1,YELLOW);
}

int IntWinManager(int mode,int win,int x,int y)
// * mode in {CURRENT,SET,WHERE,BORDER,CLEAR}
// * SET sets the current window, CURRENT returns the current window,
// * WHERE returns which window is at x,y - NULL if isn't one.
// * IntWinManager(SET,text1,0,0);
// * current=IntWinManager(CURRENT,NULL,0,0);
// * mousewin=IntWinManager(WHERE,NULL,23,56);
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
    SWIN *tmp;
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
            case BORDER : case CLEAR :
                switch (win) {
                    case title   : tmp=wtitle;  break;
                    case files   : tmp=wfiles;  break; 
                    case fview   : tmp=wfview;  break;
                    case solver  : tmp=wsolver; break; 
                    case msgbox  : tmp=wmsgbox; break; 
                    case help    : tmp=whelp;   break;
                    case method  : tmp=wmethod; break; 
                    case init    : tmp=winit;   break;
                    case newbtn  : tmp=wnewbtn; break; 
                    case savebtn : tmp=wsavebtn;break;  
                    case rndbtn  : tmp=wrndbtn; break; 
                    case gobtn   : tmp=wgobtn;  break;
                    case quitbtn : tmp=wquitbtn;break;  
                    default      : tmp=wtitle;break;  
                }
                if (mode==BORDER) BorderWin(tmp,x);
                else
                {
                    ClearSection(tmp->l+1,tmp->t+1,tmp->r-1,tmp->b-1);
                }

                break;
            
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
                if (y>MSGBOXTOP)
                    if (x>BTNSTART)     currenti=quitbtn;
                    else                currenti=msgbox;
                else if (y<TITLEHEIGHT) currenti=title;
                else if (x<FILESWIDTH)  currenti=files;
                else if (y<VIEWBOTTOM) { // * help or view * // 
                    if (x>VIEWXPOS)     currenti=help;
                    else                currenti=fview;
                }
                else {
                    if (x>FILESWIDTH+6+(METHODWIDTH*3)) // * is a button * //
                    {
                        if (y>VIEWBOTTOM+2+(BTNHEIGHT*3))      currenti=gobtn;
                        else if (y>VIEWBOTTOM+2+(BTNHEIGHT*2)) currenti=rndbtn;
                        else if (y>VIEWBOTTOM+2+(BTNHEIGHT))   currenti=savebtn;
                        else                                   currenti=newbtn;
                    }
                    else if (x>FILESWIDTH+4+(METHODWIDTH*2))   currenti=init;
                    else if (x>FILESWIDTH+2+METHODWIDTH)       currenti=method;
                    else                                       currenti=solver;
                }
                return currenti; 
                break;
        }
    }
    return -1; // * should never be made use of * //
}

void Msg(char *string,int color)
{
    HideMouse();
    IntWinManager(SET,msgbox,0,0);
    DrawText(string,1,2,color);
    ShowMouse();
}

char *InputBox(char *msg)
{
    static int initialised=0;
    static int isize;
    static char *image;
    static char *tmp;
    static SWIN *inputbox;
    
    if (!(initialised))
    {
        inputbox=NewWin(201,101,200+INPUTWIDTH,100+INPUTHEIGHT);
        isize=_imagesize(0,0,INPUTWIDTH,INPUTHEIGHT);
        image=salloc(isize,sizeof(char));
        tmp=salloc(isize,sizeof(char));        
        SetWin(inputbox);
        _clearscreen(_GVIEWPORT);
        BorderWin(inputbox,border_col);
        _rectangle(250,100+INPUTHEIGHT-50,300,100+INPUTHEIGHT-10,_GBORDER);
        SetTextWin(inputbox);
        DrawText("OK",10,5,GREEN);    
        _getimage(201,101,200+INPUTWIDTH,100+INPUTHEIGHT,image);
        
        initialised=1;
    }
    else
    {
        _getimage(201,101,200+INPUTWIDTH,100+INPUTHEIGHT,tmp);
        // * save what was there * //
        _putimage(201,101,image,_GPSET);
        SetTextWin(inputbox);
        DrawText(msg,2,2,GREEN);
        getch();
        _putimage(201,101,tmp,_GPSET);
    }
    return "hi";
}

void UpdateFileView(PROBLEM *currentprob)
{
    IntWinManager(CLEAR,fview,0,0);
    IntWinManager(SET,fview,0,0);
    DrawCities(currentprob->dist->l,currentprob->vmask);
    DrawPath(currentprob->path,currentprob->dist->l,1);
}

char *SetFile(PROBLEM **currentprob,int num,int override)
// * if num <> current sets current=num and refreshes screen, override auto
// * refreshes, func returns the name of the current file
{
    static int filenum=0;
    static int numfiles;
    static char **directory;
    static int initialised=0;
    int update=0;
    
    if (!(initialised)) {
        directory=GetDirectory( getcwd( NULL, 0 ),"TSP",&numfiles );
        printf("%d files found\n",numfiles);
        
        initialised=1;
        *currentprob=NewProblem(NewMap(NewLocates(directory[filenum])));
        (*currentprob)->path=LocatesAsPath((*currentprob)->dist->l);
        number_of_files=numfiles;
    }
    if (num<0) return directory[filenum];
    if (num!=filenum)
    {
        filenum=num;
        update=1;
        *currentprob=NewProblem(NewMap(NewLocates(directory[filenum])));
        (*currentprob)->path=LocatesAsPath((*currentprob)->dist->l);
    }
    if ((update) || (override))
    {
        HideMouse();
        IntWinManager(SET,files,0,0);
        DrawList(directory,numfiles,filenum);
        UpdateFileView(*currentprob);
        ShowMouse();
    }
    return directory[filenum];
}

char *SetInit(int num,int override)
// * sets the current init num to num, and refreshes if changed.
// * refreshes automatically if override true.
// * -ve num returns string for current method
{
    static int initialised=0;
    static char **insinit;
    static char **anninit;
    static int insinitnum=0;
    static int anninitnum=0;
    int changed=0;

    if (!(initialised))
    {
        insinit=allocate_N_by_N_array(3,13,sizeof(char),"InsInit");
        strcpy(insinit[0],"Random     ");
        strcpy(insinit[1],"Closest    ");
        strcpy(insinit[2],"Furthest   ");

        anninit=allocate_N_by_N_array(3,13,sizeof(char),"AnnInit");
        strcpy(anninit[0],"Temp =     ");
        strcpy(anninit[1],"Cool =     ");
        strcpy(anninit[2],"AtTmp=     ");

        initialised=1;
    }
    if (num<0) // * request for current * //
    {
        switch (current_solver) {
            case 0 : return insinit[insinitnum];        break;
            case 1 : return NULL; break;
            case 2 : return anninit[anninitnum];  break;
        }
    }
    if (!(override))
    {
        switch (current_solver) {
            case 0 : if (insinitnum!=num) {
                        changed=1; insinitnum=num;
                     } break;
            case 1 : break;
            case 2 : if (anninitnum!=num) {
                         changed=1; anninitnum=num;
                     } break;
        }
    }
    if ((changed) || (override)) // * update display
    {
        if (override) IntWinManager(CLEAR,init,0,0);
        SetGlobals();
        HideMouse();
        IntWinManager(SET,init,0,0);
        switch (current_solver) {
            case 0 : DrawList(insinit,3,insinitnum); break;
            case 1 : break;
            case 2 : DrawList(anninit,3,anninitnum); break;
        }
        ShowMouse();
    }
}

char *SetMethod(int num,int override)
// * sets the current method num to num, and refreshes if changed.
// * refreshes automatically if override true.
// * -ve num returns string for current method
{
    static int initialised=0;
    static char **insmethod;
    static char **optimmethod;
    static char **annealmethod;
    static int insmethodnum=0;
    static int optimmethodnum=0;
    static int annealmethodnum=0;
    int changed=0;

    if (!(initialised))
    {
        insmethod=allocate_N_by_N_array(8,13,sizeof(char),"InsMethod");
        strcpy(insmethod[0],"N Neighbour");
        strcpy(insmethod[1],"Random     ");
        strcpy(insmethod[2],"Cheapest   ");
        strcpy(insmethod[3],"Dearest    ");
        strcpy(insmethod[4],"Nearest    ");
        strcpy(insmethod[5],"Furthest   ");
        strcpy(insmethod[6],"Nearest II ");
        strcpy(insmethod[7],"Furthest II");

        optimmethod=allocate_N_by_N_array(3,13,sizeof(char),"OptimMethod");
        strcpy(optimmethod[0],"1 opt      ");
        strcpy(optimmethod[1],"2 opt      ");
        strcpy(optimmethod[2],"3 opt      ");

        annealmethod=allocate_N_by_N_array(1,13,sizeof(char),"AnnealMethod");
        strcpy(annealmethod[0],"1 vs 2 opt ");

        insmethodnum=0;
        optimmethodnum=0;
        annealmethodnum=0;
        
        initialised=1;
    }
    if (num<0) // * request for current * //
    {
        switch (current_solver) {
            case 0 : return insmethod[insmethodnum];        break;
            case 1 : return optimmethod[optimmethodnum];    break;
            case 2 : return annealmethod[annealmethodnum];  break;
        }
    }

    if (!(override))
    {
        switch (current_solver) {
            case 0 : if (insmethodnum!=num) {
                        changed=1; insmethodnum=num;
                     } break;
            case 1 : if (optimmethodnum!=num) {
                        changed=1; optimmethodnum=num;
                     } break;
            case 2 : if (annealmethodnum!=num) {
                        changed=1; annealmethodnum=num;
                     } break;
        }
    }
    if ((changed) || (override)) // * update display
    {
        if (override) IntWinManager(CLEAR,method,0,0);
        SetGlobals();
        HideMouse();
        IntWinManager(SET,method,0,0);
        switch (current_solver) {
            case 0 : DrawList(insmethod,8,insmethodnum);
                     if (insmethodnum==0)
                     {
                        IntWinManager(CLEAR,init,0,0);
                        number_of_inits=0;
                     }
                     else SetInit(0,1);
                     break;
            case 1 : DrawList(optimmethod,3,optimmethodnum);
                     SetInit(0,1);
                     break;
            case 2 : DrawList(annealmethod,1,annealmethodnum);
                     SetInit(0,1);
                     break;
        }
        ShowMouse();
        
    }
}

void SetGlobals()
{
    switch (current_solver) {
        case 0 : number_of_methods=8;
                 number_of_inits=3;
                 break;
        case 1 : number_of_methods=3;
                 number_of_inits=0;
                 break;
        case 2 : number_of_methods=1;
                 number_of_inits=3;
    }
}

char *SetSolver(int num,int override)
// * if num <> current sets current=num and refreshes screen, override auto
// * refreshes, func returns the name of the current solver
{
    static int initialised=0;
    static char **solvermethod;
    int update;

    if (!(initialised))
    {
        solvermethod=allocate_N_by_N_array(3,13,sizeof(char),"SolverMethod");
        strcpy(solvermethod[0],"Insertion");
        strcpy(solvermethod[1],"Optimisation");
        strcpy(solvermethod[2],"Annealing");
        number_of_solvers=3;
        current_solver=0;
        SetGlobals();
        
        initialised=1;
    }
    if (num<0) return solvermethod[current_solver];
    if ((num!=current_solver) && (!(override))) 
    {
        current_solver=num;
        SetGlobals();
        update=1;
    }
    if ((update) || (override))
    {
        HideMouse();
        IntWinManager(SET,solver,0,0);
        DrawList(solvermethod,number_of_solvers,current_solver);
        ShowMouse();
        SetMethod(0,1);
    }
    return solvermethod[current_solver];
}

void main()
{
    int pos=320,RequestUpdate=0,line;
    MouseStatus m;
    char buffer[60];
    int x,select,at=title,last=title,mx=0,my=0;

    PROBLEM *currentprob;

//    char **messages=allocate_N_by_N_array(14,50,sizeof(char),"Messages");

    char messages[13][45]={"Title Bar                                   ",
                           "Choose A File                               ",
                           "View of File                                ",
                           "Choose a TSP method                         ",
                           "This is the message box                     ",
                           "Help on this program                        ",
                           "Choose an 'TSP method' method               ",
                           "Choose an 'TSP method' initialisation method",
                           "Creates a new random map                    ",
                           "Saves the current map                       ",
                           "Randomizes the map                          ",
                           "Start with the current settings             ",
                           "Quit the program                            "};

    InitScreen(16);
    _setactivepage(0);
    _setvisualpage(1);

    IntWinManager(SET,title,0,0);
    DrawText("Visualisation Tool For The Travelling Salesman Problem - "
             "Drew 1997",
            1,1,GREEN);

    SetFile(&currentprob,0,1);
    SetSolver(0,1);

    Msg("This is the message box!",GREEN);
    
    IntWinManager(SET,newbtn,0,0);
    DrawText("New",7,2,GREEN);

    IntWinManager(SET,savebtn,0,0);
    DrawText("Save",7,2,GREEN);
    
    IntWinManager(SET,rndbtn,0,0);
    DrawText("Randomize",4,3,GREEN);

    IntWinManager(SET,quitbtn,0,0);
    DrawText("Quit",7,2,GREEN);

    IntWinManager(SET,gobtn,0,0);
    DrawText("GO!",7,2,GREEN);

    _setvisualpage(0);
    SetMousePage(0);
    ResetMouse();
    SetMousePos(0,0);
    ShowMouse();

    UseMouseCur(GLOVE);
    do
    {
        MouseStat(&m);
        if (ButtonDown(&m))
        {
            switch (IntWinManager(CURRENT,NULL,0,0)) {
            case quitbtn :
                ShutDown();
                exit(1);
            case files :
                line=MapPixToText(m.ypos,0)-2;
                if (line<0) select=-1;
                else if (line>=number_of_files) select=-1;
                else select=line;

                if (select>=0) SetFile(&currentprob,select,0);
           
                break;
            case solver :
                line = MapPixToText(m.ypos-VIEWBOTTOM,0);
                if (line<0) select=-1;
                else if (line>=3) select=-1;
                else select=line;
                if (select>=0) SetSolver(select,0);
                break;
             case method :
                line = MapPixToText(m.ypos-VIEWBOTTOM,0);
                if (line<0) select=-1;
                else if (line>=number_of_methods) select=-1;
                else select=line;
                if (select>=0) SetMethod(select,0);
                break;
            case init :
                line = MapPixToText(m.ypos-VIEWBOTTOM,0);
                if (line<0) select=-1;
                else if (line>=number_of_inits) select=-1;
                else select=line;
                if (select>=0) SetInit(select,0);
                break;
            }
        }
        if (MouseMoved() || (RequestUpdate) || (mx!=m.xpos) || (my!=m.ypos))
        {
            at=IntWinManager(WHERE,NULL,m.xpos,m.ypos);
            mx=m.xpos;my=m.ypos;
            if (at!=last)
            {
                last=at;
                if (at==fview) 
                {
                    sprintf(buffer,"View of %s                         ",
                        SetFile(&currentprob,-1,0));
                    Msg(buffer,GREEN);
                }
                else if (at==init)
                {
                    sprintf(buffer,"Choose an %s Initialisation Method           ",
                        SetSolver(-1,0));
                    Msg(buffer,GREEN);
                }
                else if (at==method)
                {
                   sprintf(buffer,"Choose an %s Operation Method           ",
                        SetSolver(-1,0));
                    Msg(buffer,GREEN);
                }
                else
                    Msg(messages[at],GREEN);
            }
        }
    }
    while (!(kbhit()));
    ShutDown();
    getch();
}

