/*
*  test.c - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#include <stdlib.h>
#include <stdio.h>
#include "mathlib.h"


void main(int argc, char **argv)
{
  Matrix m, m2, m3;
  Vector v,v2,x,y,z;
  
  v[0] = 1.0;
  v[1] = 2.0;
  v[2] = 3.0;
  v[3] = 4.0;

  vZero(x);
  x[0] = 1.0;
  vZero(y);
  y[1] = 1.0;
  vZero(z);
  z[2] = 1.0;

  mSetNull(m);
  mDisplay(m);
  fprintf(stdout,"\n");

  mSetIdentity(m);
  mDisplay(m);
  fprintf(stdout,"\n");

  /* 
  mSetRotateX(m,PI/2);
  mDisplay(m);
  fprintf(stdout,"\n");

  mSetRotateY(m,PI/2);
  mDisplay(m);
  fprintf(stdout,"\n");

  mSetRotateZ(m,PI/2);
  mDisplay(m);
  fprintf(stdout,"\n");
  */

  /*
  mSetRotateX2(m,1,2);
  mDisplay(m);
  fprintf(stdout,"\n");

  mSetRotateY2(m,1,2);
  mDisplay(m);
  fprintf(stdout,"\n");

  mSetRotateZ2(m,1,2);
  mDisplay(m);
  fprintf(stdout,"\n");
  */

  mSetScale(m,v);
  mDisplay(m);
  fprintf(stdout,"\n");

  mSetTranslate(m2,y);
  mDisplay(m2);
  fprintf(stdout,"\n");

  mmMult(m2,m,m3);
  mDisplay(m3);
  fprintf(stdout,"\n");

  v[0] = 1.0;
  v[1] = 1.0;
  v[2] = 0.0;
  v[3] = 0.0;
  vNormalize(v);
  vDisplay(v);

  mSetRotate(m,v,PI/2);
  mDisplay(m);
  fprintf(stdout,"\n");

  mvMult(m,x,v2);
  vDisplay(v2);
  
  mTranspose(m,m);
  mDisplay(m);
  fprintf(stdout,"\n");
  
  mCopy(m,m2);
  mSetIdentity(m);
  mSubtr(m2,m,m);
  mDisplay(m);
  fprintf(stdout,"\n");
  


}




/*
void displaylist(intList list) 
{
  int i;

  fprintf(stdout,"list");
  for(i = intlIterFirst(list); intlIterMore(list); i = intlIterNext(list)) {
    fprintf(stdout,"->%d",i);
  }
  fprintf(stdout,"\n");
}

void main(int argc, char **argv)
{
  intList a,b,c;

  a = intlNewList();
  b = intlNewList();
  c = intlNewList();

  intlAppend(a,1);
  intlAppend(a,2);
  intlPrepend(a,3);

  displaylist(a);
  displaylist(a);
  intlDelFirst(a);

  intlPrepend(b,4);
  intlAppend(b,5);
  intlAppend(b,6);
  displaylist(a);
  displaylist(b);

  intlCat(a,b);
  displaylist(a);
  displaylist(b);

  if (intlEmpty(b))
    printf("b empty!\n");
  if (intlEmpty(a))
    printf("a empty!\n");

  printf("List a first = %d\n",intlFirst(a));
  printf("List a last = %d\n",intlLast(a));
  printf("List a delfirst = %d\n",intlDelFirst(a));
  displaylist(a);

  intlFreeList(a);
  intlFreeList(b);
  intlFreeList(c);

}

*/
