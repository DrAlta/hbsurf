/*
*  error.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

/*
  This is a simple library of error handling routines.
*/

#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdio.h>

/* This prints the error message given, and exits with an argument of -1.*/
void Error(char *message);

/* This prints the warning message given, and continues. */
void Warning(char *message);

/* This closes the application. */
void Close();

#endif /* __ERROR_H__ */
