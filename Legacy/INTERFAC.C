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
#include "makemap.h"
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

#define INITIALISE -1
#define REQUEST -2

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

struct choice
{
    char *name;
    char *helptitle;
    char *helptext;
};
#define fldname  0
#define fldtitle 1
#define fldhelp  2

typedef struct choice CHOICE;

static int number_of_files;
static int number_of_solvers=3;
static int number_of_methods=8;
static int number_of_inits=3;
static int current_solver=0;
static double temperature=100.0;
static double coolingrate=0.8;
static int changesattemp=50;

int ScrAsMouseX(int m)
{
    return 37683200+m;
}

int SameExtension(char *a,char *b)
{
    if ((a==NULL) && (b==NULL)) return 0;
    else return ((a[0]==b[0]) &&
                 (a[1]==b[1]) &&
                 (a[2]==b[2]));
}

char *Extension(char *filename)
// * returns 3 char extension
{
    char *buffer=salloc(4,sizeof(char));
    int x,y,len=strlen(filename);

    strcpy(buffer,"   \0");
    
    for (x=0;x<len;x++) {
        if (filename[x]=='.') break;
    }
    ++x;
//    if (x==len) return "   ";

    for (y=x;y<len;y++)
        buffer[y-x]=filename[y];
    return buffer;
}

char **GetDirectory(char *dir,char *extension,int *numfiles,int extras)
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

    ret=allocate_N_by_N_array(count+extras,13,sizeof(char),"Directory");

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

void ReadChoiceHelp(char *filename,CHOICE *c)
{
    FILE *in;

    chdir("hlp");
    if ((in=fopen(filename,"rt"))==NULL)
    {
        strcpy(c->helptitle,filename);
        strcpy(c->helptext,"Is An Invalid File\n"
                           "Ensure it is in the HLP\n"
                           "subdirectory");
    }
    else
    {
        c->helptitle[ReadLines(in,1,c->helptitle)]='\0';
        c->helptext[ReadLines(in,9,c->helptext)]='\0';
        fclose(in);
    }
    chdir("..");
}

void DrawChoiceNames(CHOICE *list,int size,int chosen)
{
    int x;
    for (x=0;x<size;x++)
        if (x!=chosen)
            DrawText(list[x].name,1,x+1,LTRED);
        else
            DrawText(list[x].name,1,x+1,YELLOW);
}

void DrawChoiceHelp(CHOICE c)
{
    DrawText(c.helptitle,1,1,YELLOW);
    DrawText(c.helptext,1,2,LTRED);
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
        SetMousePos(wcurrent->t+1,wcurrent->l+1);
        initialised=1;
    }
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
            else ClearSection(tmp->l+1,tmp->t+1,tmp->r-1,tmp->b-1);

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
                }
                break;
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
    return -1; // * should never be made use of * //
}

void Msg(char *string,int color)
{
    HideMouse();
    IntWinManager(CLEAR,msgbox,0,0);
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
    
    if ((num==INITIALISE) || (!initialised)) {
        directory=GetDirectory( getcwd( NULL, 0 ),"TSP",&numfiles,1 );

        if ((!initialised) && (numfiles>0)) {
            *currentprob=NewProblem(NewMap(NewLocates(directory[filenum])));
            (*currentprob)->path=LocatesAsPath((*currentprob)->dist->l);
        }
        strcpy(directory[numfiles],"Current");
        WriteLocates(PathAsLocates((*currentprob)->path,(*currentprob)->dist->l),"CURRENT");
        ++numfiles;

        number_of_files=numfiles;
        initialised=1;

    }
    if (num==REQUEST) return directory[filenum];
    if ((num!=filenum) && (num!=INITIALISE))
    {
        filenum=num;
        update=1;
        if (numfiles>0)
        {
            FreeMap(& ((*currentprob)->dist) );
            *currentprob=NewProblem(NewMap(NewLocates(directory[filenum])));
            (*currentprob)->path=LocatesAsPath((*currentprob)->dist->l);
        }
    }
    if ((update) || (override))
    {
        HideMouse();
        IntWinManager(CLEAR,files,0,0);
        IntWinManager(SET,files,0,0);
        if (numfiles>0)
        {
            DrawList(directory,numfiles,filenum);
            UpdateFileView(*currentprob);
        }
        else
            DrawText(" No Files\n"
                     "Found. Use\n"
                     "  New to\n"
                     "create one",1,1,YELLOW);
        ShowMouse();
    }
    return directory[filenum];
}

