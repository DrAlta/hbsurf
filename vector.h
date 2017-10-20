/*
*  vector.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

/*
  A simple point/vector type.  This type is simply an array of 4 floats,
  which is just what OpenGL uses for points and vectors.  Here, points 
  vectors are the same objects, and it is up to the user to keep it 
  straight which is which.  Some operations are labelled with a
  preceeding "pt" instead of "v" (such as ptHomogenize) just because
  those are functions normally performed on points.
  */


#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdio.h>
#include "mathlib.h"

/* This initializes a vector to all 0's. */
void vZero(Vector v1);
void vSet(Vector v,float w,float x,float y,float z);

/* These add/subtract the elements of v1 and v2 and places them in vdest.  
   Either v1 or v2 can also be vdest.  */
void vAdd(Vector v1, Vector v2, Vector vdest);
void vSubtr(Vector v1, Vector v2, Vector vdest);

/* This computes v1(Cross)v2 and places the result in vdest.
   Here, Neither v1 nor v2 can be vdest. */
void vCross(Vector v1, Vector v2, Vector vdest);

/* This computes the dot product of v1 and v2 and returns it. */
float vDot(Vector v1, Vector v2);

/* This tests if the elements of v1 and v2 are equal. */
int vEqual(Vector v1, Vector v2);

/* This modifies v1 by multiplying each element by scale. */
void vScale(Vector v1, float scale);

/* This Normalizes the vector v1 and returns 1 if the length of v1 is 
   not 0.  Otherwise, it returns 0 and leaves v1 unchanged. */
int vNormalize(Vector v1);

/* This returns the magnitude of v1. */
float vMagnitude(Vector v1);

/* This copies the elements of v1 and places them in vdest. */
void vCopy(Vector v1, Vector vdest);

/* This displays a string of the elements of v1 
   (to stdout with a newline). */
void vDisplay(Vector v1);

/* This initializes a point to 0 (i.e. <0, 0, 0, 1>). */
void ptZero(Point p1);
void ptSet(Point p,float x,float y,float z);

/* If the w coordinate of p1 is not 0, returns 1 and homogenizes p1.
   Otherwise, returns 0. */
int ptHomogenize(Point p1);



/* These are Quaternion functions, which are also simply represented
   as a vector. */

/* This sets the q to represent a rotation of theta about v. */
void qtSet(Quater q, double theta, Vector v);

/* This sets m to represent the same rotation as q */
void qtRotMatrix(Quater q, Matrix m);

/* This sets qdest to the inverse rotation of q (q & qdest can be the same) */
void qtConjugate(Quater q, Quater qdest);

/* This sets qdest to q1*q2 (a rot by q2, THEN q1) (qdest can be q1 or q2) */
void qtMult(Quater q1, Quater q2, Quater qdest);


#endif /* __VECTOR_H__ */
