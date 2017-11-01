/*

$VER: ERequestSlide.a 1.1 (c)2003 by Mauro Panigada

ERequestSlide

TEMPLATE
========

TO/N/A,FROM,VALUE,HEX/S,HEXPREFIX/K,MOUSEPOS/S,CENTER/S,FUNC/K,PUBSCREEN/K

TO        mandatory, the max value;
FROM      the lowest value
VALUE     set slider to this value
HEX       output in hex
HEXPREFIX output in hex with prefix
MOUSEPOS  open window on mouse
CENTER    center the window
FUNC      parsing math func
PUBSCREEN open window on public screen...


THE FUNC FEATURE
================

The string provided to FUNC keywords is a set of math functions and/or operators
that ERequestSlide knows, in order to modify slider values according to a
function you choose. This makes you able to do a non linear choose.
The notation used is RPN; the stack is 256 double values, enough for any
computation you need here. The value are computed in double precision
but the result is INTEGER. At the beginning, the stack holds the value of
the slider, and as result is taken the value on the top of stack.
Here the words it knows.

+ * / - neg % ^ sin cos exp log ln pop swap sqrt

*/

#include <exec/types.h>
#include <ctype.h>
#include <string.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>

#define OK_ID 1
#define CANCEL_ID 2
#define SLIDE_ID 3
#define STR_ID 4

  static char version[] = "$VER: ERequestSlide 1.1 (c)2003 Mauro Panigada\n";

  struct IntuitionBase *IntuitionBase;
  struct DosLibrary *DOSBase;
  struct Library *GadToolsBase;

  static char template[] = "TO/N/A,FROM/N,VALUE/N,HEX/S,HEXPREFIX/K,TITLE/K,TEXT/K,MOUSEPOS/S,CENTER/S,FUNC/K,PUBSCREEN/K";
  static char myname[] = "ERequestSlide";

  struct Gadget *last;
  struct Gadget *context = NULL;
  struct Gadget *gadu, *creagadget();

  struct DrawInfo *dri;

  struct Window *win;
  struct Screen *scr;

  struct Gadget *ok_gad, *cancel_gad, *str_gad, *slide_gad, *text_gad;

  static struct NewGadget okgad = {0,0,
                                   0,0,
                                   "OK",NULL,
                                   OK_ID,
                                   PLACETEXT_IN,
                                   NULL,NULL};

  static struct NewGadget cancelgad = {0,0,
                                       0,0,
                                       "Cancel",NULL,
                                       CANCEL_ID,
                                       PLACETEXT_IN,
                                       NULL,NULL};

  static struct NewGadget strgad = {0,0,
                                    0,0,
                                    NULL,NULL,
                                    STR_ID,
                                    PLACETEXT_IN,
                                    NULL,NULL};

  static struct NewGadget slidegad = {0,0,
                                      0,0,
                                      NULL,NULL,
                                      SLIDE_ID,
                                      0,
                                      NULL,NULL};

  static struct NewGadget textgad = {0,0,
                                     0,0,
                                     NULL,NULL,
                                     78,
                                     PLACETEXT_IN,
                                     NULL,NULL};

  struct argomenti {
    LONG *to;
    LONG *from;
    LONG *value;
    LONG hex;
    char *hexprefix;
    char *title;
    char *testo;
    LONG mousepos;
    LONG center;
    char *func;
    char *pubscreen;
  } arg = {NULL,NULL,NULL,0,NULL,NULL,NULL,0,0,NULL,NULL};

  APTR vi;

  WORD winwidth,winheight;

  struct Window *win;

  LONG thefrom;

  VOID aggiornaslide(VOID);
  VOID aggiornastr(VOID);


//**************************** PARSER

  VOID push(double);
  VOID compute(char *);
  double pop(VOID);
  VOID ecomp(int);

/* STACK */
  double stack[256];
  int    si;

/* keywords */

#define K_SUB 1
#define K_ADD 2
#define K_DIV 3
#define K_MUL 4
#define K_MOD 5
#define K_LOG 6
#define K_LN 7
#define K_EXP 8
#define K_SIN 9
#define K_COS 10
#define K_SQR 11
#define K_POT 12
#define K_NEG 13
#define K_POP 14
#define K_SWA 15
#define K_LEN 15

double log(double);
double log10(double);
double exp(double);
double sin(double);
double cos(double);
double sqrt(double);
double atof(char *);
double strtod(char *,char *);
void resetstack(void);

  static struct ckeyf {
    char *key;
    int action;
  } ckey[] = { "-",K_SUB,
               "+",K_ADD,
               "/",K_DIV,
               "*",K_MUL,
               "%",K_MOD,
               "log",K_LOG,
               "ln",K_LN,
               "exp",K_EXP,
               "sin",K_SIN,
               "cos",K_COS,
               "sqrt",K_SQR,
               "^",K_POT,
               "neg",K_NEG,
               "pop",K_POP,
               "swap",K_SWA,
             };

