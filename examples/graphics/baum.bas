gelb=get_color(65535,32000,0)
schwarz=get_color(0,32000,0)
rot=get_color(65535,0,0)
weiss=get_color(65535,65535,32000)

@figur(320,200,0,1)
vsync
~inp(-2)
quit



procedure figur(ox,oy,r,scale)
  local xx,i,nr,nr2,nx,ny,nx2,ny2
  dim x(5),y(5)
  x(0)=-scale*100/2
  y(0)=-scale*100/2
  x(1)=scale*100/2
  y(1)=-scale*100/2
  x(2)=scale *100/2
  y(2)=scale*100/2
  x(3)=-scale*100/2
  y(3)=scale*100/2
  x(4)=-scale*100/2
  y(4)=-scale*100/2
  
  
  
  for i=0 to 4
    xx=cos(r)*x(i)+sin(r)*y(i)
    y(i)=-sin(r)*x(i)+cos(r)*y(i)
    x(i)=xx
  next i
  color weiss
  polyfill 5,x(),y(),ox,oy
  color gelb
  polyline 5,x(),y(),ox,oy
  vsync
  
  nr=r-pi/4
  nx=ox+cos(nr)*scale*100
  ny=oy+sin(nr)*scale*100
  nr2=r+pi/4
  nx2=ox+cos(nr2)*scale*100
  ny2=oy+sin(nr2)*scale*100
  
  
  if scale>0.005
    @figur(nx,ny,nr,scale/sqrt(2))
    @figur(nx2,ny2,nr2,scale/sqrt(2))
  '  @figur(-scale*100+ox,oy,pi/3+r,scale/2)
  '  @figur(ox,oy-scale*100,-pi/3+r,scale/2)
  endif
'  pause 0.1
return
