/*
*  main.c - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#include "project.h"
#include "window.h"
#include "error.h"

/* Globals */

Window win;

int main(int argc, char** argv)
{
  if ( !winInit(argc, argv) )
    Error("Window failed to Init ... shuting down.");
  winPrintHello();

  /* This is the I/O Loop which drives the app */
  winMainLoop();

  return 0;
}