CHOICE *NewChoiceArray(int size)
{
    int x;
    CHOICE *ret=salloc(size,sizeof(CHOICE));

    for (x=0;x<size;x++)
    {
        ret[x].name=salloc(13,sizeof(char));
        ret[x].helptitle=salloc(40,sizeof(char));
        ret[x].helptext=salloc(360,sizeof(char));
    }
    return ret;
}

char *SetInit(int num,int override,int showhelp,int *inum)
// * sets the current init num to num, and refreshes if changed.
// * refreshes automatically if override true.
// * -ve num returns string for current method
{
    static int initialised=0;
    static CHOICE *insinit;
    static CHOICE *anninit;
    static int insinitnum=0;
    static int anninitnum=0;
    int changed=0;
    int buffer[10];

    if (!(initialised))
    {
        insinit=NewChoiceArray(3);
        strcpy(insinit[0].name,"Random");
        ReadChoiceHelp("irandom.hlp",&insinit[0]);
        strcpy(insinit[1].name,"Closest");
        ReadChoiceHelp("iclose.hlp",&insinit[1]);
        strcpy(insinit[2].name,"Furthest");
        ReadChoiceHelp("ifurth.hlp",&insinit[2]);


        anninit=NewChoiceArray(3);
        ReadChoiceHelp("itemp.hlp",&anninit[0]);
        ReadChoiceHelp("icool.hlp",&anninit[1]);
        ReadChoiceHelp("iattmp.hlp",&anninit[2]);

    }
    if ((num==INITIALISE) || (!initialised))
    {
        initialised=1;
        sprintf(buffer,"Temp =%6.2f",temperature);
        strcpy(anninit[0].name,buffer);
        sprintf(buffer,"Cool =%6.2f",coolingrate);
        strcpy(anninit[1].name,buffer);
        sprintf(buffer,"AtTmp=%6d",changesattemp);
        strcpy(anninit[2].name,buffer);
    }

    if (num==REQUEST) // * request for current * //
    {
        switch (current_solver) {
            case 0 : *inum=insinitnum; return insinit[insinitnum].name;        break;
            case 1 : *inum=-1; return NULL; break;
            case 2 : *inum=anninitnum; return anninit[anninitnum].name;  break;
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
            case 0 : DrawChoiceNames(insinit,3,insinitnum); break;
            case 1 : break;
            case 2 : DrawChoiceNames(anninit,3,anninitnum); break;
        }
        ShowMouse();
    }
    if ((showhelp) && (!changed))
    {
        IntWinManager(CLEAR,help,0,0);
        IntWinManager(SET,help,0,0);
        switch (current_solver) {
            case 0 : DrawChoiceHelp(insinit[insinitnum]); break;
            case 1 : break;
            case 2 : DrawChoiceHelp(anninit[anninitnum]); break;
        }
    }

}

