/*
*  mathlib.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#include <math.h>

#ifndef BOOL
#define BOOL int
#endif /* BOOL */

#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */

#ifndef PI
#define PI 3.14159265358979323846
#endif /* PI */

#ifndef MIN
#define MIN(a,b) (a<b) ? (a) : (b)
#endif /* MIN */

#ifndef MAX
#define MAX(a,b) (a>b) ? (a) : (b)
#endif /* MAX */

#define SIZ 4

typedef float Vector[SIZ];
typedef Vector Point;
typedef Vector Quater;
typedef float Matrix[16];



#include "vector.h"
#include "matrix.h"


#endif /* __MATHLIB_H__ */