LONG cfunc(LONG valore)
{
  si = 0;

  if ( arg.func == NULL ) {
        return valore;
  } else {
    resetstack();
    push((double)valore);
    compute(arg.func);
    return (LONG)pop();
  }
}

void push(double valo)
{
  stack[si] = valo;
  si++;
  si %= 255;
}

double pop()
{
  si--;
  if (si < 0) { si = 255; }
  return stack[si];
}

void resetstack()
{
  si = 0;
}

void compute(char *linea)
{
  char minibuf[256];
  char *minbufend;
  int i;
  BOOL ex = TRUE;
  double appo;

  while (ex) {
    while ( *linea != '\0' && *linea == ' ') { linea++; }
    if ( *linea == '\0' ) {
      ex = FALSE;
    } else {
      for (i=0; i<254 ; i++) {
        if ( *linea == ' ' || *linea == '\0') { break; }
        minibuf[i] = *linea;
        linea++;
      }
      if ( *linea == '\0' ) { ex = FALSE; }
      minibuf[i] = 0;
      minbufend = &minibuf[i];
      if ( isdigit((int)minibuf[0]) ) {
        appo = strtod(minibuf,minbufend);
        push(appo);
      } else {
        //forse e` un operatore...
        for (i=0; i<K_LEN; i++) {
          if ( strcmpi(minibuf,ckey[i].key) == 0) {
            ecomp(ckey[i].action);
            break;
          }
        }
      }
    }
  }
/*
  while (~ex) {

    while ( *linea != '\0' && *linea ==' ') { linea++; }

    if ( *linea != '\0' ) {
      for (i=0; i<254 ; i++) {
        if ( *linea == ' ' || *linea == '\0' ) { break; }
        minibuf[i] = *linea;
        linea++;
      }
      if ( *linea == '\0' ) { ex= TRUE; }
      minibuf[i]=0;
      minbufend = &minibuf[i];
      if ( isdigit((int)minibuf[0]) ) {
        appo = strtod(minibuf,minbufend);
        push(appo);
      }
      else if ( (minibuf[0] == '-') && isdigit((int)minibuf[1]) ) {
        appo = strtod(minibuf,minbufend);
        push(appo);
      }
      else {
        //forse e` un operatore...
        for (i=0; i<K_LEN; i++) {
          if ( strcmpi(minibuf,ckey[i].key) == 0) {
            ecomp(ckey[i].action);
            break;
          }
        }
      }
    } else { ex = TRUE; }
  }
  */
}

VOID ecomp(int az)
{
  double appo,app1;

  switch(az) {
    case K_ADD:
       push(pop() + pop());
       break;
    case K_SUB:
       appo = pop();
       push(pop() - appo);
       break;
    case K_DIV:
       appo = pop();
       if (appo != 0) {
         push(pop() / appo);
       }
       break;
    case K_MUL:
       push(pop() * pop());
       break;
    case K_MOD:
       appo = pop();
       push( (double) ( (long)pop() % (long)appo ) );
       break;
    case K_LOG:
       appo = pop();
       if (appo > 0) {
         push(log10(appo));
       }
       break;
    case K_LN:
       appo = pop();
       if (appo > 0) {
         push(log(appo));
       }
       break;
    case K_EXP:
       push(exp(pop()));
       break;
    case K_SIN:
       push(sin(pop()));
       break;
    case K_COS:
       push(cos(pop()));
       break;
    case K_SQR:
       appo = pop();
       if (appo >= 0) {
         push(sqrt(appo));
       }
       break;
    case K_NEG:
       push(-pop());
       break;
    case K_POP:
       pop();
       break;
    case K_SWA:
       appo = pop();
       app1 = pop();
       push(appo);
       push(app1);
       break;
    case K_POT:
       appo = pop();
       app1 = pop();
       if (app1 == 0) { push(0.0); }
       else if (appo == 0) {push(1.0); }
       else {
         appo = appo*log(app1);
         push(exp(appo));
       }
       break;
  }
}

//************************* END PARSER


struct Gadget *creagadget(struct Gadget **cont)
{
  struct Gadget *l;
  struct TextAttr *fonto;
  struct IntuiText it;
  WORD posy, tx,bors,bord,dx;

  fonto = scr->Font;
  bors = (WORD)(scr->WBorLeft);
  bord = (WORD)(scr->WBorRight);
  dx = bors + 2;

