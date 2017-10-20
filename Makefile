#
#  Makefile - for HBSURF hierarchical B-Spline Surface Modeler
#
#  copyright 1996 by  Richard C. Davis and Yizhou Yu
#
#  All rights reserved.  You are free to copy, dissect, reuse, or do
#  whatever you want with this code except for selling it for profit
#  without our consent.  Please understand that it comes with NO
#  WARRANTY!  Use at your own risk!
#

OGLDIR     = /home/orodruin/j/grad/mdownes/mesa/Mesa-1.2.8
CC         = gcc
SYS_DEF    = -DOPENGL -D_HPUX_SOURCE -DANSI_CPP
INC_DIRS   = -I$(OGLDIR)/include -I$(OGLDIR)/include/GL \
             -I/usr/sww/X11R5/include/X11

CFLAGS     = -ansi -Wall -g $(SYS_DEF) $(INC_DIRS)

LIBDIR     = -L$(OGLDIR)/lib -L/usr/sww/X11R5/lib     # -L/usr/lib/Motif1.2
             # -Wl,+b/opt/graphics/common/lib      #this line for HPUX 10.x

GLUT_LIB   = -Wl,-a,archive -lglut
OGL_LIB    = -Wl,-a,archive -lMesaGLU -lMesaGL

WIDGET_LIB = -Wl,-a,archive -lGLw -Wl,-a,default -lXm 
SYS_LIBS   = -Wl,-a,default -Wl,+s -lm -lXt -lX11 -lXext -lXmu

LIBS       = $(GLUT_LIB) $(OGL_LIB) $(SYS_LIBS)

TARGETS    = hbsurf
OBJECTS    = main.o window.o world.o error.o vector.o matrix.o uiplist.o \
             mesh.o store.o

.c.o:
	$(CC) -c $(CFLAGS) $*.c $(INCDIR)

all:  $(TARGETS)

$(TARGETS): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBDIR) $(GLUT_LIB) $(LIBS)

clean:  cleanexe
	rm -f *.o

cleanexe:
	rm -f $(TARGETS)

tell:
	@echo DEMOS: $(TARGETS)

