/*
*  world.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#ifndef __WORLD_H__
#define __WORLD_H__

#include <stdlib.h>
#include "project.h"
#include <GL/glut.h>
#include "mathlib.h"
#include "error.h"

void worldInit(World *w, char *readname);
void worldReshapeViewPort(World *w, int sizex, int sizey, WinQuad domwq);
void worldGLInit();
void worldLoad();

void worldUpdate();
void worldRender(World *w, WinQuad wq);
void worldOrthHandleButton(World *w, int iButton, int iState, int iX, int iY,
			   OrientMode om, WinQuad wq);
void worldOrthHandleMotion(World *w, int iX, int iY,OrientMode om, WinQuad wq);
void worldPerHandleButton(World *w, int iButton, int iState, int iX, int iY,
			  OrientMode om, WinQuad wq);
void worldPerHandleMotion(World *w, int iX, int iY, OrientMode om, WinQuad wq);

void worldToggleDrawAxes(World *w); 
void worldToggleProjection(World *w);  
void worldToggleSolid(World *w); 
void worldToggleColorScheme(World *w);
void worldTogglePoints(World *w);
void worldToggleBControl(World *w); 
void worldIncLevel(World *w); 
void worldDecLevel(World *w); 
void worldIncSubdiv(World *w); 
void worldDecSubdiv(World *w); 

void worldSetViewProj(World *w, WinQuad wq, int iX, int iY, GLenum mode);

void uiptsRender(World *w);
void uiptsRenderCore(World *w, GLenum mode);
int uiptsSelect(World *w, int iX, int iY, int width, int height, WinQuad wq);

void orInit(Orient *o, WinQuad wq);
void orCaptureDown(Orient *o);
void orUnCaptureDown(Orient *o);
void worldSetViewports(World *w, int sizex, int sizey, WinQuad domwq);
void worldGetMoveVector(int iDX, int iDY, int width, int height, WinQuad wq, 
			float aspect, Orient *o, Point pt, Vector v);
void worldSaveMesh(World *w, char *fname);
void worldSavePOVMesh(World *w, char *fname);
uipList worldReadMesh(World *w,char *readname);
void worldMakeAxesList(GLuint list);
void patchPOVOutput(GLfloat pts[4][4][3], int level, int subdiv, FILE *f);

void worldRenderDecor(World *w, int width, int height, OrientMode om,
		      char *message);
void decorWrite(float rx, float ry, int width, int height, const char *str);


#endif /* __WORLD_H__ */
