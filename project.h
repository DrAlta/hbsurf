/*
*  project.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#ifndef __PROJECT_H__
#define __PROJECT_H__


#include <GL/gl.h>
#include <GL/glu.h>

/* Use this if HPUX and OpenGL */
/*#  include <GL/gl_pgi.h> */

/* Otherwise, set it to a no-op */
#define glHintPGI(target,mode)

#include "mathlib.h"


/* These are definitions related to the mesh object */

typedef struct _Mesh Mesh;

typedef struct _UIPoint {
  Point pt;   /* This is the x,y,z point that the user controls. */
  Mesh *mesh; /* This is a ptr to the MOST REFINED mesh that pt lies in. */
  int m;      /* This is the 1st index of pt in the mesh's point array. */
  int n;      /* This is the 2nd index of pt in the mesh's point array. */
} UIPoint;

struct _Mesh {
  int level;   /* The level of this mesh. */
  int m;       /* The number of patches in a line of this mesh (along u). */
  int n;       /* The number of patches in a line of this mesh (along v). */
  int m0;      /* The starting index along u in upper level. */
  int n0;
  Point *ref;  /* An Array of Reference points (one for each m,n). */
  Point *off;  /* An Array of Offset points (one for each m,n). */
  Matrix *poly[3]; /* Each element represents the polynomial coefficients 
                      each patch.*/

  UIPoint **uipts; /*Array of ptrs to a UIPoint (one for each m,n). */
  float u0;    /* The first u knot value used by this mesh.  */
  float v0;    /* The first v knot value used by this mesh.  */
  float du;
  float dv;
  Mesh **mask;   /* Mask array (1 = surf def'd, 0 = surf def'd in other mesh.) */
  Mesh *upper; /* Ptr to parent Mask (NULL if no parent). */
  Mesh *lower; /* Ptr to child Mask (NULL if no children). */
  Mesh *prev;  /* Ptr to previous sibling (NULL if no previous sibling). */
  Mesh *next;  /* Ptr to next sibling (NULL if no next sibling). */
};

#include "uiplist.h"




/*  Here are a number of defines related to the world.c. */


typedef enum _WinQuad {
  wqTop = 0,   /* This is the upper-left  quadrant */
  wqFront = 1, /* This is the upper-right quadrant */
  wqSide = 2,  /* This is the lower-left quadrant */
  wqPer  = 3,  /* This is the lower-right quadrant */
  wqNone = 4   /* This indicates No quadrant */
} WinQuad;

typedef enum _SolidType {
  stWire = 0, 
  stSolid = 1
} SolidType;

typedef enum _ProjectionType {
  pjtParallel = 0, 
  pjtPerspective = 1
} ProjectionType;

typedef enum _BControl {
  bcSelect = 0, 
  bcMove = 1
} BControl;

typedef struct _Orient {
  Quater rot;
  float scale;
  Vector trans;
  Quater downrot;
  float downscale;
  Vector downtrans;
  int vpllx;
  int vplly;
  int vpwid;
  int vphei;
} Orient;

typedef struct _World {
  Orient Orients[4];
  
  Matrix TopProj;
  Matrix FrontProj;
  Matrix SideProj;
  Matrix PerProj;

  /* Polygon Drawing Attributes */
  SolidType Solid;

  /* Objects in our World */
  Mesh *surf;
  uipList uipl;
  UIPoint **uipts;
  int numuipts;
  int selected;
  GLuint AxesList;

  /* UI Suff */
  int level;
  BOOL DrawAxes;
  BOOL MultiColor;
  BOOL ConPoints;
  BControl BCMode;
  int CurrButton;
  int StartX;
  int StartY;
  int LastX;
  int LastY;
  float Aspect;
  int Subdiv;

  /* Camera Stuff */
  ProjectionType Projection;  
  
} World;




/*  Here are some defines relevent to window.h.  */

#define WIN_SIZE_X 1000
#define WIN_SIZE_Y 810

typedef enum _OrientMode {
  omBall = 0, 
  omScale = 1,
  omSpin = 2,
  omTrans = 3
} OrientMode;

typedef struct _Window {
  int WindowID;
  int RedisplayWindowID;
  BOOL AllowIdle;
  /* Rect m_rWindow; */
  int SizeX;
  int SizeY;
  BOOL OrientedX;
  BOOL OrientedY;
  World world;
  WinQuad wqdown;
  WinQuad wqdom;
  OrientMode om;
  char *ReadName;
  char message[64];
} Window;





/* This is the invalid mouse button (for GLUT) */
#define INVALID_BUTTON -1
#define VIEW_ANGLE     60
#define DIST_FROM_EYE   3
#define DIST_TO_CLIP  1.5

#define PORT_TOP_HI   .97
#define PORT_TOP_LOW  .58
#define PORT_BOT_HI   .55
#define PORT_BOT_LOW  .16

extern Point lev_colors[4];

extern Point pt_color;
extern Point selpt_color;
extern Point axes_color;

#endif /* __PROJECT_H__ */
