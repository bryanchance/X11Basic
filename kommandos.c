/* kommandos.c  Sachen fuer die Kommandobearbeitung (c) Markus Hoffmann */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#if defined WINDOWS
#define EX_OK 0
#include <windows.h>
#include <io.h>
#else
#ifndef ANDROID
#include <sysexits.h>
#else 
#define EX_OK 0
#endif
#endif
#include <ctype.h>
#include <time.h>
#include <errno.h>

#include "defs.h"
#include "x11basic.h"
#include "parameter.h"
#include "variablen.h"
#include "xbasic.h"

#include "kommandos.h"
#include "array.h"
#include "sound.h"
#include "parser.h"
#include "wort_sep.h"
#include "io.h"
#include "file.h"
#include "graphics.h"
#include "aes.h"
#include "window.h"
#include "mathematics.h"
#include "gkommandos.h"
#include "sysVstuff.h"
#include "bytecode.h"


/*********************/
static int saveprg(char *fname) {
  char *buf=malloc(programbufferlen);
  int i=0;
  while(i<programbufferlen) {
    if(programbuffer[i]==0 || programbuffer[i]=='\n')
      buf[i]='\n';
    else
      buf[i]=programbuffer[i];
    i++;
  }
  bsave(fname,buf,programbufferlen);
  return(0);
}



/*****************************************/
/* Kommandos zur Programmablaufkontrolle */

void c_gosub(char *n) {
    char *buffer,*pos,*pos2;
    int pc2;
   
    buffer=indirekt2(n);
    pos=searchchr(buffer,'(');
    if(pos!=NULL) {
      pos[0]=0;pos++;
      pos2=pos+strlen(pos)-1;
      if(pos2[0]!=')') {
        puts("Syntax error bei Parameterliste");
        xberror(39,buffer); /* Program Error Gosub impossible */
      }	
      else pos2[0]=0;
    } else pos=buffer+strlen(buffer);
    
    pc2=procnr(buffer,1);
    if(pc2==-1)   xberror(19,buffer); /* Procedure nicht gefunden */
    else {       
	if(do_parameterliste(pos,procs[pc2].parameterliste,procs[pc2].anzpar)) xberror(42,buffer); /* Zu wenig Parameter */
	else { batch=1;
	  pc2=procs[pc2].zeile;
	  if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
	  else {
	    printf("Stack-Overflow ! PC=%d\n",pc); 
	    xberror(39,buffer); /* Program Error Gosub impossible */
	  }
	}
    }
    free(buffer);
}
/* Spawn soll eine Routine als separaten thread ausfuehren.
   Derzeit klappt as nur als separater Task. Das bedeutet, dass 
   die beiden Programmteile nicht ueber die Variablen reden koennen.
   Hierzu muesste man die XBASIC-Variablen in Shared-Memory auslagern.
   das waere aehnlich wie EXPORT ....
   */
void c_spawn(char *n) {
  char *buffer,*pos,*pos2;
  int pc2;
  
  buffer=indirekt2(n);
  pos=searchchr(buffer,'(');
  if(pos!=NULL) {
    pos[0]=0;pos++;
    pos2=pos+strlen(pos)-1;
    if(pos2[0]!=')') {
      puts("Syntax error bei Parameterliste");
      xberror(39,buffer); /* Program Error Gosub impossible */
    } 
    else pos2[0]=0;
  } else pos=buffer+strlen(buffer);
  
  pc2=procnr(buffer,1);
  if(pc2==-1)	xberror(19,buffer); /* Procedure nicht gefunden */
  else {
    #ifndef WINDOWS
    pid_t forkret=fork();
    if(forkret==-1) io_error(errno,"SPAWN");
    if(forkret==0) {
      if(do_parameterliste(pos,procs[pc2].parameterliste,procs[pc2].anzpar)) xberror(42,buffer); /* Zu wenig Parameter */
      else { 
        batch=1;
        pc2=procs[pc2].zeile;
        if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
        else {
          printf("Stack-Overflow ! PC=%d\n",pc); 
          xberror(39,buffer); /* Program Error Gosub impossible */
        }      
        programmlauf();
        exit(EX_OK);
      }
    }
    #endif
  }
  free(buffer);
}
      

void c_local(PARAMETER *plist,int e) {
  int i;
  if(e) {
    for(i=0;i<e;i++) do_local(plist[i].integer,sp);
  }
}

void c_goto(char *n) {
  char *b=indirekt2(n);
  pc=labelzeile(b);
  if(pc==-1) {xberror(20,b);/* Label nicht gefunden */ batch=0;}
  else batch=1;
  free(b);
}


void c_system(PARAMETER *plist,int e) {
  if(system(plist[0].pointer)==-1) io_error(errno,"system");
}
void c_edit(char *n) {
    char filename[strlen(ifilename)+8];
    char buffer[256];
    char *buffer2=strdup(ifilename);
    int sret;
#ifndef ANDROID
    sprintf(filename,"%s.~~~",ifilename);
    saveprg(filename);
    sprintf(buffer,"$EDITOR %s",filename); 
    sret=system(buffer);
    c_new("");  
    strcpy(ifilename,buffer2);
    free(buffer2);
    mergeprg(filename);
    sprintf(buffer,"rm -f %s",filename); 
    sret=system(buffer);
#else
    puts("The EDIT command is not available in ANDROID version.\n"
    "Please use Menu --> Editor to edit the current program.");
#endif
}

void c_after(PARAMETER *plist,int e) {
    everyflag=0;
    alarmpc=plist[1].integer; /*Proc nummer*/
    alarm(plist[0].integer);     /*Zeit in sec*/
}

void c_every(PARAMETER *plist,int e) {
  everyflag=1;
  alarmpc=plist[1].integer; /*Proc nummer*/
  everytime=plist[0].integer; /*Zeit in sec*/
  alarm(everytime);
}


void c_dodim(char *w) {
  char w1[strlen(w)+1],w2[strlen(w)+1];
  int e=klammer_sep(w,w1,w2);
 
  if(e<2) xberror(32,"DIM"); /* Syntax nicht Korrekt */
  else {
    char *s,*t;
    ARRAY arr;
    int ndim=count_parameters(w2);
    int dimlist[ndim];
    int i,vnr,typ;
    char *r=varrumpf(w1);
    typ=type(w1)&(~CONSTTYP);  /* Typ Bestimmen  */

    /* Dimensionen bestimmen   */
     
    i=wort_sep_destroy(w2,',',TRUE,&s,&t);
    ndim=0;
    while(i) {
      xtrim(s,TRUE,s);
      dimlist[ndim++]=(int)parser(s);
      i=wort_sep_destroy(t,',',TRUE,&s,&t); 
    }
//  printf("DIM: <%s>: dim=%d typ=$%x\n",r,ndim,typ);
    vnr=add_variable(r,ARRAYTYP,typ);
      
    /*(Re-) Dimensionieren  */
    free_array(variablen[vnr].pointer.a); /*Alten Inhalt freigeben*/
    *(variablen[vnr].pointer.a)=create_array(typ,ndim,dimlist);
//    printf("created array of type: %d\n",(variablen[vnr].pointer.a)->typ);

    free(r);
  }
}
static void do_restore(int offset) {
  datapointer=offset;
 // printf("DO RESTORE %d\n",offset);
}

void c_run(char *n) {        /* Programmausfuehrung starten und bei 0 beginnen */
  do_run();
}
void do_run() {
  pc=sp=0;
  clear_all_variables();
  batch=1;
  do_restore(0);
}

void c_chain(PARAMETER *plist,int e){ c_load(plist,e); c_run(""); }

void c_cont(char *n) {
  if(batch==0) {
    if(pc>=0 && pc<=prglen) batch=1;
    else xberror(41,"");     /*CONT nicht moeglich !*/
  } else {
    int p1,p2,p;
    /*hier koennte es ein CONTINUE in einer SWITCH Anweisung sein. */
    /*gehe zum (bzw. hinter) naechsten CASE oder DEFAULT*/
    p1=suchep(pc,1,P_CASE,P_SELECT,P_ENDSELECT);
    p2=suchep(pc,1,P_DEFAULT,P_SELECT,P_ENDSELECT);
    if(p1==-1) p=p2;
    else if(p2==-1) p=p1;
    else p=min(p1,p2);
    if(p==-1) p=suchep(pc,1,P_ENDSELECT,P_SELECT,P_ENDSELECT); 
    if(p==-1) xberror(36,"SELECT/CONTINUE"); /*Programmstruktur fehlerhaft !*/ 
    pc=p;
    pc++;
  }
}

void c_restore(PARAMETER *plist,int e) {
  if(e) {
    do_restore((int)labels[plist[0].integer].datapointer);
  //  printf("RESTORE: %d %s\n",plist[0].integer,labels[plist[0].integer].name);
  } else do_restore(0);
}

