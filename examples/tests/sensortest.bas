' Test of the Android builtin sensors  05.2012
'
'
PRINT "SENSOR?=";SENSOR?
PAUSE 1
FOR i=0 TO 10
  PRINT SENSOR(0),SENSOR(1),SENSOR(2)
  PAUSE 0.5
NEXT i

PRINT "ON"
SENSOR ON

FOR i=0 TO 10
  PRINT SENSOR(0),SENSOR(1),SENSOR(2),SENSOR(3),SENSOR(4),SENSOR(5)
  PAUSE 0.5
NEXT i
PRINT "OFF"
SENSOR OFF

FOR i=0 TO 10
  PRINT SENSOR(0),SENSOR(1),SENSOR(2)
  PAUSE 0.5
NEXT i
END