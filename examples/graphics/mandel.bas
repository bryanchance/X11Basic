' Mandelbrotfraktale (c) Markus Hoffmann 2003-02-07
' This is a Mote-Carlo algorithm. written in X11-Basic
'
RANDOMIZE
bx=0
by=0
bw=256*2
bh=256*2
SIZEW ,bw,bh
CLEARW
DIM col(256) 
col(0)=GET_COLOR(0,0,0)
col(255)=GET_COLOR(65535,65535,65535)
for i=1 to 254
  col(i)=GET_COLOR(RANDOM(65535),RANDOM(65535),RANDOM(65535))
NEXT i 

GET_GEOMETRY 1,bx,by,bw,bh
DIM field(bw,bh)
ARRAYFILL field(),0

sx=-2.2
sy=-1.7
sw=3.4
sh=3.4

starttime=TIMER
x=0
y=0
count=0
count2=0
DO
  IF MOUSEK
    VSYNC
    IF MOUSEX>=0 AND MOUSEY>=0 AND MOUSEX<bw AND MOUSEY<bh
      x=MOUSEX
      y=MOUSEY
    ENDIF
  ELSE
    t0=TIMER
    WHILE field(x,y) AND TIMER-t0<3
      x=RANDOM(bw)
      y=RANDOM(bh)
      INC count2
    WEND
  ENDIF
  c=0
  gx=(x-bx)/bw*sw+sx
  gy=(y-by)/bh*sh+sy
  zx=gx
  zy=gy
  WHILE c<256
    nzx=zx^2-zy^2+gx
    zy=2*zx*zy+gy
    zx=nzx
    EXIT IF zx^2+zy^2>4
    INC c
  WEND
  field(x,y)=TRUE
  c=c AND 255
  IF c>0
    COLOR col(c)
    PLOT x,y
    IF (count2 MOD 256)=0 OR TIMER-t>1
      COLOR col(0)
      PBOX 0,0,70,20
      COLOR col(255)
      TEXT 10,10,STR$(INT(100*count/bw/bh))+"%  "+STR$(count2-count)
      VSYNC
      t=TIMER
    ENDIF
  ENDIF
  INC count
  EXIT IF INT(100*count/bw/bh)=100
LOOP
PRINT "";TIMER-starttime;" seconds."
print "Press any key..."
~INP(-2)
END