  if ( (vi=GetVisualInfoA(scr,NULL)) != NULL) {

    posy = (WORD)(scr->BarHeight) + 3;

    l = CreateContext(cont);

    if (arg.testo != NULL) {
      textgad.ng_LeftEdge = dx;
      textgad.ng_TopEdge = posy;
      textgad.ng_VisualInfo = vi;
      textgad.ng_TextAttr = fonto;
      textgad.ng_Height = fonto->ta_YSize + 4;
      //textgad.ng_GadgetText = arg.testo;
      it.ITextFont = fonto;
      it.IText = arg.testo;
      tx = IntuiTextLength(&it) + 4;
      if ( tx > (scr->Width - bors - bord - 4) ) { tx = scr->Width - bors - bord - 4; }
      if ( tx < ( (scr->Width)>>1 )) {tx = (scr->Width)>>1;}
      textgad.ng_Width = tx;
      text_gad = l = CreateGadget(TEXT_KIND,l,&textgad, GTTX_Text, arg.testo,
                                             GTTX_Border, TRUE,
                                             GTTX_CopyText,TRUE,
                                             GTTX_Clipped,TRUE,
                                             TAG_END);
      posy += (l->Height) + 2;
    }

    thefrom = (arg.from == NULL) ? 0 : *(arg.from);
    slidegad.ng_VisualInfo = vi;
    slidegad.ng_LeftEdge = dx;
    slidegad.ng_TopEdge = posy;
    slidegad.ng_Width = (text_gad == NULL) ? ((scr->Width)>>1) : (text_gad->Width);
    slidegad.ng_Height = (fonto->ta_YSize);
    slide_gad = l = CreateGadget(SLIDER_KIND,l,&slidegad, GTSL_Min, thefrom,
                                              GTSL_Max, *(arg.to),
                                              GTSL_Level, (arg.value == NULL) ? thefrom : *(arg.value),
                                              TAG_END);
    posy += (l->Height) +2 ;

    strgad.ng_VisualInfo = vi;
    strgad.ng_TopEdge = posy;
    strgad.ng_LeftEdge = dx;
    strgad.ng_Width = slide_gad->Width;
    strgad.ng_Height = (fonto->ta_YSize) + 4;
    strgad.ng_TextAttr = fonto;
    str_gad = l = CreateGadget(INTEGER_KIND,l,&strgad, GTIN_Number, (arg.value == NULL) ? cfunc(thefrom) : cfunc(*(arg.value)),
                                             STRINGA_Justification,STRINGCENTER,
                                             TAG_END);
    posy += (l->Height) +5;

    it.ITextFont = fonto;
    it.IText = "OK";
    tx = IntuiTextLength(&it) + 6;

    okgad.ng_VisualInfo = vi;
    okgad.ng_Width = tx;
    cancelgad.ng_Height = okgad.ng_Height = (fonto->ta_YSize)+3;
    okgad.ng_LeftEdge = dx;
    cancelgad.ng_TopEdge = okgad.ng_TopEdge = posy;
    okgad.ng_TextAttr = cancelgad.ng_TextAttr = fonto;
    ok_gad = l = CreateGadgetA(BUTTON_KIND,l,&okgad, NULL);
    dx += l->Width +2;

    it.IText = "Cancel";
    tx = IntuiTextLength(&it) +6;

    cancelgad.ng_VisualInfo = vi;
    cancelgad.ng_LeftEdge = dx;
    cancelgad.ng_Width = tx;
    cancel_gad = l = CreateGadgetA(BUTTON_KIND,l,&cancelgad,NULL);

    posy += l->Height + scr->WBorBottom +2;
    winheight = posy;

    winwidth = ( (slide_gad->Width) > (ok_gad->Width + cancel_gad->Width +2) ) ? slide_gad->Width : (ok_gad->Width + cancel_gad->Width +2);
    winwidth += bors + bord +4;

  } else {return NULL;}

  return l;
}

#pragma libcall GadToolsBase GT_GetGadgetAttrsA AE BA9804
#pragma tagcall GadToolsBase GT_GetGadgetAttrs AE BA9804

VOID aggiornastr()
{
  LONG livello; //,nom;

  GT_GetGadgetAttrs(slide_gad,win,NULL,
                              GTSL_Level,&livello,TAG_END);

  //nom = RemoveGadget(win,str_gad);

  GT_SetGadgetAttrs(str_gad,win,NULL,
                          GTIN_Number,cfunc(livello),TAG_END);

  //AddGadget(win,str_gad,nom);
  RefreshGList(str_gad,win,NULL,1);
  //GT_RefreshWindow(win,NULL);

}

