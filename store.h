/*
*  store.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#ifndef __STORE_H__
#define __STORE_H__

void saveMesh(FILE *fp, Mesh *mp);
uipList readMesh(FILE *fp, Mesh **mp);

#endif /* __STORE_H__ */