char *get_next_data_entry() {
  char *ptr,*ptr2;
  char *ergebnis=NULL;
  if(databufferlen==0 || databuffer==NULL || datapointer>=databufferlen) return(NULL);
  ptr=databuffer+datapointer;
  ptr2=searchchr(ptr,',');
 // printf("Inhalt: k2=%d <%s>\n",ptr2-ptr,ptr);
  if(ptr2==NULL) {
    ergebnis=malloc(databufferlen-datapointer+1);
    strncpy(ergebnis,ptr,databufferlen-datapointer);
    ergebnis[databufferlen-datapointer]=0;
    datapointer=databufferlen;
  } else {
    ergebnis=malloc(ptr2-ptr+1);
    strncpy(ergebnis,ptr,(int)(ptr2-ptr));
    datapointer+=(ptr2-ptr)+1;
    ergebnis[ptr2-ptr]=0;
  } 
//  printf("READ: <%s>\n",ergebnis);
  return(ergebnis);
}

void c_read(PARAMETER *plist,int e) {
  int i;
  VARIABLE *v;
  char *t;
  for(i=0;i<e;i++) {
    t=get_next_data_entry();
    if(t==NULL) xberror(34,""); /* Zu wenig Data */
    else {
    switch(plist[i].typ) {
    case PL_ARRAYVAR: 
      printf("Read array. Not yet implemented.\n");
      break;
    case PL_SVAR:
      free_string((STRING *)plist[i].pointer);
      if(*t=='\"') *((STRING *)(plist[i].pointer))=string_parser(t);
      else *((STRING *)(plist[i].pointer))=create_string(t);
      break;
    case PL_IVAR:
      *((int *)(plist[i].pointer))=(int)parser(t);
      break;
    case PL_FVAR:
      *((double *)(plist[i].pointer))=parser(t);
      break;
    default:
      printf("ERROR: READ, unknown var type.\n");
      dump_parameterlist(&plist[i],1);
    }
    free(t);
    }
  }
}


void c_randomize(PARAMETER *plist, int e) {
  unsigned int seed;
  if(e) seed=plist[0].integer;
  else {
    seed=time(NULL);
    if(seed==-1) io_error(errno,"RANDOMIZE");
  }
  srand(seed);
}

void c_list(PARAMETER *plist, int e) {
  int i,a=0,o=prglen;
  if(e==2) {
    a=min(max(plist[0].integer,0),prglen);
    o=max(min(plist[1].integer+1,prglen),0);
  } else if(e==1) {
    a=min(max(plist[0].integer,0),prglen);
    o=a+1;
  }
  if(is_bytecode && programbufferlen>sizeof(BYTECODE_HEADER)-2) {
    BYTECODE_HEADER *h=(BYTECODE_HEADER *)programbuffer;
    printf("Bytecode: %s (%d Bytes) Version: %04x\n",ifilename,programbufferlen,h->version);
    printf("Info:\n");
    printf("  Size of   Text-Segment: %d\n",(int)h->textseglen);
    printf("  Size of roData-Segment: %d\n",(int)h->rodataseglen);
    printf("  Size of   Data-Segment: %d\n",(int)h->sdataseglen);
    printf("  Size of    bss-Segment: %d\n",(int)h->bssseglen);
    printf("  Size of String-Segment: %d\n",(int)h->stringseglen);
    printf("  Size of Symbol-Segment: %d (%d symbols)\n",(int)h->symbolseglen,(int)h->symbolseglen/sizeof(BYTECODE_SYMBOL));
  } else {
    for(i=a;i<o;i++) printf("%s\n",program[i]);
  }
}

char *plist_paramter(PARAMETER *p) {
  static char ergebnis[256];
  switch(p->typ) {
  case PL_EVAL:
  case PL_KEY:
    strcpy(ergebnis,p->pointer);
    break;
  case PL_LEER:
    *ergebnis=0;
    break;
  case PL_FLOAT:
  case PL_NUMBER:
    sprintf(ergebnis,"%g",p->real);
    break;
  case PL_INT:
    sprintf(ergebnis,"%d",p->integer);
    break;
  case PL_FILENR:
    sprintf(ergebnis,"#%d",p->integer);
    break;
  case PL_STRING:
    sprintf(ergebnis,"\"%s\"",(char *)p->pointer);
    break;
  case PL_LABEL:
    strcpy(ergebnis,labels[p->integer].name);
    break;
  case PL_IVAR:
    sprintf(ergebnis,"%s%%",variablen[p->integer].name);
    break;
  case PL_SVAR:
    sprintf(ergebnis,"%s$",variablen[p->integer].name);
    break;
  case PL_FVAR:
    strcpy(ergebnis,variablen[p->integer].name);
    break;
  case PL_NVAR:
  case PL_VAR:
  case PL_ALLVAR:
  case PL_ARRAYVAR:
    switch(variablen[p->integer].typ) {
    case INTTYP:
      sprintf(ergebnis,"%s%%",variablen[p->integer].name);
      break;
    case FLOATTYP:
      sprintf(ergebnis,"%s",variablen[p->integer].name);
      break;
    case STRINGTYP:
      sprintf(ergebnis,"%s$",variablen[p->integer].name);
      break;
    case ARRAYTYP:
      switch(p->arraytyp) {
      case INTTYP:
        sprintf(ergebnis,"%s%%()",variablen[p->integer].name);
        break;
      case FLOATTYP:
        sprintf(ergebnis,"%s()",variablen[p->integer].name);
        break;
      case STRINGTYP:
        sprintf(ergebnis,"%s$()",variablen[p->integer].name);
        break;
      default:
        sprintf(ergebnis,"%s?()",variablen[p->integer].name);
        break;
      }
      break;
    }
  default: 
    sprintf(ergebnis,"<ups$%x>",p->typ);
  }
  if(p->panzahl) {
    strcat(ergebnis,"(");
        char *buf;
	int i;
        for(i=0;i<p->panzahl;i++) {
          buf=plist_paramter(&(p->ppointer[i]));
          strcat(ergebnis,buf);
	  free(buf);
	  if(i<p->panzahl-1) strcat(ergebnis,",");
        }
    
    strcat(ergebnis,")");
  }
  return(strdup(ergebnis));
}

char *plist_zeile(P_CODE *code) {
  char *ergebnis=malloc(MAXLINELEN);
  int i;
  ergebnis[0]=0;
  if(code->opcode&P_INVALID) strcat(ergebnis,"==> ");
  if(code->opcode==P_REM) {
    strcat(ergebnis,"' ");
    strcat(ergebnis,code->argument);
    return(ergebnis);
  }
  if(code->opcode==(P_IGNORE|P_NOCMD)) return(ergebnis);
  
  if(code->opcode==P_LABEL) {
    strcat(ergebnis,labels[code->integer].name);
    strcat(ergebnis,":");
  } else if((code->opcode)&P_EVAL) {
    strcat(ergebnis,"eval ---> "); 
    strcat(ergebnis,code->argument);
  } else if((code->opcode&P_ZUWEIS)==P_ZUWEIS) {
    sprintf(ergebnis,"zuweis %d ---> %s",code->integer,code->argument); 
    
  } else if((code->opcode&PM_COMMS)<anzcomms) {
    strcat(ergebnis,comms[(code->opcode&PM_COMMS)].name);
    if((code->opcode&PM_SPECIAL)==P_ARGUMENT) strcat(ergebnis,code->argument);
    else if((code->opcode&PM_SPECIAL)==P_PLISTE) {
      if(code->panzahl) {
        char *buf;
        strcat(ergebnis," ");
      
        for(i=0;i<code->panzahl;i++) {
          buf=plist_paramter(&(code->ppointer[i]));
          strcat(ergebnis,buf);
	  free(buf);
	  if(i<code->panzahl-1) strcat(ergebnis,",");
        }
      }
    }
  } else sprintf(ergebnis,"=?=> %d",(int)code->opcode);
  if(code->etyp==PE_COMMENT) {
    strcat(ergebnis," !");
    strcat(ergebnis,(char *)code->extra);
  }
  return(ergebnis);
}
int plist_printzeile(P_CODE *code, int level) {
  int j;
  char *zeile=plist_zeile(code);
  if(code->opcode & P_LEVELOUT) level--;
  for(j=0;j<level;j++) printf("  ");
  printf("%s\n",zeile);
  if(code->opcode & P_LEVELIN) level++;
  free(zeile);
  return(level);
}

void c_plist(char *n) {
  int i,f=0;
  for(i=0;i<prglen;i++) { 
    printf("%4d: $%06x |",i,(unsigned int)pcode[i].opcode);
  #if DEBUG
    printf(" (%5d) |",ptimes[i]);
  #endif
    printf(" %3d,%d |",pcode[i].integer,pcode[i].panzahl);
    f=plist_printzeile(&pcode[i],f);
  }
}

