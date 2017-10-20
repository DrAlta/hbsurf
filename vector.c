/*
*  vector.c - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#include "vector.h"


/* This initializes a vector to all 0's. */
void vZero(Vector v1)
{
  int i;

  for (i=0; i<SIZ; i++) {
    v1[i] = 0.0;
  }
}

void vSet(Vector v,float w,float x,float y,float z)
{
  v[0]=w;
  v[1]=x;
  v[2]=y;
  v[3]=z;
}



/* These add/subtract the elements of v1 and v2 and places them in vdest.  
   Either v1 or v2 can also be vdest.  */
void vAdd(Vector v1, Vector v2, Vector vdest)
{
  int i;

  for (i=0; i<SIZ; i++)
    vdest[i] = v1[i] + v2[i];
}

void vSubtr(Vector v1, Vector v2, Vector vdest)
{
  int i;

  for (i=0; i<SIZ; i++)
    vdest[i] = v1[i] - v2[i];
}




/* This computes v1(Cross)v2 and places the result in vdest.
   Here, Neither v1 nor v2 can be vdest. */
void vCross(Vector v1, Vector v2, Vector vdest)
{
  vdest[0] = v1[1]*v2[2] - v1[2]*v2[1];
  vdest[1] = v1[2]*v2[0] - v1[0]*v2[2];
  vdest[2] = v1[0]*v2[1] - v1[1]*v2[0];
  vdest[3] = 0;
}



/* This computes the dot product of v1 and v2 and returns it. */
float vDot(Vector v1, Vector v2)
{
  int i;
  float d = 0.0;

  for (i=0; i<SIZ; i++)
    d += v1[i] * v2[i];
  return d;
}



/* This tests if the elements of v1 and v2 are equal. */
int vEqual(Vector v1, Vector v2)
{
  return(v1[0] == v2[0] &&
	 v1[1] == v2[1] &&
	 v1[2] == v2[2] &&
	 v1[3] == v2[3]);

}



/* This modifies v1 by multiplying each element by scale. */
void vScale(Vector v1, float scale)
{
  int i;

  for (i=0; i<SIZ; i++)
    v1[i] = v1[i]*scale;
}



/* This Normalizes the vector v1 and returns 1 if the length of v1 is 
   not 0.  Otherwise, it returns 0 and leaves v1 unchanged. */
int vNormalize(Vector v1)
{
  int i;
  float mag = 0.0;
  
  for (i=0; i<SIZ; i++)
    mag += v1[i] * v1[i];
  mag = (float)sqrt((double)mag);

  if (mag == 0.0)
    return 0;

  for (i=0; i<SIZ; i++)
    v1[i] = v1[i]/mag;
  return 1;
  
}



/* This returns the magnitude of v1. */
float vMagnitude(Vector v1)
{
  int i;
  float mag = 0.0;
  
  for (i=0; i<SIZ; i++)
    mag += v1[i] * v1[i];
  return(sqrt(mag));
}



/* This copies the elements of v1 and places them in vdest. */
void vCopy(Vector v1, Vector vdest)
{
  int i;
  
  for (i=0; i<SIZ; i++)
    vdest[i] = v1[i]; 
}



/* This displays a string of the elements of v1 
   (to stdout with a newline). */
void vDisplay(Vector v1)
{
  fprintf(stdout,"<%.2f %.2f %.2f %.2f>\n",v1[0],v1[1],v1[2],v1[3]);
}






/* This initializes a point to 0 (i.e. <0, 0, 0, 1>). */
void ptZero(Point p1)
{
  p1[0] = 0.0;
  p1[1] = 0.0;
  p1[2] = 0.0;
  p1[3] = 1.0;
}

void ptSet(Point p,float x,float y,float z)
{
  p[0]=x;
  p[1]=y;
  p[2]=z;
  p[3]=1;
}


/* If the w coordinate of p1 is not 0, returns 1 and homogenizes p1.
   Otherwise, returns 0. */
int ptHomogenize(Point p1)
{
  int i;

  if (p1[3] == 1.0)
    return(1);
  if (p1[3] == 0.0)
    return(0);
  
  for (i=0; i<SIZ; i++)
    p1[i] = p1[i]/p1[3];
  return(1);
}


/* These are Quaternion functions, which are also simply represented
   as a vector. */


/* This sets the q to represent a rotation of theta about v. */
void qtSet(Quater q, double theta, Vector v)
{
  vCopy(v,q);
  vScale(q,sin(theta/2));
  q[3] = cos(theta/2);
}


/* This sets m to represent the same rotation as q */
void qtRotMatrix(Quater q, Matrix m)
{
  float w = q[3];
  float x = q[0];
  float y = q[1];
  float z = q[2];

  m[0] = (w*w)+(x*x)-(y*y)-(z*z);
  m[4] = 2*x*y-2*w*z;
  m[8] = 2*x*z+2*w*y;
  m[12] = 0;
  
  m[1] = 2*x*y+2*w*z;
  m[5] = (w*w)-(x*x)+(y*y)-(z*z);
  m[9] = 2*y*z-2*w*x;
  m[13] = 0;
  
  m[2]= 2*x*z-2*w*y;
  m[6] = 2*y*z+2*w*x;
  m[10] = (w*w)-(x*x)-(y*y)+(z*z);
  m[14] = 0;
  
  m[3] = 0;
  m[7] = 0;
  m[11] = 0;
  m[15] = (w*w)+(x*x)+(y*y)+(z*z);
}



/* This sets qdest to the inverse rotation of q (q & qdest can be the same) */
void qtConjugate(Quater q, Quater qdest)
{
  qdest[0] = -q[0];
  qdest[1] = -q[1];
  qdest[2] = -q[2];
  qdest[3] = q[3];
}


/* This sets qdest to q1*q2 (a rot by q2, THEN q1) (qdest can be q1 or q2) */
void qtMult(Quater q1, Quater q2, Quater qdest)
{
  Vector v1,v2;
  float w1, w2, dot;

  vCopy(q1,v1);
  v1[3] = 0;
  vCopy(q2,v2);
  v2[3] = 0;
  w1 = q1[3];
  w2 = q2[3];

  vCross(v1,v2,qdest);
  dot = vDot(v1,v2);
  vScale(v1,w2);
  vScale(v2,w1);
  vAdd(qdest,v1,qdest);
  vAdd(qdest,v2,qdest);
  qdest[3] = w1*w2 - dot; 
}



