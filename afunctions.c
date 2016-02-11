/* afunctions.c   Array functions (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "xbasic.h"
#include "array.h"
#include "afunctions.h"
#include "mathematics.h"

ARRAY f_smula(PARAMETER *plist, int e) {
  ARRAY ergeb;
  ergeb.typ=plist->typ;
  ergeb.dimension=plist->integer;
  ergeb.pointer=plist->pointer;
  ergeb=double_array(&ergeb);
  array_smul(ergeb,plist[1].real);
  return(ergeb);
}
ARRAY f_nullmat(PARAMETER *plist, int e) {
    int dimlist[2]={plist[0].integer,plist[1].integer};
    return(nullmatrix(FLOATTYP,e,dimlist));
}
ARRAY f_einsmat(PARAMETER *plist, int e) {
    int dimlist[2]={plist[0].integer,plist[0].integer};
    return(einheitsmatrix(FLOATTYP,2,dimlist));
}


/* Gleichungssystem loesen  d=Mx    x()=SOLVE(m(),d())*/
ARRAY f_solvea(PARAMETER *plist, int e) {
  ARRAY ergeb;
  int anzzeilen,anzspalten;
  ergeb.typ=plist[0].typ;
  ergeb.dimension=1;
  if(plist[0].integer!=2) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */
  if(plist[1].integer!=1) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */
  anzspalten=*((int *)(plist[0].pointer+sizeof(int)));
  anzzeilen=*((int *)(plist[0].pointer));

  if(anzzeilen!=*((int *)(plist[1].pointer))) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */

  ergeb.pointer=malloc(INTSIZE+anzspalten*sizeof(double));
  *((int *)ergeb.pointer)=anzspalten;
  makeSVD2((double *)(plist[1].pointer+plist[1].integer*INTSIZE),(double *)(plist[0].pointer+plist[0].integer*INTSIZE),anzzeilen,anzspalten, (double *)(ergeb.pointer+INTSIZE));
  return(ergeb);
}
#ifdef CONTROL
ARRAY f_csvgeta(char *pos) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(csvget(plist[0].pointer,nn,o));
}
#endif
#ifdef TINE
ARRAY f_tinegeta(PARAMETER *plist, int e) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(tinevget(plist[0].pointer,nn,o));
}
ARRAY f_tinehistorya(PARAMETER *plist, int e) {
	  return(tinehistory(plist[0].pointer,plist[1].integer,plist[2].integer));
}
#endif
#ifdef DOOCS
ARRAY doocsnames(char *n);
ARRAY f_doocsgeta(PARAMETER *plist, int e) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(doocsvget(plist[0].pointer,nn,o));
}
ARRAY f_doocsnames(PARAMETER *plist, int e) {
  return(doocsnames(plist[0].pointer));
}
#endif
