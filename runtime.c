
/* runtime.c Sachen fuer den Interpreter-Programmablauf (c) Markus Hoffmann*/


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xbasic.h"

void usage(){
  printf("\n Bedienungsanleitung: \n");
  printf(" -------------------- \n\n");
  printf("Das Programm kann mit folgenden Parametern aufgerufen werden:  \n");
  printf("\n");
  printf("%s <filename> --- Basic-Programm ausfuehren  [%s]\n",xbasic_name,ifilename);
  printf("\n"); 
  printf("-l                  --- Programm nur laden \n");
  printf("-e <kommando>       --- Basic Kommando ausfuehren \n");
  printf("--eval <ausdruck>   --- Num. Ausdruck auswerten  \n");
  printf("\n");
}
void intro(){
  printf("***************************************************************\n");
  printf("*             %s                 V. %s                *\n",xbasic_name, version);
  printf("*                   von Markus Hoffmann 1997                  *\n");
  printf("*                                                             *\n");
  printf("*                                                             *\n");
  printf("* Programmversion vom %s                              *\n",vdate);
  printf("*                                                             *\n");
  printf("***************************************************************\n\n"); 
}

void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;

  /* Kommandozeile bearbeiten   */
  runfile=TRUE;
  for(count=1;count<anzahl;count++) {
    if (strcmp(argumente[count],"-l")==FALSE)               runfile=FALSE;
    else if (strcmp(argumente[count],"--load-only")==FALSE) runfile=FALSE;
    else if (strcmp(argumente[count],"--eval")==FALSE) {
      printf("%.13g\n",parser(argumente[++count])); 
      quitflag=1;
    } else if (strcmp(argumente[count],"-e")==FALSE) {
      kommando(argumente[++count]);
      quitflag=1;
    } else if (strcmp(argumente[count],"--exec")==FALSE) {
      kommando(argumente[++count]);
      quitflag=1;
    } else if (strcmp(argumente[count],"--help")==FALSE) {
      intro();
      usage();
      quitflag=1;
    } else {
      loadfile=TRUE;
      strcpy(ifilename,argumente[count]); 
    }
   }
   if(quitflag) c_quit("");
}

int loadprg(char *filename) {

  programbufferlen=0;prglen=0; pc=0; sp=0;
    mergeprg(filename);
    return(0);
}
