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
#include "rndfile.h"
#include "insert.h"
#include "optim.h"
#include "anneal.h"
#include "mouse.h"

#define VERSION 2.07

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
#define REQUESTFULL -3
#define SETLAST -4

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

int ReadString(char buffer[],int size,int x,int y,int color,int (* filter)(char))
// Reads in a string from a text window
// returns 0 if failure, 1 if success, 2 if exceeded buffer size
{
    int pos;
    char ch[1];

    for (pos=0;pos<size;)
    {
        ch[0]=getch();
        switch (ch[0]) {
            case 27   : return 0;                           // ESC
                        break;
            case 13   : buffer[pos]='\0'; return 1;        // ENTER
                        break;
            case 8    : if (pos>0)
                        {
                            --pos;
                            DrawText(" ",x+pos,y,color);   // BKSPCE
                        }
                        break;
            default   : if (filter(ch[0]))
                        {
                            DrawText(ch,x+pos,y,color);    // CHARACTER
                            buffer[pos]=ch[0];
                            ++pos;
                        }
                        break;
        }
    }
    return 2;
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
    int ix,iy;

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
        map=allocate_N_by_N_array(640,350,sizeof(int),"Screen Map");
        for (ix=0;ix<=639;ix++)
        {
            for (iy=0;iy<=349;iy++)
            {
                if (iy>MSGBOXTOP)
                    if (ix>BTNSTART)     map[ix][iy]=quitbtn;
                    else                 map[ix][iy]=msgbox;
                else if (iy<TITLEHEIGHT) map[ix][iy]=title;
                else if (ix<FILESWIDTH)  map[ix][iy]=files;
                else if (iy<VIEWBOTTOM) { // * help or fview * // 
                    if (ix>VIEWXPOS)     map[ix][iy]=help;
                    else                 map[ix][iy]=fview;
                }
                else {
                    if (ix>FILESWIDTH+6+(METHODWIDTH*3)) // * is a button * //
                    {
                        if (iy>VIEWBOTTOM+2+(BTNHEIGHT*3))      map[ix][iy]=gobtn;
                        else if (iy>VIEWBOTTOM+2+(BTNHEIGHT*2)) map[ix][iy]=rndbtn;
                        else if (iy>VIEWBOTTOM+2+(BTNHEIGHT))   map[ix][iy]=savebtn;
                        else                                    map[ix][iy]=newbtn;
                    }
                    else if (ix>FILESWIDTH+4+(METHODWIDTH*2))   map[ix][iy]=init;
                    else if (ix>FILESWIDTH+2+METHODWIDTH)       map[ix][iy]=method;
                    else                                        map[ix][iy]=solver;
                }
            }
        }
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
                return currenti;
                break;
            case WHERE :
                currenti=map[x][y];
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
        if (num!=(number_of_files-1))
                WriteLocates(PathAsLocates((*currentprob)->path,(*currentprob)->dist->l),"Current");
        ++numfiles;

        number_of_files=numfiles;
        initialised=1;

    }
    if (num==REQUEST) return directory[filenum];
    else if ((num!=filenum) && (num!=INITIALISE))
    {
        if (num==SETLAST)
            filenum=number_of_files-1;
        else
            filenum=num;
        update=1;
        if (numfiles>0)
        {
            FreeMap(& ((*currentprob)->dist) );
            *currentprob=NewProblem(NewMap(NewLocates(directory[filenum])));
            (*currentprob)->path=LocatesAsPath((*currentprob)->dist->l);
        }
    }
    if (num!=INITIALISE) ViewStatistics(*currentprob);
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

