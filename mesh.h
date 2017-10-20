/*
*  mesh.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#ifndef __MESH_H__
#define __MESH_H__

#include "project.h"
#include "uiplist.h"  
#include "error.h"     
#include "vector.h"
#include "matrix.h"    /* These last four header files contain useful
			  Function prototypes. */

/* The following four functions form the interface between the mesh
   object and the rest of the program. */


/* This function takes a pointer to a Mesh pointer and modifies the 
   Mesh Pointer to point to a freshly allocated Mesh object with 
   m*n patches (m is in the u param, n is in the v param).  The 
   function also creates and returns a new uipList object containing 
   pointers to an initial set of User Interface points.  */
uipList meshInit(Mesh **mesh, int m, int n);



/* This instructs the mesh to move a control point.  It is called
   whenever a user moves the UIPoint that is contained in "mesh" at
   index (m,n) (remember, this is the mesh at the "most refined" level 
   for that UIPoint).  The user's current level, the level at which the
   UIPoint is moved, is given in "level"  The vector along which the
   UIPoint is moved is given in "v". */
void meshMove(Mesh *mesh, int m, int n, int level, Vector v);



/* This instructs the mesh to add one more level of refinement around
   the UIPoint that is contained in "mesh" at index (m,n) (remember,
   this is the mesh at the "most refined" level for that UIPoint).
   If this causes any new User Interface Points to be created, the
   function also creates and returns a new uipList object containing 
   pointers to the additional UIPoints.  If there are no new UIPoints,
   NULL is returned. */
uipList meshRefine(Mesh *mesh, int m, int n);



/* This instructs the mesh to render itself.  It relies on the function
   patchRender (whos definition appears in "window.h") to render
   each patch. */
void meshRender(Mesh *mesh, SolidType st, int subdiv, BOOL MulitC,
		BOOL pov, FILE *f);


extern Mesh *allocMesh(int,int);
extern void getPolyRep(Mesh *);
extern UIPoint *getUIPoint(Mesh *,int,int,float,float);


/* The remaining functions are helper functions for the above functions. */



#endif /* __MESH_H__ */