VOID aggiornaslide()
{
  LONG valore;

  GT_GetGadgetAttrs(str_gad,win,NULL,
                           GTIN_Number,&valore,TAG_END);

  if ( valore > *(arg.to) ) {valore = *(arg.to);}
  else if ( valore < thefrom ) {valore = thefrom;}

  GT_SetGadgetAttrs(slide_gad,win,NULL,
                      GTSL_Level,valore,TAG_END);
  GT_SetGadgetAttrs(str_gad,win,NULL,
                      GTIN_Number,cfunc(valore),TAG_END);

  RefreshGList(slide_gad,win,NULL,1);
  RefreshGList(str_gad,win,NULL,1);
}


int main(void)
{
  struct IntuiMessage *imsg;

  LONG retval = RETURN_FAIL;

  struct RDArgs *rarg;


  BOOL exitflag = FALSE, valflag = FALSE;
  char *tit;
  char defti[] = "ERequestSlide";

  LONG cl,uscita;

  WORD winx,winy;


  DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",39L);
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",39L);
  GadToolsBase = OpenLibrary("gadtools.library",39L);

  if (DOSBase && IntuitionBase && GadToolsBase) {
    if ( (rarg = ReadArgs(template,(LONG *)&arg,0)) != NULL) {
      if ( (scr = LockPubScreen(arg.pubscreen)) != NULL) {
        dri = GetScreenDrawInfo(scr);

          if ( (gadu = creagadget(&context) ) != NULL) {

            tit = (arg.title == NULL) ? defti : arg.title;

            winx =winy= 0;
            if (arg.center) {
              winx = ((scr->Width) - winwidth)>>1;
              winy = ((scr->Height) - winheight)>>1;
            }
            if (arg.mousepos) {
              winx = (scr->MouseX);
              winy = (scr->MouseY);
            }
            win = OpenWindowTags(NULL,
                            WA_Title, tit,
                            WA_Width, (LONG)winwidth,
                            WA_Height,(LONG) winheight,
                            WA_Left,(LONG)winx,
                            WA_Top,(LONG)winy,
                            WA_Gadgets,context,
                            WA_PubScreen,scr,
                            WA_SizeGadget,FALSE,
                            WA_DragBar,TRUE,
                            WA_DepthGadget,TRUE,
                            WA_Activate,TRUE,
                            WA_CloseGadget,TRUE,
                            WA_RMBTrap,TRUE,
                            WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_MOUSEMOVE|IDCMP_REFRESHWINDOW|SLIDERIDCMP,
                            TAG_END);
             if (win) {
               retval = RETURN_OK;

               GT_RefreshWindow(win,NULL);
               while (exitflag == FALSE) {
                 WaitPort(win->UserPort);
                 imsg = GT_GetIMsg(win->UserPort);
                 if (imsg) {
                   cl = imsg->Class;
                   if (cl == IDCMP_CLOSEWINDOW)
                          { exitflag = TRUE; retval = RETURN_WARN; }
                   if (cl == IDCMP_MOUSEMOVE)
                          { aggiornastr(); }
                   if (cl == IDCMP_REFRESHWINDOW) {
                     GT_BeginRefresh(win);
                     GT_EndRefresh(win,TRUE);
                     continue;
                   }
                   if (cl == IDCMP_GADGETUP) {
                     gadu = imsg->IAddress;
                     switch (gadu->GadgetID) {
                       case OK_ID:
                         exitflag = TRUE;
                         valflag = TRUE;
                         break;
                       case CANCEL_ID:
                         exitflag = TRUE;
                         retval = RETURN_WARN;
                         break;
                       case STR_ID:
                         aggiornaslide();
                         break;
                       case SLIDE_ID:
                         aggiornastr();
                         break;
                       default:
                         break;
                     }
                   }
                   GT_ReplyIMsg(imsg);
                 }
               }
             }
          }
      }
    } else {
      PrintFault(IoErr(),myname);
    }
  }

  if (valflag) {
    GT_GetGadgetAttrs(str_gad,win,NULL,
                       GTIN_Number,&uscita,TAG_END);

    if (arg.hex) {
      if (arg.hexprefix == NULL) {
        Printf("%lx\n",uscita);
      } else { Printf("%s%lx\n",arg.hexprefix,uscita); }
    } else if (arg.hexprefix != NULL) {
      Printf("%s%lx\n",arg.hexprefix,uscita);
    } else {
      Printf("%ld\n",uscita);
    }
  }

exit:
  if (win !=NULL) {CloseWindow(win);}
  FreeGadgets(context);
  if (vi != NULL) { FreeVisualInfo(vi); }
  if (dri != NULL) { FreeScreenDrawInfo(scr,dri); }
  if (scr != NULL) { UnlockPubScreen(NULL,scr); }
  if (rarg != NULL) { FreeArgs(rarg); }
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)GadToolsBase);
  CloseLibrary((struct Library *)DOSBase);

  return retval;
}
