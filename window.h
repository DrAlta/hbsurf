/*
*  window.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#ifndef __WINDOW_H__
#define __WINDOW_H__

/*
  This is the Window object.  It is assumed that there is a single
  window object, a global variable named "win".  This should be the
  only global variable in the program.
  */


#include "project.h"
#include <GL/glut.h>
#include "world.h"
#include "error.h"

BOOL winInit(int argc, char **argv);
BOOL winParseCommandLine(int argc, char **argv);
void winInitGLUT(int argc, char **argv);
void winMainLoop();

/* Call Back Handlers */
void winHandleButton(int iButton, int iState, int ipxX, int ipxY);
void winHandleMotion(int ipxX, int ipxY);
void winHandleKeys(unsigned char ucKey, int ipxX, int ipxY);
void winDisplay();
void winReshape(int iWidth, int iHeight);
void winIdle();

/* Helpful Print Functions */
void winPrintHello();

void winSetAllowIdle(BOOL bAllowIdle);

/* Abstracts the Screen Coordinant system to be Right Handed */
int winDecodePixelX(int iX);
int winDecodePixelY(int iY);
int winEncodePixelX(int iX);
int winEncodePixelY(int iY);

/* Abstracts full screen coordinates to viewport coordinates.  
   If (wqdown == wqNone), takes (iX,iY) and returns the WinQuad 
   the point lies in.  If (wqdown != wqNone), assumes the point
   stays in the same WinQuad.  If neither riX nor riY are NULL, 
   places new, translated values in *riX,*riY. */
WinQuad winWhichQuad(int iX, int iY, WinQuad wqdown, WinQuad wqdom, 
		     int* riX, int *riY);

/* This is a Bicubic B-spline patch rendering function.  It takes a 
   3-D array of floats (i,j,k) organized as follows:

        ^ (0,3,k) (1,3,k) (2,3,k) (3,3,k)
 incr.  | (0,2,k) (1,2,k) (2,2,k) (3,2,k)    k=(0,1,2)=(x,y,z)
  v     | (0,1,k) (1,1,k) (2,1,k) (3,1,k)
        | (0,0,k) (1,0,k) (2,0,k) (3,0,k)
	  ------->
	  incr. u

   ...and renders the patch associated with those 16 points.  The value
   "st" determines whether or not the patch will be rendered as a solid 
   or a wireframe.  The value "level" may be used to determine the color
   of the patch.
 */
void patchRender(GLfloat pts[4][4][3], int level, BOOL MultiC,
		 SolidType st, int subdiv);

extern Window win;

#endif /* __WINDOW_H__ */