void c_load(PARAMETER *plist, int e) { 
  programbufferlen=prglen=pc=sp=0;
  c_merge(plist,e); 
}
void c_save(PARAMETER *plist, int e) { 
  if(programbufferlen) {
    char *name;
    if(e) name=plist[0].pointer;
    else name=ifilename;
    if(strlen(name)==0) name=ifilename;
    if(exist(name)) {
      char buf[100];
      sprintf(buf,"mv %s %s.bak",name,name);
      if(system(buf)==-1) io_error(errno,"system");
    }
    saveprg(name);
  }
}

void c_merge(PARAMETER *plist, int e){
  if(exist(plist[0].pointer)) {
    if(programbufferlen==0) strcpy(ifilename,plist[0].pointer);
    mergeprg(plist[0].pointer);
  } else printf("LOAD/MERGE: Datei %s nicht gefunden !\n",(char *)plist[0].pointer);
}

void c_new(char *n) {
  erase_all_variables();
  batch=0;
  programbufferlen=prglen=pc=sp=0;
  strcpy(ifilename,"new.bas");
  graphics_setdefaults();
}
void c_let(char *n) {  
    char v[strlen(n)+1],w[strlen(n)+1];
    wort_sep(n,'=',TRUE,v,w);
    xzuweis(v,w);
}


void c_quit(PARAMETER *plist, int e) { 
  int ecode=0;
  if(e) ecode=plist[0].integer; 
  quit_x11basic(ecode); 
}

void quit_x11basic(int c) {
#ifdef ANDROID
  invalidate_screen();
  sleep(1);
#endif
#ifndef NOGRAPHICS
  close_window(usewindow); 
#endif
  free(programbuffer);
#ifdef CONTROL
  cs_exit();
#endif
  exit(c); 
}

/* Linearer Fit (regression) optional mit Fehlerbalken in x und y Richtung  */

