/*
*  matrix.c - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#include "matrix.h"



/* These functions initialize the matrix to a certain form. */
/* All functions that take a vector as an argument ignore the 
   w coordinate, using only x, y, and z values. */
void mSetNull(Matrix m)
{
  int i;

  for (i=0; i<16; i++)
    m[i] = 0.0;
}

void mSetIdentity(Matrix m)
{
  int i;

  for (i=0; i<16; i++)
    m[i] = 0.0;
  m[0] = 1.0;
  m[5] = 1.0;
  m[10] = 1.0;
  m[15] = 1.0;
}

void mSetTranslate(Matrix m, Vector v)
{
  mSetIdentity(m);
  m[12] = v[0];
  m[13] = v[1];
  m[14] = v[2];
}

void mSetCross(Matrix m, Vector v)
{
  mSetNull(m);

  m[4] = -v[2];
  m[1] = v[2];
  m[8] = v[1];
  m[2] = -v[1];
  m[9] = -v[0];
  m[6] = v[0];
}

/* This function assumes v is a normal vector. */
void mSetRotate(Matrix m, Vector v, double dTheta)
{
  /* Rodrigues Formula */
  
  Matrix I, mtemp, mtemp2, mtemp3;
  float sint, cost;
  
  sint = (float)sin(dTheta);
  cost = (float)cos(dTheta);
  
  /*
    u = v;
    u.Normalize();
    */

  mSetCross(mtemp,v);
  mSetIdentity(I);
  
  /*   The following implements this.  */
  /*   m = I + (mtemp*sint) + (mtemp*mtemp*(1.0f - cost)); */
  mScale(mtemp,sint,mtemp2);
  mAdd(I,mtemp2,mtemp2);
  
  mmMult(mtemp,mtemp,mtemp3);
  mScale(mtemp3,1.0f - cost,mtemp3);
  mAdd(mtemp2,mtemp3,m);
}

void mSetRotateX(Matrix m, double dTheta)
{
  mSetIdentity(m);
  m[5] = (float)cos(dTheta);
  m[9] = (float)(-sin(dTheta));
  m[6] = (float)sin(dTheta);
  m[10] = (float)cos(dTheta);
}

void mSetRotateX2(Matrix m, float sint, float cost)
{
  mSetIdentity(m);
  m[5] = cost;
  m[9] = -sint;
  m[6] = sint;
  m[10] = cost;
}

void mSetRotateY(Matrix m, double dTheta)
{
  mSetIdentity(m);
  m[10] = (float)cos(dTheta);
  m[2] = (float)(-sin(dTheta));
  m[8] = (float)sin(dTheta);
  m[0] = (float)cos(dTheta);
}

void mSetRotateY2(Matrix m, float sint, float cost)
{
  mSetIdentity(m);
  m[10] = cost;
  m[2] = -sint;
  m[8] = sint;
  m[0] = cost;
}

void mSetRotateZ(Matrix m, double dTheta)
{
  mSetIdentity(m);
  m[0] = (float)cos(dTheta);
  m[4] = (float)(-sin(dTheta));
  m[1] = (float)sin(dTheta);
  m[5] = (float)cos(dTheta);
}

void mSetRotateZ2(Matrix m, float sint, float cost)
{
  mSetIdentity(m);
  m[0] = cost;
  m[4] = -sint;
  m[1] = sint;
  m[5] = cost;
}

void mSetScale(Matrix m, Vector v)
{
  int i;

  for (i=0; i<16; i++)
    m[i] = 0.0;
  m[0] = v[0];
  m[5] = v[1];
  m[10] = v[2];
  m[15] = 1.0;
}








/* These get/set the value of Matrix m at element row/column. */
/* To maintain consistency with Vectors, the upper-left element
   is indexed by (0,0) and not (1,1).  */
float mGet(Matrix m, int row, int col)
{
  return(m[col*4 + row]);
}

void mSet(Matrix m, int row, int col, float val)
{
  m[col*4 + row] = val;
}





/* These perform the indicated operations with Matrices, and return 
   results in the last argument.  In all cases (EXCEPT MULTIPLY), the
   destination argument can be one of the source arguments. */

void mTranspose(Matrix m1, Matrix mdest)
{
  float temp;
  int i,j;

  for (i=0; i<4; i++) {
    for (j=0; j<=i; j++) {
      /* row i, col j, gives us the lower left half (with diagonal). */
      temp = mGet(m1,i,j);
      mSet(mdest,i,j,mGet(m1,j,i));
      mSet(mdest,j,i,temp);
    }
  }
}

void mAdd(Matrix m1, Matrix m2, Matrix mdest)
{
  int i;

  for (i=0; i<16; i++) {
    mdest[i] = m1[i] + m2[i];
  }
}

void mSubtr(Matrix m1, Matrix m2, Matrix mdest)
{
  int i;

  for (i=0; i<16; i++) {
    mdest[i] = m1[i] - m2[i];
  }
}

void mmMult(Matrix m1, Matrix m2, Matrix mdest)
{
  int i,j,k;
  float accum;

  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      accum = 0.0;
      for (k=0; k<4; k++) {
	accum += mGet(m1,i,k)*mGet(m2,k,j);
      }
      mSet(mdest,i,j,accum);
    }
  }
}

void mvMult(Matrix m1, Vector v2, Vector vdest)
{
  int i,j;
  float accum;

  for (i=0; i<4; i++) {
    accum = 0.0;
    for (j=0; j<4; j++) {
      accum += mGet(m1,i,j)*v2[j];
    }
    vdest[i] = accum;
  }
}

void mScale(Matrix m1, float scale, Matrix mdest)
{
  int i;

  for (i=0; i<16; i++) {
    mdest[i] = m1[i]*scale;
  }
}

void mCopy(Matrix m1, Matrix mdest)
{
  int i;

  for (i=0; i<16; i++) {
    mdest[i] = m1[i];
  }
}





/* This displays Matrix m1 to stdout. */
void mDisplay(Matrix m1)
{
  int i,j;

  for (i=0; i<4; i++) {
    if (i == 0)
      fprintf(stdout,"Matrix = <  ");
    else
      fprintf(stdout,"         <  ");
    for (j=0; j<4; j++) {
      fprintf(stdout,"%3.2f  ",mGet(m1,i,j));
    }
    fprintf(stdout,">\n");
  }
}