char *SetMethod(int num,int override,int showhelp,int *mnum)
// * sets the current method num to num, and refreshes if changed.
// * refreshes automatically if override true.
// * -ve num returns string for current method
{
    static int initialised=0;
    static CHOICE *insmethod;
    static CHOICE *iteratemethod;
    static CHOICE *annealmethod;
    static int insmethodnum=0;
    static int iteratemethodnum=0;
    static int annealmethodnum=0;
    int changed=0;

    if (!(initialised))
    {
        insmethod=NewChoiceArray(8);
        strcpy(insmethod[0].name,"N Neighbour");
        ReadChoiceHelp("mnearn.hlp",&insmethod[0]);
        strcpy(insmethod[1].name,"Random     ");
        ReadChoiceHelp("mrandom.hlp",&insmethod[1]);
        strcpy(insmethod[2].name,"Cheapest   ");
        ReadChoiceHelp("mcheap.hlp",&insmethod[2]);
        strcpy(insmethod[3].name,"Dearest    ");
        ReadChoiceHelp("mdear.hlp",&insmethod[3]);
        strcpy(insmethod[4].name,"Nearest    ");
        ReadChoiceHelp("mnear.hlp",&insmethod[4]);
        strcpy(insmethod[5].name,"Furthest   ");
        ReadChoiceHelp("mfurth.hlp",&insmethod[5]);
        strcpy(insmethod[6].name,"Nearest II ");
        ReadChoiceHelp("mnear2.hlp",&insmethod[6]);
        strcpy(insmethod[7].name,"Furthest II");
        ReadChoiceHelp("mfurth2.hlp",&insmethod[7]);

        iteratemethod=NewChoiceArray(3);
        strcpy(iteratemethod[0].name,"1 opt      ");
        ReadChoiceHelp("1opt.hlp",&iteratemethod[0]);
        strcpy(iteratemethod[1].name,"2 opt      ");
        ReadChoiceHelp("2opt.hlp",&iteratemethod[1]);
        strcpy(iteratemethod[2].name,"3 opt      ");
        ReadChoiceHelp("3opt.hlp",&iteratemethod[2]);

        annealmethod=NewChoiceArray(1);
        strcpy(annealmethod[0].name,"1 vs 2 opt");
        ReadChoiceHelp("1vs2opt.hlp",&annealmethod[0]);

        insmethodnum=0;
        iteratemethodnum=0;
        annealmethodnum=0;
        
        initialised=1;
    }
    if (num==REQUEST) // * request for current * //
    {
        switch (current_solver) {
            case 0 : *mnum=insmethodnum; return insmethod[insmethodnum].name;         break;
            case 1 : *mnum=iteratemethodnum; return iteratemethod[iteratemethodnum].name;   break;
            case 2 : *mnum=annealmethodnum; return annealmethod[annealmethodnum].name;break;
        }
    }

    if (!(override))
    {
        switch (current_solver) {
            case 0 : if (insmethodnum!=num) {
                        changed=1; insmethodnum=num;
                     } break;
            case 1 : if (iteratemethodnum!=num) {
                        changed=1; iteratemethodnum=num;
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
            case 0 : DrawChoiceNames(insmethod,8,insmethodnum);
                     if (insmethodnum==0)
                     {
                        IntWinManager(CLEAR,init,0,0);
                        number_of_inits=0;
                     }
                     else SetInit(0,TRUE,FALSE,NULL);
                     break;
            case 1 : DrawChoiceNames(iteratemethod,3,iteratemethodnum);
                     SetInit(0,TRUE,FALSE,NULL);
                     break;
            case 2 : DrawChoiceNames(annealmethod,1,annealmethodnum);
                     SetInit(0,TRUE,FALSE,NULL);
                     break;
        }
        ShowMouse();
    }
    if (showhelp)
    {
        IntWinManager(CLEAR,help,0,0);
        IntWinManager(SET,help,0,0);
        switch (current_solver) {
            case 0 : DrawChoiceHelp(insmethod[insmethodnum]); break;
            case 1 : DrawChoiceHelp(iteratemethod[iteratemethodnum]); break;
            case 2 : DrawChoiceHelp(annealmethod[annealmethodnum]); break;
        }
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

char *SetSolver(int num,int override,int showhelp)
// * if num <> current sets current=num and refreshes screen, override auto
// * refreshes, func returns the name of the current solver
{
    static int initialised=0;
    static CHOICE *solvermethod;
    int update;

    if (!(initialised))
    {
        solvermethod=NewChoiceArray(3);
        strcpy(solvermethod[0].name,"Insertion");
        ReadChoiceHelp("sinsert.hlp",&solvermethod[0]);
        strcpy(solvermethod[1].name,"Iteration");
        ReadChoiceHelp("siterate.hlp",&solvermethod[1]);
        strcpy(solvermethod[2].name,"Annealing");
        ReadChoiceHelp("sanneal.hlp",&solvermethod[2]);

        number_of_solvers=3;
        current_solver=0;
        SetGlobals();
        
        initialised=1;
    }
    if (num==REQUEST) return solvermethod[current_solver].name;
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
        DrawChoiceNames(solvermethod,number_of_solvers,current_solver);
        ShowMouse();
        SetMethod(0,TRUE,FALSE,NULL);
    }
    if (showhelp)
    {
        IntWinManager(CLEAR,help,0,0);
        IntWinManager(SET,help,0,0);
        DrawChoiceHelp(solvermethod[current_solver]);
    }
    return solvermethod[current_solver].name;
}

void DrawScreen(PROBLEM **currentprob)
{
    IntWinManager(BORDER,files,border_col,0);
    IntWinManager(BORDER,fview,border_col,0);
    IntWinManager(BORDER,msgbox,border_col,0);
    IntWinManager(BORDER,solver,border_col,0);
    IntWinManager(BORDER,help,border_col,0);
    IntWinManager(BORDER,method,border_col,0);
    IntWinManager(BORDER,init,border_col,0);
    IntWinManager(BORDER,newbtn,border_col,0);
    IntWinManager(BORDER,savebtn,border_col,0);
    IntWinManager(BORDER,rndbtn,border_col,0);
    IntWinManager(BORDER,gobtn,border_col,0);
    IntWinManager(BORDER,quitbtn,border_col,0);

    IntWinManager(SET,title,0,0);
    DrawText("Visualisation Tool For The Travelling Salesman Problem",1,1,GREEN);

    IntWinManager(SET,help,0,0);
    DrawText("      VTTSP V1.01 1997\n"
             "\n"
             "       By Drew\n"
             "       \n"
             "       Semester II 1997\n"
             "               for\n"
             "         PROJECT\n"
             "\n"
             "       PROJECT COORDINATOR\n"
             "         Raymond Lister\n",1,1,GREEN);

    SetFile(currentprob,INITIALISE,TRUE);
    SetSolver(0,TRUE,FALSE);

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

}

void main()
{
    int pos=320,line;
    MouseStatus m;
    char buffer[60];
    int x,select,at=title,last=title,mx=0,my=0,tinit,tmethod,currentwin=title;
    INSPROB *insprob;
    PROBLEM *currentprob=NULL;
    FILE *out;
    double length;

//    char **messages=allocate_N_by_N_array(14,50,sizeof(char),"Messages");

    char messages[13][60]={"",
                           "Choose A File",
                           "View of File",
                           "Choose a TSP method",
                           "This is the message box",
                           "Help on this program",
                           "Choose an 'TSP method' method",
                           "Choose an 'TSP method' initialisation method",
                           "Creates a new random map",
                           "Saves the current map",
                           "Randomizes the map",
                           "Start with the current settings",
                           "Quit the program"};

    InitScreen(16);
    _setactivepage(0);
    _setvisualpage(1);

    DrawScreen(&currentprob);

    _setvisualpage(0);
    SetMousePage(0);
    ResetMouse();
    SetMousePos(300,200);
    ShowMouse();

    UseMouseCur(GLOVE);
    do
    {
        MouseStat(&m);
        if (MouseMoved()) 
        {
            at=IntWinManager(WHERE,NULL,m.xpos,m.ypos);
            mx=m.xpos;my=m.ypos;
            if (at!=last)
            {
                last=at;
                if (at==fview) 
                {
                    sprintf(buffer,"View of %s                         ",
                        SetFile(&currentprob,REQUEST,FALSE));
                    Msg(buffer,GREEN);
                }
                else if (at==init)
                {
                    sprintf(buffer,"Choose an %s Initialisation Method           ",
                        SetSolver(REQUEST,FALSE,FALSE));
                    Msg(buffer,GREEN);
                }
                else if (at==method)
                {
                   sprintf(buffer,"Choose an %s Operation Method           ",
                        SetSolver(REQUEST,FALSE,FALSE));
                    Msg(buffer,GREEN);
                }
                else
                    Msg(messages[at],GREEN);
            }

        }

        if (ButtonDown(&m))
        {
            currentwin=IntWinManager(CURRENT,NULL,0,0);
            switch (currentwin) {
            case quitbtn :
                ShutDown();
                exit(1);
                break;
            case newbtn :
                IntWinManager(CLEAR,help,0,0);
                IntWinManager(SET,help,0,0);
                HideMouse();
                DrawText("New...",1,1,GREEN);
                DrawText(":Please Enter",1,2,YELLOW);
                DrawText(":The Number Of Points\n:",1,3,YELLOW);
                scanf("%d",&x);
                DrawText(":The File Name\n:",1,5,YELLOW);
                scanf("%s",buffer);
                ShowMouse();
                if ((out=fopen(buffer,"wt"))==NULL)
                    DrawText("Invalid File Name...",1,7,LTRED);
                else
                {
                    StartRandom();
                    MakeMap(out,x);
                    fclose(out);
                    DrawText("Map Created..",1,7,GREEN);
                }
                SetFile(&currentprob,INITIALISE,TRUE);
//                WriteLocates(PathAsLocates(currentprob->path,currentprob->dist->l),"CURRENT");
//               SetFile(&currentprob,number_of_files-1,FALSE);
                break;
            case savebtn :
                break;
            case gobtn :
                HideMouse();
                InitScreen(16);
                _setactivepage(0);
                _setvisualpage(1);
                SwapPage();

                InsWinManager(Main);
                InsWinManager(Text);
                InsWinManager(BORDER);
//                getch();
                ShowText("Hit Esc to Quit,\n\n"
                     "Any other key \n"
                     "To continue..",1,19,BLUE);

                TotalDelay(WAITKEY);
                if (current_solver==0)
                {
                    SetInit(REQUEST,FALSE,FALSE,&tinit);
                    SetMethod(REQUEST,FALSE,FALSE,&tmethod);
                    insprob=NewInsProb(currentprob->dist);
                    length=ControlAllIns(insprob,tinit,tmethod-1,1);
                }
                DrawNorm(insprob->prob,1);
                SwapPage();

                sprintf(buffer,"F I N I S H E D");
                ShowText(buffer,1,16,LTRED);
                sprintf(buffer,"Length Is %8.6f",length);
                ShowText(buffer,1,18,YELLOW);

                ShowText("                \n"
                     "Hit Esc         \n"
                     "to Quit...   \n"
                     "               ",1,19,LTRED);
            
                while (getch()!=27) {}

                currentprob=insprob->prob;
                WriteLocates(PathAsLocates(currentprob->path,currentprob->dist->l),"CURRENT");
                SetFile(&currentprob,number_of_files-1,FALSE);
                
                InitScreen(16);
                
                _setvisualpage(0);
                _setactivepage(0);
                _clearscreen(_GCLEARSCREEN);
                DrawScreen(&currentprob);
                ResetMouse();
                SetMousePos(mx,my);
                UseMouseCur(GLOVE);
                ShowMouse();
                break;
            case files :
                line=MapPixToText(m.ypos,0)-2;
                if (line<0) select=-1;
                else if (line>=number_of_files) select=-1;
                else select=line;

                if (select>=0) SetFile(&currentprob,select,FALSE);
           
                break;
            case solver :
                line = MapPixToText(m.ypos-VIEWBOTTOM,0);
                if (line<0) select=-1;
                else if (line>=3) select=-1;
                else select=line;
                if (select>=0) SetSolver(select,FALSE,TRUE);
                break;
             case method :
                line = MapPixToText(m.ypos-VIEWBOTTOM,0);
                if (line<0) select=-1;
                else if (line>=number_of_methods) select=-1;
                else select=line;
                if (select>=0) SetMethod(select,FALSE,TRUE,NULL);
                break;
            case init :
                line = MapPixToText(m.ypos-VIEWBOTTOM,0);
                if (line<0) select=-1;
                else if (line>=number_of_inits) select=-1;
                else select=line;
                if (select>=0) SetInit(select,FALSE,TRUE,NULL);
                break;
            }
        }
    }
    while (1);
    ShutDown();
    getch();
}

/*
            _clearscreen(_GCLEARSCREEN);
            SwapPage();

            WinManager(Main);
            WinManager(Text);

            ShowText("Hit Esc to Quit,\n\n"
                 "Any other key \n"
                 "To continue..",1,21,BLUE);


            length=ControlAllIns(insprob,inittype,instype,tdelay!=0);
        
            DrawNorm(insprob->prob,1);
            SwapPage();

            sprintf(buffer,"F I N I S H E D");
            ShowText(buffer,1,16,LTRED);
            sprintf(buffer,"Length Is %8.6f",length);
            ShowText(buffer,1,18,YELLOW);

            ShowText("                \n"
                     "Hit Esc         \n"
                     "to Quit...   \n"
                     "               ",1,19,LTRED);
            
            while (getch()!=27) {}
            ShutDown();

*/