void c_fit_linear(char *n) {  
  char w1[strlen(n)+1],w2[strlen(n)+1];                  
  int e,typ,scip=0,i=0,mtw=0;  
  int vnrx,vnry,vnre,vnre2,ndata; 
  double a,b,siga,sigb,chi2,q;
  char *r;
  e=wort_sep(n,',',TRUE,w1,w2);
  while(e) {
    scip=0;
    if(strlen(w1)) {
       switch(i) {
         case 0: { /* Array mit x-Werten */     
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnrx=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnrx==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else puts("FIT: Kein ARRAY.");
	   break;
	   }
	 case 1: {   /* Array mit y-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnry=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnry==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else puts("FIT: Kein ARRAY.");
	   break;
	   } 
	 case 2: {   /* Array mit err-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnre=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnre==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	     else mtw=1;
	   } else {scip=1; mtw=0;}
	   break;
	   } 
	 case 3: {   /* Array mit err-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnre2=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnre2==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	     else mtw=2;
	   } else {scip=1;}
	   break;
	   } 
	 case 4: {
	   ndata=(int)parser(w1); 
	   if(vnrx!=-1 && vnry!=-1) {
             if(mtw==2 && vnre!=-1 && vnre2!=-1) {
	       linear_fit_exy((double *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE),ndata,
		   (double *)(variablen[vnre].pointer.a->pointer+variablen[vnre].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnre2].pointer.a->pointer+variablen[vnre2].pointer.a->dimension*INTSIZE),
		   &a,&b,&siga,&sigb,&chi2,&q); 

	     } else {
	       linear_fit((double *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE),ndata,(mtw)?(
		   (double *)(variablen[vnre].pointer.a->pointer+variablen[vnre].pointer.a->dimension*INTSIZE)):NULL,mtw,&a,&b,&siga,&sigb,&chi2,&q); 
             }
	   }
	   break; 
	 } 
	 case 5: { zuweis(w1,a); break; } 
	 case 6: { zuweis(w1,b); break;} 
	 case 7: { zuweis(w1,siga); break;} 
	 case 8: { zuweis(w1,sigb);  break;} 
	 case 9: { zuweis(w1,chi2);  break;} 
	 case 10: { zuweis(w1,q);  break;} 
	   
         default: break;
       }
    }
    if(scip==0) e=wort_sep(w2,',',TRUE,w1,w2);
    i++;
  }
}

/* Sort-Funktion (wie qsort() ), welche ausserdem noch ein integer-Array mitsortiert */

void do_sort(void *a, size_t n,size_t size,int(*compar)(const void *, const void *), int *b) {
 // printf("sort: n=%d size=%d\n",n,size);
  if (n<2) return;
  if(b==NULL) qsort(a,n,size,compar);
  else { 
    void *rra=malloc(size);
    unsigned long i,ir,j,l;
    int index;

  l=(n>>1)+1;
  ir=n;
  for(;;) {
    if(l>1) {
      memcpy(rra,a+size*(l-2),size);
      l--;
      index=b[l-1];
    } else {
      memcpy(rra,a+size*(ir-1),size);
      index=b[ir-1];
      memcpy(a+size*(ir-1),a+size*(1-1),size);
      b[ir-1]=b[1-1];
      if (--ir==1) {
        memcpy(a,rra,size);
        *b=index;
        break;
      }
    }
    i=l;j=l+l;
    while(j<=ir) {
      if(j<ir && compar(a+size*(j-1),a+size*j)<0) j++;
      if(compar(rra,a+size*(j-1))<0) {
	memcpy(a+size*(i-1),a+size*(j-1),size); 
	b[i-1]=b[j-1];
	i=j;
	j<<=1;
      } else j=ir+1;
    }
    memcpy(a+size*(i-1),rra,size);
    b[i-1]=index;
  }
  free(rra);
  }
}


/*The sort functions for all variable types */
static int cmpstring(const void *p1, const void *p2) {
 // printf("cmpstring\n");
  return(memcmp(((STRING *)p1)->pointer,((STRING *)p2)->pointer,min(((STRING *)p1)->len,((STRING *)p2)->len)));
}
static int cmpdouble(const void *p1, const void *p2) {
  if(*(double *)p1==*(double *)p2) return(0);
  else if(*(double *)p1>*(double *)p2) return(1);
  else return(-1);
}
static int cmpint(const void *p1, const void *p2) {
  if((*(int *)p1)==(*(int *)p2)) return(0);
  else if((*(int *)p1)>(*(int *)p2)) return(1);
  else return(-1);
}

/* Sortierfunktion fuer ARRAYS 

Todo: 
* Umstellen auf pliste.
* Stringsortierung bei unterschiedlicher Laenge ist nicht optimal.
* Indexarray muss INTARRAYTYP sein. Das geht auch flexibler! (mit allarray)

*/



void c_sort(PARAMETER *plist,int e) {  
  int typ,subtyp;
  int vnrx,vnry=-1,ndata=0; 
  char *r;

  vnrx=plist[0].integer;
  ndata=anz_eintraege(variablen[vnrx].pointer.a);

  if(e>=2) ndata=plist[1].integer;
  if(e>=3) vnry=plist[2].integer;
  typ=variablen[vnrx].typ;
  subtyp=variablen[vnrx].pointer.a->typ;
  
 //  printf("c_sort vnr=%d ndata=%d vnry=%d\n",vnrx,ndata,vnry);


  if(subtyp==STRINGTYP) 
    do_sort((void *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE)
      ,ndata,sizeof(STRING),cmpstring,
      (int *)((vnry!=-1)?(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE):NULL));      
  else if(subtyp==INTTYP) 
      do_sort((void *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE)
      ,ndata,sizeof(int),cmpint,
      (int *)((vnry!=-1)?(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE):NULL));      
  else if(subtyp==FLOATTYP)  
      do_sort((void *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE)
      ,ndata,sizeof(double),cmpdouble,
      (int *)((vnry!=-1)?(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE):NULL));
}

/* Allgemeine Fit-Funktion  mit Fehlerbalken in y Richtung  */

void c_fit(char *n) {  
  char w1[strlen(n)+1],w2[strlen(n)+1];                  
  int e,typ,scip=0,i=0,mtw=0;  
  int vnrx,vnry,vnre,vnre2,ndata; 
  double a,b,siga,sigb,chi2,q;
  char *r;
  e=wort_sep(n,',',TRUE,w1,w2);
  xberror(9,"FIT"); /* Funktion noch nicht moeglich */
  while(e) {
    scip=0;
    if(strlen(w1)) {
       switch(i) {
         case 0: { /* Array mit x-Werten */     
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnrx=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnrx==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else puts("FIT: Kein ARRAY.");
	   break;
	   }
	 case 1: {   /* Array mit y-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnry=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnry==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else puts("FIT: no ARRAY.");
	   break;
	   } 
	 case 2: {   /* Array mit err-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnre=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnre==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	     else mtw=1;
	   } else {scip=1; mtw=0;}
	   break;
	   } 
	 case 4: {
	   ndata=(int)parser(w1); 
           break;
	   }
	 case 5: {   /* Funktion mit Parameterliste */
	   }
	 case 6: {   /* Ausdruck, der Angibt, welche Parameter zu fitten sind */  	 
	   if(vnrx!=-1 && vnry!=-1) {
             if(mtw==2 && vnre!=-1 && vnre2!=-1) {
	       linear_fit_exy((double *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE),ndata,
		   (double *)(variablen[vnre].pointer.a->pointer+variablen[vnre].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnre2].pointer.a->pointer+variablen[vnre2].pointer.a->dimension*INTSIZE),
		   &a,&b,&siga,&sigb,&chi2,&q); 

	     } else {
	       linear_fit((double *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE),ndata,(vnre!=-1)?(
		   (double *)(variablen[vnre].pointer.a->pointer+variablen[vnre].pointer.a->dimension*INTSIZE)):NULL,mtw,&a,&b,&siga,&sigb,&chi2,&q); 
             }
	   }
	   break; 
	 } 
	 case 7: { zuweis(w1,chi2); break; } 
	 case 8: { zuweis(w1,b); break;} 
	 case 9: { zuweis(w1,siga); break;} 
	 case 10: { zuweis(w1,sigb);  break;} 
	 case 11: { zuweis(w1,chi2);  break;} 
	 case 12: { zuweis(w1,q);  break;}	   
         default: break;
       }
    }
    if(scip==0) e=wort_sep(w2,',',TRUE,w1,w2);
    i++;
  }
}

void c_fft(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int isign=1;
  int e=wort_sep(n,',',TRUE,v,w);
  if(e>=1) {
    int typ,vnr;
    char *r;
     /* Typ bestimmem. Ist es Array ? */
 
    typ=type(v)&(~CONSTTYP);
    if(typ & ARRAYTYP) {
      r=varrumpf(v);
      vnr=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
      free(r);
      if(vnr==-1) xberror(15,v); /* Feld nicht dimensioniert */ 
      else {
        if(typ & FLOATTYP) {
	  int nn=do_dimension(&variablen[vnr]);
	  double *varptr=(double  *)(variablen[vnr].pointer.a->pointer+variablen[vnr].pointer.a->dimension*INTSIZE);

	  if(e==2) isign=(int)parser(w);
	  realft(varptr,(nn-1)/2,isign);
        } else puts("FFT: Muss Float-ARRAY sein.");
      }
    } else puts("FFT: Kein ARRAY.");
  
  } else xberror(32,"FFT"); /* Syntax error */
}


void c_arraycopy(PARAMETER *plist,int e) {
  int vnr1=plist[0].integer;
  int vnr2=plist[1].integer;
  ARRAY *arr1=variablen[vnr1].pointer.a;
  ARRAY *arr2=variablen[vnr2].pointer.a;
  ARRAY a;
  switch(arr1->typ) {
  case INTTYP:
      if(arr2->typ==INTTYP) a=double_array(arr2);
      else if(arr2->typ==FLOATTYP) a=convert_to_intarray(arr2);
      else printf("ARRAYCOPY: ARRAY hat falschen typ.\n");
      break;
  case FLOATTYP:
      if(arr2->typ==FLOATTYP) a=double_array(arr2);
      else if(arr2->typ==INTTYP) a=convert_to_floatarray(arr2);
      else printf("ARRAYCOPY: ARRAY hat falschen typ.\n");
      break;
  case STRINGTYP:
      if(arr2->typ==STRINGTYP) a=double_array(arr2);
      else printf("ARRAYCOPY: ARRAY hat falschen typ.\n");
      break;
  default:
    printf("ERROR: arraycopy : typ? $%x \n",variablen[vnr1].typ);
  }
  free_array(arr1);
  *arr1=a;
}

void c_arrayfill(PARAMETER *plist,int e) {
  int vnr=plist[0].integer;
  int typ;
//  printf("ARRAYFILL: vnr=%d\n",vnr);
  ARRAY *arr=variablen[vnr].pointer.a;
  ARRAY a;
  
  switch(arr->typ) {
  case INTTYP:
    if(plist[1].typ==PL_FLOAT) plist[1].integer=(int)plist[1].real;
    else if(plist[1].typ!=PL_INT) printf("ERROR: ARRAYFILL: falscher typ\n");
    a=create_int_array(arr->dimension,arr->pointer,plist[1].integer);
    break;
  case FLOATTYP:
    if(plist[1].typ==PL_INT) plist[1].real=(double)plist[1].integer;
    else if(plist[1].typ!=PL_FLOAT) printf("ERROR: ARRAYFILL: falscher typ\n");
    a=create_float_array(arr->dimension,arr->pointer,plist[1].real);
    break;
  case STRINGTYP:
    if(plist[1].typ!=PL_STRING) printf("ERROR: ARRAYFILL: falscher typ\n");
    a=create_string_array(arr->dimension,arr->pointer,(STRING *)&(plist[1].integer));
    break;  
  default:
    printf("ERROR: arrayfill : typ? $%x var is not an array\n",variablen[vnr].typ);
  }
  free_array(arr);
  *arr=a;
}
void c_memdump(PARAMETER *plist,int e) {
  memdump((unsigned char *)plist[0].integer,plist[1].integer);
}

char *varinfo(VARIABLE *v) {
  static char info[128];
  char *buf;
  char a;
  int i=0;
  switch(v->typ) {
    case INTTYP:   sprintf(info,"%s%%=%d",v->name,*(v->pointer.i));break;
    case FLOATTYP: sprintf(info,"%s=%.13g",v->name,*(v->pointer.f)); break;
    case STRINGTYP:
      buf=malloc(v->pointer.s->len+1);
      while(i<v->pointer.s->len && i<80) {
        a=(v->pointer.s->pointer)[i];
        if(isprint(a)) buf[i]=a;
	else buf[i]='.';
        i++;
      } 
      buf[i]=0;
      sprintf(info,"%s$=\"%s\" (len=%d)",v->name,buf,v->pointer.s->len);
      free(buf);
      break;
    default:
      sprintf(info,"?_var_?=?_? ");
  }
  return(info);
}

void c_dump(PARAMETER *plist,int e) {
  int i;
  char kkk=0;
  
  if(e) kkk=((char *)plist[0].pointer)[0];

  if(kkk==0 || kkk=='%') {/*  dump ints */
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==INTTYP) 
        printf("%02d: %s\n",i,varinfo(&variablen[i]));
    }
  }
  if(kkk==0) {
    for(i=0;i<anzvariablen;i++) {/*  dump floats */
      if(variablen[i].typ==FLOATTYP) 
        printf("%02d: %s\n",i,varinfo(&variablen[i]));
    }
  }
  if(kkk==0 || kkk=='$') {/*  dump strings */
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==STRINGTYP) {
        printf("%02d: %s\n",i,varinfo(&variablen[i]));
      }
    }
  }
  if(kkk==0 || kkk=='%' || kkk=='(') {/*  dump int arrays */
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==ARRAYTYP && variablen[i].pointer.a->typ==INTTYP) {
        int j;
        printf("%02d: %s%%(",i,variablen[i].name);
        for(j=0;j<variablen[i].pointer.a->dimension;j++) {
          if(j>0) printf(",%d",((int *)variablen[i].pointer.a->pointer)[j]);
	  else  printf("%d",((int *)variablen[i].pointer.a->pointer)[j]);
        }
        puts(")");
      }
    }
  }
  
  if(kkk==0 || kkk=='(') {/*  dump arrays */
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==ARRAYTYP && variablen[i].pointer.a->typ==FLOATTYP) {
        int j;
        printf("%02d: %s(",i,variablen[i].name);
        for(j=0;j<variablen[i].pointer.a->dimension;j++) {
          if(j>0) printf(",%d",((int *)variablen[i].pointer.a->pointer)[j]);
  	  else  printf("%d",((int *)variablen[i].pointer.a->pointer)[j]);
        }
        printf(")  [%d]\n",variablen[i].local);
      }
    }
  }
  if(kkk==0 || kkk=='$' || kkk=='(') {/*  dump string arrays */
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==ARRAYTYP && variablen[i].pointer.a->typ==STRINGTYP) {
        int j;
        printf("%02d: %s$(",i,variablen[i].name);
        for(j=0;j<variablen[i].pointer.a->dimension;j++) {
          if(j>0) printf(",%d",((int *)variablen[i].pointer.a->pointer)[j]);
  	  else  printf("%d",((int *)variablen[i].pointer.a->pointer)[j]);
        }
        puts(")");
      }
    }
  }
  if(kkk==':') {/*  dump Labels */
    for(i=0;i<anzlabels;i++) {
      printf("%s: [%d]\n",labels[i].name,labels[i].zeile);
    }
  }
  if(kkk=='@') {/*  dump Procedures */
    for(i=0;i<anzprocs;i++) {
      printf("%d|%s [%d]\n",procs[i].typ,procs[i].name,procs[i].zeile);
    }
  }
  if(kkk=='#') {                   /*  dump Channels */
    for(i=0;i<ANZFILENR;i++) {
      if(filenr[i]==1) {
        printf("#%d: %s [FILE]\n",i,"");
      } else if(filenr[i]==2) {
        printf("#%d: %s [SHARED OBJECT]\n",i,"");
      }
    }
  }
  if(kkk=='C' || kkk=='K'|| kkk=='c'|| kkk=='k') { /*  dump commands */
    int j;
    for(i=0;i<anzcomms;i++) {
      printf("%3d: [%08x] %s ",i,(unsigned int)comms[i].opcode,comms[i].name);  
      if(comms[i].pmin) {
        for(j=0;j<comms[i].pmin;j++) {
	  switch(comms[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<comms[i].pmin-1) printf(",");
	}
      }
      if(comms[i].pmax>comms[i].pmin || comms[i].pmax==-1) printf("[,");
      if(comms[i].pmax==-1) printf("...");
      else {
      for(j=comms[i].pmin;j<comms[i].pmax;j++) {
	  switch(comms[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<comms[i].pmax-1) printf(",");
	}
      }
      if(comms[i].pmax>comms[i].pmin || comms[i].pmax==-1) printf("]");
      printf("\n");
    }
  }
  if(kkk=='F' || kkk=='f') { /*  dump functions */
    int j;
    for(i=0;i<anzpfuncs;i++) {
      printf("%3d: [%08x] %s(",i,(unsigned int) pfuncs[i].opcode,pfuncs[i].name);  
      if(pfuncs[i].pmin) {
        for(j=0;j<pfuncs[i].pmin;j++) {
	  switch(pfuncs[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<pfuncs[i].pmin-1) printf(",");
	}
      }
      if(pfuncs[i].pmax>pfuncs[i].pmin || pfuncs[i].pmax==-1) printf("[,");
      if(pfuncs[i].pmax==-1) printf("...");
      else {
      for(j=pfuncs[i].pmin;j<pfuncs[i].pmax;j++) {
	  switch(pfuncs[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<pfuncs[i].pmax-1) printf(",");
	}
      }
      if(pfuncs[i].pmax>pfuncs[i].pmin || pfuncs[i].pmax==-1) printf("]");
      printf(")\n");
    }    
    for(i=0;i<anzpsfuncs;i++) {
      printf("%3d: [%08x] %s(",i,(unsigned int)psfuncs[i].opcode,psfuncs[i].name);  
      if(psfuncs[i].pmin) {
        for(j=0;j<psfuncs[i].pmin;j++) {
	  switch(psfuncs[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<psfuncs[i].pmin-1) printf(",");
	}
      }
      if(psfuncs[i].pmax>psfuncs[i].pmin || psfuncs[i].pmax==-1) printf("[,");
      if(psfuncs[i].pmax==-1) printf("...");
      else {
      for(j=psfuncs[i].pmin;j<psfuncs[i].pmax;j++) {
	  switch(psfuncs[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<psfuncs[i].pmax-1) printf(",");
	}
      }
      if(psfuncs[i].pmax>psfuncs[i].pmin || psfuncs[i].pmax==-1) printf("]");
      printf(")\n");
    }
  }
}

void c_end(char *n) { batch=0; }

void c_on(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1],w3[strlen(n)+1];
  int e=wort_sep(n,' ',TRUE,w1,w2);
  int mode=0;
  if(e==0) xberror(32,"ON"); /* Syntax error */
  else {
    wort_sep(w2,' ',TRUE,w2,w3);
    if(strcmp(w2,"CONT")==0) mode=1;
    else if(strcmp(w2,"GOTO")==0) mode=2;
    else if(strcmp(w2,"GOSUB")==0) mode=3;
    else mode=0;
    
    if(strcmp(w1,"ERROR")==0) {
      errcont=(mode>0);
      if(mode==2) errorpc=labelzeile(w3);
      else if(mode==3) {
        errorpc=procnr(w3,1);
	if(errorpc!=-1) errorpc=procs[errorpc].zeile;      
      }
    } else if(strcmp(w1,"BREAK")==0) {
      breakcont=(mode>0);
      if(mode==2) breakpc=labelzeile(w3);
      else if(mode==3) {
        breakpc=procnr(w3,1);
	if(breakpc!=-1) breakpc=procs[breakpc].zeile;
      }
#ifndef NOGRAPHICS 
    } else if(strcmp(w1,"MENU")==0) {
      if(mode==0)  c_menu("");  
      else if(mode==3) {
       int pc2=procnr(w3,1);
       if(pc2==-1) xberror(19,w3); /* Procedure nicht gefunden */
       else menuaction=pc2;
      } else  printf("Unbekannter Befehl: ON <%s> <%s>\n",w1,w2);  
#endif
    } else { /* on n goto ...  */
      if(mode<2) printf("Unbekannter Befehl: ON <%s> <%s>\n",w1,w2);
      else {
        int gi=max(0,(int)parser(w1));
	if(gi) {
	  while(gi) { e=wort_sep(w3,',',TRUE,w2,w3); gi--;}
	  if(e) {
            if(mode==3) c_gosub(w2);
	    else if(mode==2) c_goto(w2);
          }
	}
      }
    }
  }
}

void c_add(PARAMETER *plist,int e) {
  int vnr=plist[0].integer;
  char *varptr=plist[0].pointer;
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
  switch(typ) {
  case INTTYP:
    if(plist[1].typ==PL_INT) {
      *((int *)varptr)+=plist[1].integer;
    } else if(plist[1].typ==PL_FLOAT) {
      *((int *)varptr)+=(int)plist[1].real;    
    } else printf("ADD. Argument wrong, must be flt.\n");
    break;
  case FLOATTYP:
    if(plist[1].typ==PL_FLOAT) {
      *((double *)varptr)+=plist[1].real;
    } else if(plist[1].typ==PL_INT) {
      *((double *)varptr)+=(double)plist[1].integer;    
    } else printf("ADD. Argument wrong, must be flt.\n");
    break;
  case STRINGTYP:
    if(plist[1].typ==PL_STRING) {
      STRING *s=(STRING *)varptr;
      s->pointer=realloc(s->pointer,s->len+plist[1].integer+1);
      memcpy(s->pointer+s->len,plist[1].pointer,plist[1].integer);
      s->len+=plist[1].integer;
      (s->pointer)[s->len]=0;
    } else printf("ADD. Argument wrong, must be string.\n");
    break;
  default:
    printf("ADD: typ unbek. $%x\n",typ);
    xberror(32,""); /* Syntax error */
  }
}
void c_sub(PARAMETER *plist,int e) {
  int vnr=plist[0].integer;
  char *varptr=plist[0].pointer;
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
//  printf("SUB: vnr=%d\n",vnr);
//  dump_parameterlist(plist,e);
  switch(typ) {
  case INTTYP:
    if(plist[1].typ==PL_INT) {
      *((int *)varptr)-=plist[1].integer;
    } else if(plist[1].typ==PL_FLOAT || plist[1].typ==PL_NUMBER) {
      *((int *)varptr)-=(int)plist[1].real;    
    } else printf("SUB. Argument wrong, must be flt.\n");
    break;
  case FLOATTYP:
    if(plist[1].typ==PL_FLOAT || plist[1].typ==PL_NUMBER) {
      *((double *)varptr)-=plist[1].real;
    } else if(plist[1].typ==PL_INT) {
      *((double *)varptr)-=(double)plist[1].integer;    
    } else printf("SUB. Argument wrong, must be flt.\n");
    break;
  default:
    xberror(32,""); /* Syntax error */
  }
}
void c_mul(PARAMETER *plist,int e) {
  int vnr=plist[0].integer;
  char *varptr=plist[0].pointer;
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
//  printf("MUL: vnr=%d\n",vnr);
//  dump_parameterlist(plist,e);
  switch(typ) {
  case INTTYP:
    if(plist[1].typ==PL_INT) {
      *((int *)varptr)*=plist[1].integer;
    } else if(plist[1].typ==PL_FLOAT || plist[1].typ==PL_NUMBER) {
      *((int *)varptr)*=(int)plist[1].real;    
    } else printf("MUL. Argument wrong, must be flt.\n");
    break;
  case FLOATTYP:
    if(plist[1].typ==PL_FLOAT || plist[1].typ==PL_NUMBER) {
      *((double *)varptr)*=plist[1].real;
    } else if(plist[1].typ==PL_INT) {
      *((double *)varptr)*=(double)plist[1].integer;    
    } else printf("MUL. Argument wrong, must be flt.\n");
    break;
  default:
    xberror(32,""); /* Syntax error */
  }
}
void c_div(PARAMETER *plist,int e) {
  int vnr=plist[0].integer;
  char *varptr=plist[0].pointer;
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
//  printf("DIV: vnr=%d\n",vnr);
//  dump_parameterlist(plist,e);
  switch(typ) {
  case INTTYP:
    if(plist[1].typ==PL_INT) {
      *((int *)varptr)/=plist[1].integer;
    } else if(plist[1].typ==PL_FLOAT || plist[1].typ==PL_NUMBER) {
      *((int *)varptr)/=(int)plist[1].real;    
    } else printf("DIV. Argument wrong, must be flt.\n");
    break;
  case FLOATTYP:
    if(plist[1].typ==PL_FLOAT || plist[1].typ==PL_NUMBER) {
      *((double *)varptr)/=plist[1].real;
    } else if(plist[1].typ==PL_INT) {
      *((double *)varptr)/=(double)plist[1].integer;    
    } else printf("DIV. Argument wrong, must be flt.\n");
    break;
  default:
    xberror(32,""); /* Syntax error */
  }
}



void c_swap(PARAMETER *plist,int e) {
  VARIABLE tmp;
  char *zb;
  int vnr1,vnr2;
  vnr1=plist[0].integer;
  vnr2=plist[1].integer;
  // printf("SWAP: %d %d\n",vnr1,vnr2);
  if(vnr1==vnr2 && plist[0].pointer==plist[1].pointer) return;
  if(plist[0].typ!=plist[1].typ) {
  printf("$%x $%x",variablen[plist[0].integer].typ,variablen[plist[1].integer].typ);
     xberror(58,""); /* Variable %s ist vom falschen Typ */
     return;
   }
//   switch(typ) {
/* Was ist, wenn wir eine variable mit einem Arrayindex vertauschen ?*/     
//   default:
     tmp=variablen[plist[0].integer];
     zb=variablen[plist[0].integer].name;
     variablen[plist[0].integer]=variablen[plist[1].integer];
     variablen[plist[0].integer].name=zb;
     zb=variablen[plist[1].integer].name;
     variablen[plist[1].integer]=tmp;     
     variablen[plist[1].integer].name=zb;
//   }
} 

void c_do(char *n) {   /* wird normalerweise ignoriert */
  if(*n==0) ; 
  else if(strncmp(n,"WHILE",5)==0) c_while(n);
  else if(strncmp(n,"UNTIL",5)==0) ;
  else  printf("Unbekannter Befehl: DO <%s>\n",n);
}

void c_dim(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int p=wort_sep(n,',',TRUE,w,v);
  while(p) {
     xtrim(w,TRUE,w);
     c_dodim(w);
     p=wort_sep(v,',',TRUE,w,v); 
  }
}
void c_erase(PARAMETER *plist,int e) {
  while(e) erase_variable(&variablen[plist[--e].integer]);
}



void c_return(char *n) {
  if(sp>0) {
    if(n && strlen(n)) {
      if(type(n) & STRINGTYP) {
        returnvalue.str=string_parser(n); /* eigentlich muss auch der Funktionstyp 
                                       	 abgefragt werden */
      } else returnvalue.f=parser(n);
    }
    restore_locals(sp);
    pc=stack[--sp];
  }
  else xberror(93,""); /*Stack-Error !*/
}

void c_void(char *n) { 
  if(type(n) & STRINGTYP) {
    char *erg=s_parser(n);
    free(erg);
  } else parser(n);
}
void c_nop(char *n) { return; }

void c_inc(PARAMETER *plist,int e) {
  int typ=variablen[plist->integer].typ;
  if(typ==ARRAYTYP) typ=variablen[plist->integer].pointer.a->typ;
  if(typ&FLOATTYP) (*((double *)plist->pointer))++;
  else if(typ&INTTYP) (*((int *)plist->pointer))++;
//    else printf("INC seltsam. $%x\n",variablen[plist->integer].typ);
}

void c_dec(PARAMETER *plist,int e) { 
  int typ=variablen[plist->integer].typ;
  if(typ==ARRAYTYP)   typ=variablen[plist->integer].pointer.a->typ;
  if(typ&FLOATTYP)    (*((double *)plist->pointer))--;
  else if(typ&INTTYP) (*((int *)plist->pointer))--;
}
void c_cls(char *n) { 
#ifdef WINDOWS
  DWORD written; /* number of chars actually written */
  COORD coord; /* coordinates to start writing */
  CONSOLE_SCREEN_BUFFER_INFO coninfo; /* receives console size */ 
  HANDLE ConsoleOutput; /* handle for console output */
  ConsoleOutput=GetStdHandle(STD_OUTPUT_HANDLE); 
  GetConsoleScreenBufferInfo(ConsoleOutput,&coninfo);
#define  COLS coninfo.dwSize.X
#define  LINES coninfo.dwSize.Y
  coord.X=0;
  coord.Y=0;
  FillConsoleOutputCharacter(ConsoleOutput,' ',LINES*COLS,
    coord,&written);
  FillConsoleOutputAttribute(ConsoleOutput,
    FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,LINES*COLS,
    coord,&written);

  SetConsoleCursorPosition(ConsoleOutput,coord);
#else
  printf("\033[2J\033[H");
#endif
}
void c_home(char *n) { 
#ifdef WINDOWS
  COORD coord;
  HANDLE ConsoleOutput; /* handle for console output */
  ConsoleOutput=GetStdHandle(STD_OUTPUT_HANDLE); 
  coord.X=0;
  coord.Y=0;
  SetConsoleCursorPosition(ConsoleOutput,coord);
#else
  printf("\033[H");
#endif
}
void c_version(char *n) { printf("X11-BASIC Version: %s %s\n",version,vdate);}

#ifndef WINDOWS
#include <fnmatch.h>
#else
#include "Windows.extension/fnmatch.h"
#endif
void c_help(PARAMETER *plist,int e) {
  if(e==0) puts("HELP [topic]");
  else do_help(plist[0].pointer);
}  
void do_help(char *w) {
    int j,i;
    for(i=0;i<anzcomms;i++) {
    
      if(fnmatch(w,comms[i].name,FNM_NOESCAPE)==0) {
        printf("%s ",comms[i].name);  
        if(comms[i].pmin) {
          for(j=0;j<comms[i].pmin;j++) {
	    switch(comms[i].pliste[j]) {
	      case PL_INT: printf("i%%"); break;
	      case PL_FILENR: printf("#n"); break;
	      case PL_STRING: printf("t$"); break;
	      case PL_NUMBER: printf("num"); break;
	      case PL_SVAR: printf("var$"); break;
	      case PL_IVAR: printf("var%%"); break;
	      case PL_NVAR: printf("var"); break;
	      case PL_KEY: printf("KEY"); break;
	      case PL_FARRAY: printf("a()"); break;
	      case PL_IARRAY: printf("h%%()"); break;
	      case PL_SARRAY: printf("f$()"); break;
	      case PL_LABEL: printf("<label>"); break;
	      case PL_PROC: printf("<procedure>"); break;
	      case PL_FUNC: printf("<function>"); break;
	      default: printf("???");
	    }
	    if(j<comms[i].pmin-1) printf(",");
	  }
        }
        if(comms[i].pmax>comms[i].pmin || comms[i].pmax==-1) printf("[,");
        if(comms[i].pmax==-1) printf("...");
        else {
        for(j=comms[i].pmin;j<comms[i].pmax;j++) {
	    switch(comms[i].pliste[j]) {
	      case PL_INT: printf("i%%"); break;
	      case PL_FILENR: printf("#n"); break;
	      case PL_STRING: printf("t$"); break;
	      case PL_NUMBER: printf("num"); break;
	      case PL_SVAR: printf("var$"); break;
	      case PL_NVAR: printf("var"); break;
	      case PL_IVAR: printf("var%%"); break;
	      case PL_KEY: printf("KEY"); break;
	      case PL_FARRAY: printf("a()"); break;
	      case PL_IARRAY: printf("h%%()"); break;
	      case PL_SARRAY: printf("f$()"); break;
	      case PL_LABEL: printf("<label>"); break;
	      case PL_PROC: printf("<procedure>"); break;
	      case PL_FUNC: printf("<function>"); break;
	      default: printf("???");
	    }
	    if(j<comms[i].pmax-1) printf(",");
	  }
        }
        if(comms[i].pmax>comms[i].pmin || comms[i].pmax==-1) printf("]");
        puts("");
      }
    }
    for(i=0;i<anzpfuncs;i++) {
      if(fnmatch(w,pfuncs[i].name,FNM_NOESCAPE)==0) {
        printf("%s(",pfuncs[i].name);
	if(pfuncs[i].pmin) printf("%d",pfuncs[i].pmin);
	if(pfuncs[i].pmax-pfuncs[i].pmin) printf("-%d",pfuncs[i].pmax);
        puts(")");
      }
    }
     for(i=0;i<anzpsfuncs;i++) {
      if(fnmatch(w,psfuncs[i].name,FNM_NOESCAPE)==0) {
        printf("%s(",psfuncs[i].name);  
	if(psfuncs[i].pmin) printf("%d",psfuncs[i].pmin);
	if(psfuncs[i].pmax-psfuncs[i].pmin) printf("-%d",psfuncs[i].pmax);
        puts(")");
      }
    }
     for(i=0;i<anzsysvars;i++) {
      if(fnmatch(w,sysvars[i].name,FNM_NOESCAPE)==0) {
        if(sysvars[i].opcode&INTTYP) printf("int ");
	else if(sysvars[i].opcode&FLOATTYP) printf("flt ");
	else printf("??? ");
        printf("%s\n",sysvars[i].name);          
      }
    }
      for(i=0;i<anzsyssvars;i++) {
      if(fnmatch(w,syssvars[i].name,FNM_NOESCAPE)==0) {
        printf("%s\n",syssvars[i].name);          
      }
    }
}
void c_error(PARAMETER *plist,int e) {
  xberror(plist[0].integer,"");
}
void c_free(PARAMETER *plist,int e) {
  free((char *)plist[0].integer);
}
void c_detatch(PARAMETER *plist,int e) {
  int r=shm_detatch(plist[0].integer);
  if(r!=0) io_error(r,"DETATCH");
}
void c_shm_free(PARAMETER *plist,int e) {
  shm_free(plist[0].integer);
}
void c_pause(PARAMETER *plist,int e) {
#ifdef WINDOWS
  Sleep((int)(1000*plist[0].real));
#else
  double zeit=plist[0].real;
  int i=(int)zeit;
  if(i) sleep(i);
  zeit=zeit-(double)i;
  if(zeit>0) usleep((int)(1000000*zeit));
#endif
}

void c_echo(PARAMETER *plist,int e) {
  char *n=plist[0].pointer;
  if(strcmp(n,"ON")==0) echoflag=TRUE; 
  else if(strcmp(n,"OFF")==0) echoflag=FALSE;
  else  echoflag=(int)parser(n);
}
void c_gps(PARAMETER *plist,int e) {
  char *n=plist[0].pointer;
#ifdef ANDROID
  if(strcmp(n,"ON")==0) do_gpsonoff(1);  
  else if(strcmp(n,"OFF")==0) do_gpsonoff(0);
  else do_gpsonoff((int)parser(n));
#endif
}
void c_sensor(PARAMETER *plist,int e) {
  char *n=plist[0].pointer;
#ifdef ANDROID
  if(strcmp(n,"ON")==0) do_sensoronoff(1); 
  else if(strcmp(n,"OFF")==0) do_sensoronoff(0); 
  else  do_sensoronoff((int)parser(n));
#endif
}

void c_stop()  {batch=0;} 
void c_tron()  {echoflag=1;}
void c_troff() {echoflag=0;}
void c_beep()  {putchar('\007');}
 
void c_clear(PARAMETER *plist,int e){clear_all_variables(); graphics_setdefaults();}

void c_clr(PARAMETER *plist,int e){  
//  printf("CLR: pointer=%x\n",(int)plist[0].pointer);
  while(--e>=0) {
 //  printf("clr: typ=$%x\n",plist[e].typ);
    switch(plist[e].typ) {
    case PL_ARRAYVAR: 
    case PL_IARRAYVAR: 
    case PL_FARRAYVAR: 
    case PL_SARRAYVAR: 
      clear_variable(&variablen[plist[e].integer]);
      break;
    case PL_SVAR:
      *(((STRING *)(plist[e].pointer))->pointer)=0;
      ((STRING *)(plist[e].pointer))->len=0;
      break;
    case PL_IVAR:
      *((int *)(plist[e].pointer))=0;
      break;
    case PL_FVAR:
      *((double *)(plist[e].pointer))=0;
      break;
    default:
      printf("ERROR: CLR, unknown var type. i=%d pc=%d\n",e,pc);
      dump_parameterlist(plist,1);
    }
  }
}


void c_exit(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  
  wort_sep(n,' ',TRUE,w1,w2);
  if(*w1==0) c_return(n); 
  else if(strcmp(w1,"IF")==0) {
    if(parser(w2)==-1) c_break("");
  } else  printf("Unbekannter Befehl: EXIT <%s>\n",n);
}

void c_break(char *n) {
  int i,f=0,o;
  i=pcode[pc-1].integer;
  if(i==-1) {
    for(i=pc; (i<prglen && i>=0);i++) {
      o=pcode[i].opcode&PM_SPECIAL;
      if((o==P_LOOP || o==P_NEXT || o==P_WEND ||  o==P_UNTIL)  && f<=0) break;
      if(o & P_LEVELIN) f++;
      if(o & P_LEVELOUT) f--;
     }
    if(i==prglen) { xberror(36,"BREAK/EXIT IF"); /*Programmstruktur fehlerhaft */return;}
    pc=i+1;
  } else pc=i;
}

void c_if(char *n) {
  if((int)parser(n)==0) {  
    int i,f=0,o;
  
    for(i=pc; (i<prglen && i>=0);i++) {
      o=pcode[i].opcode&PM_SPECIAL;
      if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
      else if(o==P_IF) f++;
      else if(o==P_ENDIF) f--;
    }
    
    if(i==prglen) { xberror(36,"IF"); /*Programmstruktur fehlerhaft */return;}
    pc=i+1;
    if(o==P_ELSEIF) {
      char w1[MAXSTRLEN],*w2,*w3,*w4;
      xtrim(program[i],TRUE,w1);
      wort_sep_destroy(w1,' ',TRUE,&w2,&w3);
      wort_sep_destroy(w3,' ',TRUE,&w3,&w4);
      if(strcmp(w3,"IF")==0) c_if(w4); 
    } 
  }
}

void c_select(PARAMETER *plist,int e) {
  int i,wert2;
  char *w1=NULL,*w2,*w3;  
  
  int npc=pcode[pc-1].integer;
  if(npc==-1) {
    xberror(36,"SELECT"); /*Programmstruktur fehlerhaft */
    return;
  }
  int wert=plist->integer;
 // printf("SELECT: value=%d  e=%d\n",wert,e);
  int l=0,l2;
  
  /* Case-Anweisungen finden */
  while(1) {
  //  printf("branch to line %d. <%s>\n",npc-1,program[npc-1]);
    pc=npc;
     if((pcode[pc-1].opcode&PM_SPECIAL)==P_CASE) {
       l2=strlen(program[pc-1])+1;
       if(l2>l || w1==NULL) {
         l=l2+256;
         w1=realloc(w1,l);
       }
       xtrim(program[pc-1],TRUE,w1);
       wort_sep_destroy(w1,' ',TRUE,&w2,&w3);
       
       e=wort_sep_destroy(w3,',',TRUE,&w2,&w3);
       while(e) {
         wert2=parser(w2);
       //  printf("wert2=%d\n",wert2);
	 if(wert==wert2) break;
	 e=wort_sep_destroy(w3,',',TRUE,&w2,&w3);
       }
       if(wert==wert2) break;
       else {
         npc=pcode[pc-1].integer;
       }
     } else break;
  } 
  free(w1);
}
void c_case(char *n) {  /* case und default */
  /*gehe zum naechsten ENDSELECT*/
    pc=suchep(pc,1,P_ENDSELECT,P_SELECT,P_ENDSELECT);
    if(pc==-1) xberror(36,"CASE"); /*Programmstruktur fehlerhaft !*/ 
    pc++;
}

void bidnm(char *n) {
  xberror(38,n); /* Befehl im Direktmodus nicht moeglich */
}


/*Diese routine kann stark verbessert werden, wenn 
  Variablen-typ sowie DOWNTO flag schon beim laden in pass 1 bestimmt wird.*/

void c_next(PARAMETER *plist,int e) {
  char w1[MAXSTRLEN],*w2,*w3,*w4,*var;
  double step, limit,varwert;
  int ss,f=0,hpc=pc,type=NOTYP;

   pc=pcode[pc-1].integer; /*Hier sind wir beim FOR*/
   if(pc==-1) {xberror(36,"NEXT"); /*Programmstruktur fehlerhaft */return;}

//printf("c_next: das for befindet sich bei pc=%d\n",pc);
//printf("Argument dort ist: <%s>\n",pcode[pc].argument);


   strcpy(w1,pcode[pc].argument);
   wort_sep_destroy(w1,' ',TRUE,&w2,&w3);
 
   /* Variable bestimmem */
   if((var=searchchr(w2,'='))!=NULL) {
     *var++=0;
     var=w2;
     type=vartype(var);
     if(type!=INTTYP && type!=FLOATTYP) {printf("Syntax Error: FOR %s, illegal variable type.\n",w2);batch=0;return;}
   } else {printf("Syntax Error ! FOR %s\n",w2); batch=0;return;}
   wort_sep_destroy(w3,' ',TRUE,&w4,&w2);
   
   if(strcmp(w4,"TO")==0) ss=1; 
   else if(strcmp(w4,"DOWNTO")==0) ss=-1; 
   else {printf("Syntax Error ! FOR %s\n",w4); batch=0;return;}

   /* Limit bestimmem  */
   e=wort_sep_destroy(w2,' ',TRUE,&w4,&w3);
   if(e==0) {printf("Syntax Error ! FOR %s\n",w2); batch=0;return;}
   else {
     limit=parser(w4);
     if(e==1) step=1;
     else {
       /* Step-Anweisung auswerten  */
       wort_sep_destroy(w3,' ',TRUE,&w4,&w2);
       if(strcmp(w4,"STEP")==0)
         step=parser(w2);
       else {printf("Syntax Error ! FOR %s\n",w4); batch=0;return;}
     }
   }
   step*=ss;
   varwert=parser(var)+step;
 //  printf("var=<%s>\n",var);
   if(type==FLOATTYP) zuweis(var,varwert);
   else if(type==INTTYP) izuweis(var,(int)varwert);
   
   if(step<0) ss=-1;
   else ss=1;
//   printf("step=%g ss=%d\n",step,ss);
   /* Schleifenende ueberpruefen    */
   if(ss>0) {
     if(varwert>limit) f=TRUE;
   } else {
     if(varwert<limit) f=TRUE;
   } 
   if(f)  pc=hpc;          /* Schleifenende, gehe hinter NEXT */
   else pc++;
}
void c_for(char *n) {
  /* erledigt nur die erste Zuweisung  */
  char *buf=strdup(n);
  char *w1,*w2;

  wort_sep_destroy(buf,' ',TRUE,&w1,&w2);
  if((w2=searchchr(w1,'='))!=NULL) {
    *w2++=0;
    xzuweis(w1,w2);
  } else {printf("Syntax Error ! FOR %s\n",n); batch=0;}
  free(buf);
}
void c_until(char *n) {
  if(parser(n)==0) {
    int npc=pcode[pc-1].integer;
    if(npc==-1) xberror(36,"UNTIL"); /*Programmstruktur fehlerhaft */
    else pc=npc+1;
  }
}

void c_while(char *n) {
  if(parser(n)==0) {
    int npc=pcode[pc-1].integer;
    if(npc==-1) xberror(36,"WHILE"); /*Programmstruktur fehlerhaft */
    pc=npc;
  } 
}


void c_wort_sep(PARAMETER *plist,int e) {
  STRING str1,str2;
  str1.pointer=malloc(plist->integer+1);
  str2.pointer=malloc(plist->integer+1);
  
  wort_sep2(plist[0].pointer,plist[1].pointer,plist[2].integer,str1.pointer,str2.pointer);
  str1.len=strlen(str1.pointer);
  str2.len=strlen(str2.pointer);
  varcaststring_and_free(plist[3].typ,plist[3].pointer,str1);  
  if(e>4)  varcaststring_and_free(plist[4].typ,(STRING *)plist[4].pointer,str2);
  else free_string(&str2);
}

/* Bei split wollen wir den optionalen int parameter ans ende setzen.
   ist aber noch nicht wegen kompatibilitaet.*/
void c_split(PARAMETER *plist,int e) {
  STRING str1,str2;
  str1.pointer=malloc(plist->integer+1);
  str2.pointer=malloc(plist->integer+1);
  
  wort_sep2(plist[0].pointer,plist[1].pointer,plist[2].integer,str1.pointer,str2.pointer);
  str1.len=strlen(str1.pointer);
  str2.len=strlen(str2.pointer);
  varcaststring_and_free(plist[3].typ,plist[3].pointer,str1);  
  if(e>4)  varcaststring_and_free(plist[4].typ,(STRING *)plist[4].pointer,str2);
  else free_string(&str2);
}

/* GET_LOCATION lat,lon,alt,res,speed,....*/
/* globale veriablen, welche die GPS Informationen aufnehmen.*/
double gps_alt,gps_lat=-1,gps_lon=-1;
float gps_bearing,gps_accuracy,gps_speed;
double gps_time;
char *gps_provider;

void c_getlocation(PARAMETER *plist,int e) {
#ifdef ANDROID
  /* mae sure, that the values get updated */
  ANDROID_get_location();
#endif
  if(e>0 && plist[0].typ!=PL_LEER) varcastfloat(plist[0].integer,plist[0].pointer,gps_lat);
  if(e>1 && plist[1].typ!=PL_LEER) varcastfloat(plist[1].integer,plist[1].pointer,gps_lon);
  if(e>2 && plist[2].typ!=PL_LEER) varcastfloat(plist[2].integer,plist[2].pointer,gps_alt);
  if(e>3 && plist[3].typ!=PL_LEER) varcastfloat(plist[3].integer,plist[3].pointer,gps_bearing);
  if(e>4 && plist[4].typ!=PL_LEER) varcastfloat(plist[4].integer,plist[3].pointer,gps_accuracy);
  if(e>5 && plist[5].typ!=PL_LEER) varcastfloat(plist[5].integer,plist[3].pointer,gps_speed);
  if(e>6 && plist[6].typ!=PL_LEER) varcastfloat(plist[6].integer,plist[6].pointer,gps_time);
  if(e>7 && plist[7].typ!=PL_LEER) {
    STRING a;
    a.pointer=gps_provider;
    if(a.pointer) a.len=strlen(a.pointer);
    else a.len=0;
    varcaststring(plist[7].integer,plist[7].pointer,a);
  }
}


void c_poke(PARAMETER *plist,int e) {
  char *adr=(char *)(plist[0].integer);
  *adr=(char)plist[1].integer;
}
void c_dpoke(PARAMETER *plist,int e) {
  short *adr=(short *)(plist[0].integer);
  *adr=(short)plist[1].integer;
}
void c_lpoke(PARAMETER *plist,int e) {
  long *adr=(long *)plist[0].integer;
  *adr=(long)plist[1].integer;
}

/* SOUND channel,frequency [Hz],volume (0-1),duration (s)*/

void c_sound(PARAMETER *plist,int e) { 
  double duration=-1;
  int c=-1;
  double frequency=-1;
  double volume=-1;
  if(plist[0].typ!=PL_LEER) c=plist[0].integer;
  if(e>=2 && plist[1].typ!=PL_LEER) frequency=plist[1].real;
  if(e>=3 && plist[2].typ!=PL_LEER) volume=plist[2].real;
  if(e>=4) duration=plist[3].real;
  sound_activate();
  do_sound(c,frequency,volume,duration);
}

/* PLAYSOUND channel,data$[,pitch,volume] */

void c_playsound(PARAMETER *plist,int e) { 
  int pitch=0x100,volume=0xffff;
  int c=-1;
  sound_activate();
  if(plist[0].typ!=PL_LEER) c=plist[0].integer;
  if(e>=3) pitch= (int)(plist[2].real*0x100);
  if(e>=4) volume= (int)(plist[3].real*0xffff);
  do_playsound(c,plist[1].pointer,plist[1].integer,pitch,volume,0);
}

void c_playsoundfile(PARAMETER *plist,int e) {
  if(exist(plist[0].pointer)) {
#ifdef ANDROID
  ANDROID_playsoundfile(plist[0].pointer);
#else
  char buffer[256];
  int sret;
  sprintf(buffer,"ogg123 %s &",plist[0].pointer); 
  sret=system(buffer);
#endif    
  } else xberror(-33,plist[0].pointer); /* file not found*/
}





/* WAVE channel,...*/

void c_wave(PARAMETER *plist,int e) { 
  int c=-1;
  int form=-1;
  double attack=-1;
  double decay=-1;
  double sustain=-1;
  double release=-1;
  
  if(plist[0].typ!=PL_LEER) c=plist[0].integer;
  if(e>=2 && plist[1].typ!=PL_LEER) form=plist[1].integer;
  if(e>=3 && plist[2].typ!=PL_LEER) attack=plist[2].real;
  if(e>=4 && plist[3].typ!=PL_LEER) decay=plist[3].real;
  if(e>=5 && plist[4].typ!=PL_LEER) sustain=plist[4].real;
  if(e>=6 && plist[5].typ!=PL_LEER) release=plist[5].real;
  sound_activate();
  do_wave(c,form,attack,decay,sustain,release);
}

#ifdef ANDROID
  extern void ANDROID_speek(char *,double,double,char *);
#endif

void c_speak(PARAMETER *plist,int e) { 
  double pitch=-1,rate=-1;
  char *enc=NULL;
  if(e>=2) pitch= plist[1].real;
  if(e>=3) rate=plist[2].real;
  if(e>=4) enc=plist[3].pointer;
#ifdef ANDROID
  ANDROID_speek(plist[0].pointer,pitch,rate,enc);
#endif
}

void c_eval(PARAMETER *plist,int e) { kommando(plist[0].pointer); }
