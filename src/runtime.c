
/* runtime.c Sachen fuer den Interpreter-Programmablauf (c) Markus Hoffmann*/


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "protos.h"
#include "globals.h"
#include "options.h"


void *obh;       /* old break handler  */


/* Standard-Fehlerroutine   */

int globalerr=0;

void xberror(char errnr, char *bem) {
  extern int globalerr;
  globalerr=errnr;
  if(errcont) {   
    errcont=0;
    if(errorpc!=-1) {
      int osp=sp;
        if(sp<STACKSIZE) {stack[sp++]=pc;pc=errorpc+1;}
        else {printf("Stack overflow ! PC=%d\n",pc); batch=0;}
        programmlauf();
	
        if(osp!=sp) {
	  pc=stack[--sp]; /* wenn error innerhalb der func. */
        }
      }
  } else { 
    batch=0;   
#ifdef GERMAN
    printf("Zeile %d: %s\n",pc-1,error_text(errnr,bem));
#else
    printf("Line %d: %s\n",pc-1,error_text(errnr,bem));
#endif
  }
}

extern int breakcont;
extern int breakpc;

void break_handler( int signum) {
  if(batch) {
    if(breakcont) {
      breakcont=0;
      if(breakpc!=-1) {
        int osp=sp;
        if(sp<STACKSIZE) {stack[sp++]=pc;pc=breakpc+1;}
        else {printf("Stack overflow ! PC=%d\n",pc); batch=0;}
        programmlauf();
        if(osp!=sp) {
	  pc=stack[--sp]; /* wenn error innerhalb der func. */
        }
      }
    } else {
      puts("** PROGRAM-STOP");
      batch=0;
    }
    signal(signum, break_handler);
  } else {
    puts("** X11BASIC-QUIT");
#ifdef DOOCS
    doocssig_handler(signum);
#endif  
    signal(SIGINT, obh);
    raise(signum);
  }
}
void fatal_error_handler( int signum) {
  int obatch=batch;
  switch(signum) {
  case SIGILL: xberror(104,""); break;
#ifndef WINDOWS
  case SIGBUS: xberror(102,""); break;
#endif
  case SIGSEGV: xberror(101,""); break;
#ifndef WINDOWS
  case SIGPIPE: xberror(110,""); break;
#endif
  default:
    printf("** Fataler BASIC-Interpreterfehler #%d \n",signum);
    batch=0;
  }
  if(obatch) {
    if(pc>0) {
      if(pc>1) printf("    pc-1  : %s\n",program[pc-2]);
      printf("--> pc=%d : %s\n",pc-1,program[pc-1]);
      printf("    pc+1  : %s\n",program[pc]);
    } else puts("PC negativ !");
    puts("** PROGRAM-STOP");
  } else {
    printf("Stack-Pointer: SP=%d\n",sp);
    c_dump("");
    puts("** fatal error ** X11BASIC-QUIT");    
    signal(signum,SIG_DFL);
  }
  raise(signum);
}
#ifdef WINDOWS
void alarm(int dummy) {

}
#endif

void timer_handler( int signum) {
  if(alarmpc==-1) printf("** Uninitialisierter Interrupt #%d \n",signum);
  else {
    int oldbatch,osp=sp,pc2;
      pc2=procs[alarmpc].zeile;
      
      if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
      else {printf("Stack overflow ! PC=%d\n",pc); batch=0;}
      oldbatch=batch;batch=1;
      programmlauf();
      batch=min(oldbatch,batch);
      if(osp!=sp) {
	pc=stack[--sp]; /* wenn error innerhalb der func. */
      }   
  }
  
  signal(signum, timer_handler);
  if(everyflag) alarm(everytime); 
}


/* Initialisierungsroutine  */

void x11basicStartup() {

#ifdef CONTROL  
  cs_init();        /* Kontrollsystem anmelden */
#endif
  /* Signal- und Interrupt-Handler installieren  */
  obh=signal(SIGINT, break_handler);
  signal(SIGILL, fatal_error_handler);
  signal(SIGSEGV, fatal_error_handler);
#ifndef  WINDOWS  
  signal(SIGALRM, timer_handler);
  signal(SIGBUS, fatal_error_handler);
#endif
#ifdef DOOCS
  signal(SIGPIPE, doocssig_handler);
  signal(SIGTERM, doocssig_handler);
#else
#ifndef WINDOWS
  signal(SIGPIPE, fatal_error_handler);
#endif
#endif
}
