/*
*  matrix.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <stdio.h>
#include "mathlib.h"

/*
  A matrix type.  The Representation is the same as an OpenGL Matrix, it
  is an array of 16 floats.  Ordered the following way:

  0  4  8 12
  1  5  9 13
  2  6 10 14
  3  7 11 15

  Also note that these matrices multiply column vectors that are on the
  right of the matrix.

  You can access the elements of this array with the functions mGet
  and mSet, which use row/column numbers and also do bounds checking.
  
  */


/* These functions initialize the matrix to a certain form. */
/* All functions that take a vector as an argument ignore the 
   w coordinate, using only x, y, and z values. */
void mSetNull(Matrix m);
void mSetIdentity(Matrix m);
void mSetTranslate(Matrix m, Vector v);
/* This function assumes v is a normal vector. */
void mSetRotate(Matrix m, Vector v, double dTheta);
void mSetRotateX(Matrix m, double dTheta);
void mSetRotateX2(Matrix m, float sint, float cost);
void mSetRotateY(Matrix m, double dTheta);
void mSetRotateY2(Matrix m, float sint, float cost);
void mSetRotateZ(Matrix m, double dTheta);
void mSetRotateZ2(Matrix m, float sint, float cost);
void mSetScale(Matrix m, Vector v);

/* These get/set the value of Matrix m at element row/column. */
/* To maintain consistency with Vectors, the upper-left element
   is indexed by (0,0) and not (1,1).  */
float mGet(Matrix m, int row, int col);
void mSet(Matrix m, int row, int col, float val);

/* These perform the indicated operations with Matrices, and return 
   results in the last argument.  In all cases (EXCEPT mm/mvMult), the
   destination argument can be one of the source arguments. */
void mTranspose(Matrix m1, Matrix mdest);
void mAdd(Matrix m1, Matrix m2, Matrix mdest);
void mSubtr(Matrix m1, Matrix m2, Matrix mdest);
void mmMult(Matrix m1, Matrix m2, Matrix mdest);
void mvMult(Matrix m1, Vector v2, Vector vdest);
void mScale(Matrix m1, float scale, Matrix mdest);
void mCopy(Matrix m1, Matrix mdest);

/* This displays Matrix m1 to stdout. */
void mDisplay(Matrix m1);

#endif /* __MATRIX_H__ */