void ViewStatistics(PROBLEM *prob)
{
    char buffer[40];
    HideMouse();
    IntWinManager(CLEAR,help,0,0);
    IntWinManager(SET,help,0,0);
    sprintf(buffer,"Statistics Of %s",SetFile(&prob,REQUEST,FALSE));
    DrawText(buffer,1,1,YELLOW);
    sprintf(buffer,"Path Length = %8.6f",PathLength(prob->path,prob->dist));
    DrawText(buffer,1,4,GREEN);
    sprintf(buffer,"Cities      = %4d",prob->dist->size);
    DrawText(buffer,1,6,GREEN);
    ShowMouse();
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
    char buffer[40];

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
        changed=1;
    }
    else if (num==REQUEST) // * request for current * //
    {
        switch (current_solver) {
            case 0 : *inum=insinitnum; return insinit[insinitnum].name;  break;
            case 1 : *inum=-1; return NULL; break;
            case 2 : *inum=anninitnum; return anninit[anninitnum].name;  break;
        }
    }
    else if (num==REQUESTFULL)
    {
        switch (current_solver) {
            case 0 : *inum=insinitnum; return insinit[insinitnum].helptitle;        break;
            case 1 : *inum=-1; return NULL; break;
            case 2 : *inum=anninitnum; return anninit[anninitnum].helptitle;  break;
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

void ViewAbout()
{
    int initialised=0;
    static char buffer[20];
    if (!initialised)
    {
        initialised=1;
        sprintf(buffer,"VTTSP V%1.2f 1997",VERSION);
    }
    HideMouse();
    IntWinManager(CLEAR,help,0,0);
    IntWinManager(SET,help,0,0);
    DrawText(buffer,8,1,GREEN);
    DrawText("       By Drew\n"
             "       \n"
             "       Semester II 1997\n"
             "              for\n"
             "       \n"
             "      PROJECT COORDINATOR\n"
             "         Raymond Lister\n",1,3,GREEN);
    ShowMouse();
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

    ViewAbout();

    SetFile(currentprob,INITIALISE,TRUE);
    SetSolver(0,TRUE,FALSE);

//    Msg("This is the message box!",GREEN);

    IntWinManager(SET,newbtn,0,0);
    DrawText("N",7,2,GREEN);
    DrawText("ew",8,2,BLUE);

    IntWinManager(SET,savebtn,0,0);
    DrawText("S",7,2,GREEN);
    DrawText("ave",8,2,BLUE);
    
    IntWinManager(SET,rndbtn,0,0);
    DrawText("R",4,2,GREEN);
    DrawText("andomize",5,3,BLUE);

    IntWinManager(SET,quitbtn,0,0);
    DrawText("Q",7,2,GREEN);
    DrawText("uit",8,2,BLUE);

    IntWinManager(SET,gobtn,0,0);
    DrawText("G",7,2,GREEN);
    DrawText("O!",8,2,BLUE);

}

void AnnealInitChange()
{
    int x,checki,ok=1;
    double checkd;
    char buffer[60];
    char ch;

    while (kbhit()) getch(); // clear the buffer
    HideMouse();
    IntWinManager(CLEAR,help,0,0);
    IntWinManager(SET,help,0,0);
    sprintf(buffer,"Please Enter The\n:%s\n:",SetInit(REQUESTFULL,FALSE,FALSE,&x));                    
    DrawText(buffer,1,1,YELLOW);
    while (!kbhit()) {}
    if (ReadString(buffer,60,2,3,LTRED,x==2?IsIntChar:IsDoubleChar))
    {
        switch (x) {
            case 0 : checkd=atof(buffer);
                 if (checkd>=0.0)
                 {
                     temperature=checkd;
                     DrawText("Temperature Accepted",1,4,GREEN);
                 }
                 else
                     DrawText("Temperature Must Be > 0.0",1,4,LTRED);
                     break;
            case 1 : checkd=atof(buffer);
                 if ((checkd>0.0) && (checkd<1.0))
                 {
                     coolingrate=checkd;
                     DrawText("Cooling Rate Accepted",1,4,GREEN);
                 }
                 else
                     DrawText("0.0 < cooling rate < 1.0",1,4,LTRED);
                 break;
            case 2 : checki=atoi(buffer);
                 if (checki>0)
                 {
                     changesattemp=checki;
                     DrawText("Changes At Temperature OK",1,4,GREEN);
                 }
                 else
                     DrawText("Must be at least one change...",1,4,LTRED);
                 break;
        }
        SetInit(INITIALISE,TRUE,FALSE,NULL);
    }
    else
        DrawText("Operation Aborted",1,4,GREEN);
    ShowMouse();
}

void NewBtnPress(PROBLEM **currentprob)
{
    FILE *out;
    int x;
    char buffer[10];

    while (kbhit()) getch(); // clear the buffer
    HideMouse();
    IntWinManager(CLEAR,help,0,0);
    IntWinManager(SET,help,0,0);
    DrawText("New...",1,1,GREEN);
    DrawText(":Please Enter",1,2,YELLOW);
    DrawText(":The Number Of Points (Integer) \n:",1,3,YELLOW);
    ReadString(buffer,10,2,4,LTRED,IsIntChar);
    x=atoi(buffer);
    ShowMouse();
    out=fopen("Current","wt");
    if (out!=NULL)
    {
        StartRandom();
        MakeMap(out,x);
        fclose(out);
        DrawText("Map Created..",1,6,GREEN);
    }

//  WriteLocates(PathAsLocates(currentprob->path,currentprob->dist->l),"Current");
    SetFile(currentprob,SETLAST,FALSE);

}

void SaveBtnPress(PROBLEM *currentprob)
{
    char ch,filename[20],buffer[30];

    while (kbhit()) getch(); // clear the buffer
    HideMouse();
    IntWinManager(CLEAR,help,0,0);
    IntWinManager(SET,help,0,0);
    DrawText("Save As :\n:",1,1,YELLOW);
    while (!kbhit()) {}
    if (ReadString(filename,20,2,2,LTRED,IsFileChar))
    {
        WriteLocates(PathAsLocates(currentprob->path,currentprob->dist->l),filename);
        sprintf(buffer,"Map Saved As %s...",filename);
        DrawText(buffer,1,7,GREEN);
        SetFile(&currentprob,INITIALISE,TRUE);
        SetFile(&currentprob,number_of_files-1,FALSE);
    }
    else
        DrawText("Save Aborted..",1,7,LTRED);
    ShowMouse();
}

void RndBtnPress(PROBLEM **currentprob)
{
    char infile[13];

    strcpy(infile,SetFile(currentprob,REQUEST,FALSE));
    IntWinManager(CLEAR,help,0,0);
    IntWinManager(SET,help,0,0);

    DrawText("Randomizing...",1,2,YELLOW);
    rndfile(infile,"Current");

    DrawText("Done!",1,4,GREEN);
    SetFile(currentprob,SETLAST,FALSE);
}

void main()
{
    int pos=320,line;
    MouseStatus m;
    char buffer[60];
    int x,select,at,last,mx=0,my=0,tinit,tmethod,currentwin=title;
    int keypress,keywin;
    INSPROB *insprob;
    PROBLEM *currentprob=NULL;
    PROBLEM *tempprob=NULL;
    double length;
    CHOICE *vttsphelp;

//    char **messages=allocate_N_by_N_array(14,50,sizeof(char),"Messages");

    char messages[13][60]={"",
                           "Choose A File",
                           "View of 'File'",
                           "Choose a TSP method",
                           "This is the message box",
                           "Help on this program                     (Shift-H)",
                           "Choose an 'TSP method' method",
                           "Choose an 'TSP method' initialisation method",
                           "Creates a new random map - to Current    (Shift-N)",
                           "Saves the selected map - to a file       (Shift-S)",
                           "Randomizes the selected map - to Current (Shift-R)",
                           "Start with the current settings          (Shift-G)",
                           "Quit the program                         (Shift-Q)"};


    vttsphelp=NewChoiceArray(1);
    strcpy(vttsphelp[0].name,"VTTSP - Help");
    ReadChoiceHelp("vttsp.hlp",&vttsphelp[0]);


    InitScreen(16);
    _setactivepage(0);
    _setvisualpage(1);

    DrawScreen(&currentprob);

    _setvisualpage(0);
    SetMousePage(0);
    ResetMouse();
    SetMousePos(300,100);
    MouseStat(&m);
    at=IntWinManager(WHERE,NULL,m.xpos,m.ypos);
    last=title;
    ShowMouse();

    UseMouseCur(GLOVE);
    do
    {
        MouseStat(&m);
        if ((my!=m.ypos) || (mx!=m.xpos))
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
                    switch (current_solver)
                    {
                        case 0 : case 1 :
                            sprintf(buffer,"Choose an %s Initialisation Method           ",
                            SetSolver(REQUEST,FALSE,FALSE));
                            break;
                        case 2 :
                            sprintf(buffer,"Annealing Inits LMB = Select, RMB = Change.");
                            break;
                    }
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

        if (kbhit())
        {
            keypress=TRUE;
            switch (getch())
            {
                case 'Q' : keywin=quitbtn; break;
                case 'G' : keywin=gobtn;   break;
                case 'S' : keywin=savebtn; break;
                case 'N' : keywin=newbtn;  break;
                case 'R' : keywin=rndbtn;  break;
                case 'H' : keywin=help;    break;
                default  : keypress=FALSE; break;
            }
        }

        if ((m.left) || (m.right) || (keypress))
        {
            currentwin=IntWinManager(CURRENT,NULL,0,0);
            switch (keypress?keywin:currentwin) {
            case quitbtn :
                ShutDown();
                exit(1);
                break;
            case newbtn :
                NewBtnPress(&currentprob);
                break;
            case savebtn :
                SaveBtnPress(currentprob);
                break;
            case rndbtn:
                RndBtnPress(&currentprob);
//                SetFile(currentprob,number_of_files-1,FALSE);
                break;
            case gobtn :
                HideMouse();

                SetInit(REQUEST,FALSE,FALSE,&tinit);
                SetMethod(REQUEST,FALSE,FALSE,&tmethod);

                InitScreen(16);
                _setactivepage(0);
                _setvisualpage(1);
                SwapPage();

                switch (current_solver)
                {
                    case 0 : case 1 :

                    switch (current_solver)
                    {
                        case 0 : InsWinManager(Main);
                                 InsWinManager(Text);
                                 break;
                        case 1 : OptWinManager(Main);
                                 OptWinManager(Text);
                                 break;
                    }
                    DrawNorm(currentprob,1);
                    SwapPage();

                    ShowText("Hit Esc to Quit,\n"
                             "'0' for Fast Mode\n"
                             "Any other key \n"
                             "To continue..",1,19,BLUE);

                    TotalDelay(WAITKEY);
                    switch (current_solver) {
                    case 0 : insprob=NewInsProb(currentprob->dist);
                             length=ControlAllIns(insprob,tinit,tmethod-1,TRUE);
                             if (length>0.0) currentprob=insprob->prob;
                             break;
                    case 1 : tempprob=NewProblem(currentprob->dist);
                             tempprob->path=LocatesAsPath(PathAsLocates(currentprob->path,currentprob->dist->l));
                             length=ControlAllOpt(tempprob,tmethod+1,TRUE,WAITKEY);
                             if (length>0.0) currentprob=tempprob;
                             break;
                    }
                    if (length>0.0)
                    {
                        sprintf(buffer,"F I N I S H E D");
                        ShowText(buffer,1,16,LTRED);
                        sprintf(buffer,"Length Is %8.6f",length);
                        ShowText(buffer,1,18,YELLOW);

                        DrawNorm(currentprob,1);
                        SwapPage();

                        ShowText("                 \n"
                                 "Hit Esc          \n"
                                 "to Quit...       \n"
                                 "                  ",1,19,LTRED);
                    
                        while (getch()!=27) {}
                        WriteLocates(PathAsLocates(currentprob->path,currentprob->dist->l),"CURRENT");
                        SetFile(&currentprob,number_of_files-1,FALSE);
                    }
                    break;
                    case 2 :
                        ControlAnneal(currentprob,temperature,coolingrate,changesattemp,WAITKEY);
                        break;
                }

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
                if ((m.right) && (current_solver==2))
                {
                    AnnealInitChange();
                }
                break;
            case help :
                if (keypress) // only works off keypress mode
                {
                    HideMouse();
                    IntWinManager(CLEAR,help,0,0);
                    IntWinManager(SET,help,0,0);
                    DrawChoiceHelp(vttsphelp[0]);
                    ShowMouse();
                }
                break;
            }
        }

        if (keypress) keypress=FALSE;
    }
    while (1);
    ShutDown();
}

