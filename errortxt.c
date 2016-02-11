/* errortxt.c  (c) Markus Hoffmann   */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include "config.h"
#include "options.h"
#include <stdio.h>

/* This is the list of official error messages liked to a number
   (see the command reference for ERROR <n>). Positive numbers correspond to
   X11-Basic ruintime errors, negative numbers correspond to system errors

   The translation is not complete. The length of the Messages 
   (including any possible inserted stings) can not 
   exceed MAXERRORTXTLEN.
   */


#define MAXERRORTXTLEN 128

const char *errortxt[] = {
#ifdef GERMAN
/*   0*/  "Division durch Null",
/*   1*/  "�berlauf",
/*   2*/  "Zahl nicht Integer -2147483648 .. 2147483647",
/*   3*/  "Zahl nicht Byte 0 .. 255",
/*   4*/  "Zahl nicht Wort -32768 .. 32767",
/*   5*/  "Quadratwurzel nur f�r positive Zahlen",
/*   6*/  "Logarithmen nur f�r Zahlen gr��er Null",
/*   7*/  "Unbekannter Fehler",
/*   8*/  "Speicher voll",
/*   9*/  "Funktion oder Befehl |%s| ist in dieser Version nicht m�glich.",
/*  10*/  "String zu lang",
/*  11*/  "Argument muss positiv sein!",
/*  12*/  "Programm zu lang Speicher voll NEW",
/*  13*/  "Unpassende Typen im Ausdruck %s",
/*  14*/  "Feld %s zweimal dimensioniert",
/*  15*/  "Feld nicht dimensioniert: %s()",
#else
/*   0*/  "Divide by zero",
/*   1*/  "Overflow",
/*   2*/  "Value not integer -2147483648 .. 2147483647",
/*   3*/  "Value not byte 0 .. 255",
/*   4*/  "Value not short -32768 .. 32767",
/*   5*/  "Square root: only positive numbers",
/*   6*/  "Logarithm only for positive numbers",
/*   7*/  "Unknown Error %s",
/*   8*/  "Out of Memory",
/*   9*/  "Function or command |%s| is not implemented in this version",
/*  10*/  "String too long",
/*  11*/  "Argument needs to be positive",
/*  12*/  "Program too long, buffer size exceeded --> NEW",
/*  13*/  "Type mismatch in expression %s",
/*  14*/  "Array %s() is already dimensioned",
/*  15*/  "Array not dimensioned: %s()",
#endif
#ifdef GERMAN
/*  16*/  "Feldindex zu gro�",
/*  17*/  "Dim zu gro�",
/*  18*/  "Falsche Anzahl Indizes",
/*  19*/  "Procedure %s nicht gefunden",
/*  20*/  "Label %s nicht gefunden",
/*  21*/  "Bei Open nur erlaubt: \"I\"nput  \"O\"utput \"A\"ppend \"U\"pdate",
/*  22*/  "File schon ge�ffnet",
/*  23*/  "File # falsch",
/*  24*/  "File nicht ge�ffnet",
/*  25*/  "Falsche Eingabe, keine Zahl",
/*  26*/  "Fileende erreicht EOF",
/*  27*/  "Zu viele Punkte f�r Polyline/Polyfill",
/*  28*/  "Feld muss eindimensional sein",
/*  29*/  "Ungueltige Adresse! %s",
/*  30*/  "Merge - Kein ASCII-File",
/*  31*/  "Merge - Zeile zu lang - Abbruch",
/*  32*/  "==> Syntax nicht korrekt %s",
#else
/*  16*/  "Field index too large",
/*  17*/  "Dim too large",
/*  18*/  "Wrong number of indexes",
/*  19*/  "Procedure %s not found",
/*  20*/  "Label %s not found",
/*  21*/  "Open only \"I\"nput  \"O\"utput \"A\"ppend \"U\"pdate",
/*  22*/  "File already opened",
/*  23*/  "Wrong file #",
/*  24*/  "File not opened",
/*  25*/  "Wrong input, no number",
/*  26*/  "EOF - reached end of file",
/*  27*/  "Too many points for Polyline/Polyfill",
/*  28*/  "Array must be one dimensional",
/*  29*/  "Illegal address! %s",
/*  30*/  "Merge - no ASCII file",
/*  31*/  "Merge - line too long - CANCEL",
/*  32*/  "==> Syntax error %s",
#endif
#ifdef GERMAN
/*  33*/  "Marke %s nicht definiert",
/*  34*/  "Zu wenig Data",
/*  35*/  "Data nicht numerisch",
/*  36*/  "Programmstruktur Fehlerhaft %s",
/*  37*/  "Diskette voll",
/*  38*/  "Befehl %s im Direktmodus nicht m�glich",
/*  39*/  "Programmfehler. Kein Gosub m�glich",
/*  40*/  "Clear nicht m�glich in For-Next-Schleifen oder Proceduren",
/*  41*/  "Cont nicht m�glich",
/*  42*/  "Zu wenig Parameter %s",
/*  43*/  "Ausdruck zu komplex",
/*  44*/  "Funktion %s nicht definiert",
/*  45*/  "Zu viele Parameter",
/*  46*/  "Parameter %s falsch, keine Zahl",
/*  47*/  "Parameter %s falsch, kein String",
/*  48*/  "Open \"R\" - Satzl�nge falsch",
#else
/*  33*/  "Label %s not defined",
/*  34*/  "Not enough data",
/*  35*/  "data must be numeric",
/*  36*/  "Error in program structure %s",
/*  37*/  "Disk full",
/*  38*/  "Command %s not allowed in interactive mode",
/*  39*/  "Program Error GOSUB impossible",
/*  40*/  "CLEAR not allowed within For-Next-loops or procedures",
/*  41*/  "CONT not possible here",
/*  42*/  "Not enough parameters %s",
/*  43*/  "Expression too complex",
/*  44*/  "Function %s not defined",
/*  45*/  "Too many parameters",
/*  46*/  "Incorrect parameter, %s must be number",
/*  47*/  "Incorrect parameter, %s must be string",
/*  48*/  "Open \"R\" - incorrect Field length ",
#endif
#ifdef GERMAN
/* 1*/  "Zu viele \"R\"-Files (max. 31)",
/* 2*/  "Kein \"R\"-File",
/* 3*/  "Parser: Syntax Error <%s>",
/* 4*/  "Fields gr��er als Satzl�nge",
/* 5*/  "Falsches Grafik-Format %s",
/* 6*/  "GET/PUT Field-String L�nge falsch",
/* 7*/  "GET/PUT Satznummer falsch",
/* 56*/  "Falsche Anzahl Parameter %s",
/* 57*/  "Variable %s noch nicht initialisiert",
/* 58*/  "Variable %s ist vom falschen Typ",
/* 59*/  "Grafik hat falsche Farbtiefe",
/* 60*/  "Sprite-String-L�nge falsch",
/* =*/  "Fehler bei RESERVE",
/* >*/  "Menu falsch",
/* ?*/  "Reserve falsch",
#else
/* 49*/  "Too many \"R\"-files (max. 31)",
/* 50*/  "No \"R\"-file",
/* 51*/  "Parser: Syntax Error <%s>",
/* 52*/  "Fields larger than field length",
/* 53*/  "Wrong graphic format %s",
/* 54*/  "GET/PUT wrong Field-String length",
/* 55*/  "GET/PUT wrong number",
/* 56*/  "Wrong number of parameters %s",
/* 57*/  "Variable %s is not yet initialized",
/* 58*/  "Variable %s has incorrect type",
/* 59*/  "Graphic has wrong color depth",
/* 60*/  "Sprite-String length wrong",
/* 61*/  "Error with RESERVE",
/* 62*/  "Menu wrong",
/* 63*/  "Reserve wrong",
#endif
#ifdef GERMAN
/* 64*/  "Pointer falsch",
/* A*/  "Feldgr��e < 256",
/* B*/  "Kein VAR-Array",
/* C*/  "ASIN/ACOS falsch",
/*  68*/  "Falsche VAR-Type",
/*  69*/  "ENDFUNC ohne RETURN",
/*  70*/  "Unbekannter Fehler 70",
/*  71*/  "Index zu gro�",
/* 72*/  "Fehler bei RSRC_LOAD",
/* 73*/  "Fehler bei RSRC_FREE",
#else
/* 64*/  "Pointer wrong",
/* A*/  "Field size < 256",
/* B*/  "No VAR-Array",
/* C*/  "ASIN/ACOS wrong",
/*  68*/  "Wrong VAR-Type",
/*  69*/  "ENDFUNC without RETURN",
/*  70*/  "Unknown Error 70",
/*  71*/  "Index too large",
/* 72*/  "Error in RSRC_LOAD",
/* 73*/  "Error in RSRC_FREE",
#endif
/* 74*/  "Array %s dimensioning mismatch",
/* 75*/  "Stack overflow!",
/* 76*/  "Illegal variable name %s. can not create.",
/* 77*/  "Error 77",
/* 78*/  "Error 78",
/* 79*/  "Error 79",
#ifdef GERMAN
/* 80*/  "Matrizenoperationen nur f�r ein- oder zweidimensionale Felder",
/* 81*/  "Matrizen haben nicht die gleiche Ordnung",
/* 82*/  "Vektorprodukt nicht definiert",
/* 83*/  "Matrizenprodukt nicht definiert",
/* 84*/  "Scalarprodukt nicht definiert",
/* 85*/  "Transposition nur f�r zweidimensionale Matrizen",
/* 86*/  "Matrix nicht quadratisch",
/* W*/  "Transposition nicht definiert",
/* X*/  "FACT/COMBIN/VARIAT nicht definiert",
/* 89*/  "Das Array muss zweidimensional sein",
#else
/* 80*/  "Matrix operations only allowed for one or two dimensional arrays",
/* 81*/  "Matrices do not have the same order",
/* 82*/  "Vector product not defined",
/* 83*/  "Matrix product not defined",
/* 84*/  "Scalar product not defined",
/* 85*/  "Transposition only for two dimensional matrices",
/* 86*/  "Matrix must be square",
/* W*/   "Transposition not defined",
/* X*/   "FACT/COMBIN/VARIAT not defined",
/* 89*/  "Array must be two dimensional",
#endif
#ifndef GERMAN
/*  90*/  "Error in Local",
/*  91*/  "Error in For",
/*  92*/  "Resume (next) not possible: Fatal, For or Local",
/*  93*/  "Stack Error",
/*  94*/  "Parameter %s must be float ARRAY",
/* 95 */  "Parameter %s must be ARRAY",
/* 96*/  "ARRAY %s has the wrong type. Can not convert.",
/*  a*/  "This operation %s is not allowed for root window",
/*  98*/  "Illegal Window number %s (0-16)",
/*  99*/  "Window %s does not exist",
#else
/*  90*/  "Fehler bei Local",
/*  91*/  "Fehler bei For",
/*  92*/  "Resume (next) nicht m�glich   Fatal, For oder Local",
/*  93*/  "Stapel-Fehler",
/*  94*/  "Parameter %s muss float ARRAY sein",
/* 95 */  "Parameter %s muss ARRAY sein",
/* 96*/  "ARRAY %s hat den falschen Typ. Kann nicht konvertieren.",
/*  a*/  "Diese Operation %s ist nicht erlaubt fuer das Hintergrund-Fenster",
/* 98*/  "Illegale Fensternummer %s (0-16)",
/* 99*/  "Das Fenster %s existiert nicht",
#endif

/* 100*/  "X11-BASIC Version " VERSION "  Copyright (c) 1997-2014 Markus Hoffmann",
#ifdef GERMAN
/* e*/  "** 1 - Speicherschutzverletzung",
/* f*/  "** 2 - Bus Error Peek/Poke falsch?",
/* g*/  "** 3 - Adress-Fehler. Ungerade Wort-Adresse! Dpoke/Dpeek, Lpoke/Lpeek?",
/* h*/  "** 4 - Illegal Instruction : ung�ltiger Maschinenbefehl",
/* i*/  "** 5 - Division durch Null",
/* j*/  "** 6 - CHK exception : CHK-Befehl",
/* k*/  "** 7 - TRAPV-Befehl",
/* l*/  "** 8 - Privilegverletzung",
/* m*/  "** 9 - Trace exception : Trace ohne Monitor",
/* n*/  "** 10 - Broken pipe : Ausgabeweitergabe abgebrochen",
#else
/* e*/  "** 1 - Segmentation fault",
/* f*/  "** 2 - Bus Error: peek/poke ?",
/* g*/  "** 3 - Address error:  Dpoke/Dpeek, Lpoke/Lpeek?",
/* h*/  "** 4 - Illegal Instruction",
/* i*/  "** 5 - Divide by Zero",
/* j*/  "** 6 - CHK exception",
/* k*/  "** 7 - TRAPV exception",
/* l*/  "** 8 - Privilege Violation",
/* m*/  "** 9 - Trace exception",
/* n*/  "** 10 - Broken pipe",
#endif
/* o*/  "Error 111",
/* p*/  "Error 112",
/* q*/  "Error 113",
/* r*/  "Error 114",
/* s*/  "Error 115",
/* t*/  "Error 116",
/* u*/  "Error 117",
/* v*/  "Error 118",
/* w*/  "Error 119",
/* x*/  "Error 120",
/* y*/  "Error 121",
/* z*/  "Error 122",
/* {*/  "Error 123",
/*  */  "Error 124",
/* }*/  "Error 125",
/* ~*/  "Error 126",
/* 127*/"Error 127",

/* �*/  "Error -128",
/* �*/  "Error -127",
/* �*/  "Error -126",
#ifdef GERMAN
/* -125*/  "* Number of hash collisions exceeds maximum generation counter value.",
/* -124*/  "* Andere Diskette einlegen",
/* -123*/  "* Bitte Disk einlegen",
#else
/* -125*/  "* Number of hash collisions exceeds maximum generation counter value.",
/* -124*/  "* Wrong medium type",
/* -123*/  "* No medium found",
#endif
/* -122*/  "* Quota exceeded",
/* -121*/  "* Remote I/O error",
/* -120*/  "* Is a named type file",
/* -119*/  "* No XENIX semaphores available",
/* -118*/  "* Not a XENIX named type file",
/* -117*/  "* Structure needs cleaning",
/* -116*/  "* Stale NFS file handle",
/* -115*/  "* Operation now in progress",
/* -114*/  "* Operation already in progress",
/* -113*/  "* No route to host",
/* -112*/  "* Host is down",
#ifdef GERMAN
/* -111*/  "* Verbindungsaufbau verweigert",
/* -110*/  "* Zeit�berschreitung bei Verbindung",
/* -109*/  "* Too many references: can not splice",
/* -108*/  "* Can not send after transport endpoint shutdown",
/* -107*/  "* Transport endpoint is not connected :%s: Keine Verbindung, Verbindung unterbrochen ?",
/* -106*/  "* Transport endpoint is already connected : Verbindung schon ge�ffnet ",
/* -105*/  "* No buffer space available : Speicher voll",
/* -104*/  "* Connection reset by peer",
/* -103*/  "* Verbindungsabbruch durch Anwender",
/* -102*/  "* Netzwerk Verbindung wurde zurueckgesetzt",
/* -101*/  "* Netzwerk ist nicht erreichbar",
/* -100*/  "* Netzwerk ist nicht verfuegbar",
/* -99*/  "* Verbindungsaufbau %s nicht m�glich",
/* -98*/  "* Besetzt, Verbindung %s nicht m�glich",
#else
/* -111*/  "* Connection refused",
/* -110*/  "* Connection timed out",
/* -109*/  "* Too many references: can not splice",
/* -108*/  "* Can not send after transport endpoint shutdown",
/* -107*/  "* Transport endpoint is not connected %s",
/* -106*/  "* Transport endpoint is already connected",
/* -105*/  "* No buffer space available",
/* -104*/  "* Connection reset by peer",
/* -103*/  "* Software caused connection abort",
/* -102*/  "* Network dropped connection because of reset",
/* -101*/  "* Network is unreachable",
/* -100*/  "* Network is down",
/* -99*/  "* Can not assign requested address %s",
/* -98*/  "* Address %s already in use",
#endif

/* -97*/  "* Address family not supported by protocol",
/* -96*/  "* Protocol family not supported",
/* -95*/  "* Operation not supported on transport endpoint",
/* -94*/  "* Socket type not supported",
/* -93*/  "* Protocol not supported",
/* -92*/  "* Protocol not available",
/* -91*/  "* Protocol wrong type for socket",
/* -90*/  "* Message too long",
/* -89*/  "* Destination address required",
#ifdef GERMAN
/* -88*/  "* Operation nur mit Sockets erlaubt",
#else
/* -88*/  "* Socket operation on non-socket",
#endif
/* -87*/  "* Too many users",
/* -86*/  "* Streams pipe error",
/* -85*/  "* Interrupted system call should be restarted",
/* -84*/  "* Illegal byte sequence",
/* -83*/  "* Can not exec a shared library directly",
/* -82*/  "* Attempting to link in too many shared libraries",
/* -81*/  "* .lib section in a.out corrupted",

/* -80*/  "* Accessing a corrupted shared library",
/* -79*/  "* Can not access a needed shared library",
/* -78*/  "* Remote address changed",
/* -77*/  "* File descriptor in bad state",
/* -76*/  "* Name not unique on network",
/* -75*/  "* Value too large for defined data type",
/* -74*/  "* Not a data message",
/* -73*/  "* RFS specific error",
#ifdef GERMAN
/* -72*/  "* Operation zur Zeit nicht m�glich, versuchen Sie es spaeter nochmal",
#else
/* -72*/  "* Try again",
#endif
/* -71*/  "* Too many symbolic links encountered",

#ifdef GERMAN
/* -70*/  "* Dateiname zu lang",
/* -69*/  "* Resource deadlock would occur",
/* -68*/  "* Advertise error",
/* -67*/  "* Speicherblockfehler",
/* -66*/  "* Kein Bin�rprogramm",
#else
/* -70*/  "* File name too long",
/* -69*/  "* Resource deadlock would occur",
/* -68*/  "* Advertise error",
/* -67*/  "* memory page error",
/* -66*/  "* no executable",
#endif
/* -67*/  "* Link has been severed",
/* -66*/  "* Object is remote",
/* -63*/  "* Math result not representable",
/* -62*/  "* Math arg out of domain of func",
/* -61*/  "* Cross-device link",

/* -60*/  "* Device not a stream",
/* -59*/  "* Mount device busy",
/* -58*/  "* Block device required",
/* -57*/  "* Bad address",
/* -56*/  "* No more processes",
/* -55*/  "* No children",
/* -54*/  "* Exchange full",
/* -53*/  "* Interrupted system call",
/* -52*/  "* Invalid exchange",
/* -51*/  "* Permission denied, you must be super-user",

#ifdef GERMAN
/* -50*/  "* Operation auf diesem Kanal nicht (mehr) m�glich",
/* -49*/  "* Keine weiteren Dateien",
/* -48*/  "* Link number out of range",
/* -47*/  "* Level 3 reset",
/* -46*/  "* Ung�ltige Laufwerksbezeichnung",
#else
/* -50*/  "* Operation in this channel not possible (any more)",
/* -49*/  "* no more files ",
/* -48*/  "* Link number out of range",
/* -47*/  "* Level 3 reset",
/* -46*/  "* Illegal Drive identifier",
#endif
/* -45*/  "* Level 2 not synchronized",
/* -44*/  "* Channel number out of range",
/* -43*/  "* Identifier removed",
/* -42*/  "* No message of desired type",
/* -41*/  "* Operation %s would block",
#ifdef GERMAN
/* -40*/  "* Ung�ltige Speicherblockadresse",
/* -39*/  "* Das Verzeichnis %s ist nicht leer",
/* -38*/  "* Unbekannter Befehl %s",
/* -37*/  "* Ung�ltiges Handle %s",
/* -36*/  "* Zugriff nicht m�glich",
/* -35*/  "* Zu viele Dateien offen",
/* -34*/  "* Pfadname nicht gefunden",
/* -33*/  "* %s Datei nicht gefunden",
/* -32*/  "* Verbindung wurde unterbrochen",
/* -31*/  "* Zu viele Links",
#else
/* -40*/  "* illegal page address",
/* -39*/  "* Directory %s not empty",
/* -38*/  "* Function %s not implemented",
/* -37*/  "* Illegal Handle %s",
/* -36*/  "* Access not possible",
/* -35*/  "* Too many open files",
/* -34*/  "* Path not found",
/* -33*/  "* File %s not found",
/* -32*/  "* Broken pipe",
/* -31*/  "* Too many links",
#endif
#ifdef GERMAN
/* -30*/  "* File-System ist Schreibgesch�tzt",
/* -29*/  "* Seek falsch",
/* -28*/  "* File-System %s ist voll",
/* -27*/  "* File %s ist zu gross f�r diese Operation",
/* -26*/  "* Text file %s busy",
/* -25*/  "* %s Not a typewriter",
/* -24*/  "* Too many open files",
/* -23*/  "* File table overflow : Zur Zeit sind keine weiteren offenen Files m�glich",
/* -22*/  "* Invalid argument %s",
/* -21*/  "* %s Is a directory",
#else
/* -30*/  "* Read-Only File-System",
/* -29*/  "* Illegal seek",
/* -28*/  "* No space left on device %s ",
/* -27*/  "* File %s too large",
/* -26*/  "* Text file %s busy",
/* -25*/  "* %s Not a typewriter",
/* -24*/  "* Too many open files",
/* -23*/  "* File table overflow",
/* -22*/  "* Invalid argument %s",
/* -21*/  "* %s Is a directory",
#endif
#ifdef GERMAN
/* -20*/  "* %s ist kein Ordner",
/* -19*/  "* Geraet %s existiert nicht",
/* -18*/  "* Cross-device link",
/* -17*/  "* File schon vorhanden",
/* -16*/  "* Bad Sektor (Verify)",
/* -15*/  "* Unbekanntes Ger�t",
/* -14*/  "* Diskette wurde gewechselt",
/* -13*/  "* Die Erlaubnis wurde verweigert",
/* -12*/  "* Speicher voll",
/* -11*/  "* Lesefehler",
#else
/* -20*/  "* %s Not a directory",
/* -19*/  "* No such device %s",
/* -18*/  "* Cross-device link",
/* -17*/  "* File exists",
/* -16*/  "* Bad sector (verify)",
/* -15*/  "* unknown device",
/* -14*/  "* Disk was changed",
/* -13*/  "* Permission denied",
/* -12*/  "* Not enough core memory",
/* -11*/  "* read error",
#endif
#ifdef GERMAN
/* -10*/  "* Schreibfehler",
/*  -9*/  "* Kein Papier",
/*  -8*/  "* Sektor nicht gefunden",
/*  -7*/  "* Zu viele Parameter",
/*  -6*/  "* Seek Error Spur nicht gefunden",
/*  -5*/  "* Ung�ltiger Befehl",
/*  -4*/  "* CRC Fehler: Disk-Pr�fsumme falsch",
/*  -3*/  "* Prozess existiert nicht",
/*  -2*/  "* Zeitueberschreitung",
/*  -1*/  "* Allgemeiner Eingabe/Ausgabe-Fehler %s"
#else
/* -10*/  "* write error",
/*  -9*/  "* No paper",
/*  -8*/  "* Sector not found",
/*  -7*/  "* Arg list too long",
/*  -6*/  "* Seek Error",
/*  -5*/  "* Bad Request",
/*  -4*/  "* CRC Error wrong check sum",
/*  -3*/  "* No such process",
/*  -2*/  "* Timeout",
/*  -1*/  "* IO-Error %s"
#endif
};

/* Returns the Error message which belongs to the number errnr.
   The message is completed by an optional string bem which is inserted into
   the error message at a position denoted by %s */

const char *error_text(unsigned char errnr, const char *bem) {
  if(bem) {
    static char errbuffer[MAXERRORTXTLEN];
    snprintf(errbuffer,MAXERRORTXTLEN,errortxt[errnr],bem );
    return(errbuffer);
  } else return(errortxt[errnr]);
}

