/*
*  store.c - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#include <stdio.h>
#include "error.h"
#include "window.h"
#include "matrix.h"
#include "vector.h"
#include "mathlib.h"
#include "mesh.h"
#include "uiplist.h"
#include "project.h"
#define EOA(AA,xx,yy,rr)    AA[(xx)+(yy)*(rr)]

void savePoint(FILE *fp, Point p)
{
  int i;
  for(i=0;i<4;i++)
    fprintf(fp,"%.10f ",p[i]);
  fprintf(fp,"\n");
}

void readPoint(FILE *fp, Point p)
{
  int i;
  for(i=0;i<4;i++)
    fscanf(fp,"%f",&(p[i]));
}

void saveMesh(FILE *fp, Mesh *mp)
{
  int i,j, m,n;
  if(!mp) return;

  fprintf(fp,"%d %d %d %d %d\n",mp->level, mp->m, mp->n, mp->m0, mp->n0);
  fprintf(fp,"%f %f %f %f\n",mp->u0,mp->v0,mp->du,mp->dv);
  m = mp->m;
  n = mp->n;

  for(i=0; i< m+3; i++)
    for(j=0; j< n+3; j++)
      savePoint(fp, EOA((mp->ref),i,j,mp->m+3));
  for(i=0; i< m+3; i++)
    for(j=0; j< n+3; j++)
      savePoint(fp, EOA((mp->off),i,j,mp->m+3));

  if(mp->lower){
    fprintf(fp,"%d\n",-1);
    saveMesh(fp,mp->lower);
  }

  if(mp->next){
    fprintf(fp,"%d\n",0);
    saveMesh(fp,mp->next);
  }

  fprintf(fp,"%d\n",1);
}

extern void getSurfPoint(Mesh *,int,int,float,float,Point);

Mesh *readNMesh(FILE *fp, Mesh *mp, uipList ul)
{
  int level,i,j,m,n;
  int p0,q0,p1,q1;
  Mesh *tmp;
  UIPoint *uip;

  if(!mp) return NULL;
  fscanf(fp,"%d%d%d",&level,&m,&n);
  tmp = allocMesh(m,n);
  tmp -> level = level;
  fscanf(fp,"%d%d",&(tmp->m0),&(tmp->n0));
  fscanf(fp,"%f%f%f%f",&(tmp->u0),&(tmp->v0),&(tmp->du),&(tmp->dv));
  
  for(i=0; i< m+3; i++)
    for(j=0; j< n+3; j++)
      readPoint(fp, EOA((tmp->ref),i,j, tmp->m+3));
  for(i=0; i< m+3; i++)
    for(j=0; j< n+3; j++)
      readPoint(fp, EOA((tmp->off),i,j, tmp->m+3));

  getPolyRep(tmp);

  for(i=2;i<=tmp->m-2;i++)
    for(j=2;j<=tmp->n-2;j++)
        if(i%2 || j%2){
            EOA((tmp->uipts),i,j,tmp->m+1)= getUIPoint(tmp,i,j,0.0,0.0);
            uiplAppend(ul,EOA((tmp->uipts),i,j,tmp->m+1));
        }
        else{
          EOA((tmp->uipts),i,j,tmp->m+1)=
          EOA((mp->uipts),tmp->m0+i/2,tmp->n0+j/2,mp->m+1);
          uip=EOA((tmp->uipts),i,j,tmp->m+1);
          /* Use (UIPoint)-1 to indicate this uiPoint has been moved to a lower level.*/
          EOA((mp->uipts),tmp->m0+i/2,tmp->n0+j/2,mp->m+1) = (UIPoint *)-1;
          getSurfPoint(tmp,i,j,0.0,0.0, uip->pt);
          uip -> mesh = tmp;
          uip -> m = i;
          uip -> n = j;
        }

  tmp-> upper = mp;

  p0 = MAX(tmp->m0,0);
  q0 = MAX(tmp->n0,0);
  p1 = MIN(tmp->m0+tmp->m/2, mp->m);
  q1 = MIN(tmp->n0+tmp->n/2, mp->n);
  for(i=p0; i<p1; i++)
    for(j=q0; j<q1; j++)
        EOA((mp->mask),i,j,mp->m) = tmp;

  fscanf(fp,"%d",&level);
  while(level != 1){
    switch(level){
    case -1:
      tmp-> lower = readNMesh(fp, tmp, ul);
      break;
    case  0:
      tmp-> next = readNMesh(fp, mp, ul);
      tmp-> next-> prev = tmp;
      break;
    }
    fscanf(fp,"%d",&level);
  }
  return tmp;
}

extern void getTranspose();

uipList readMesh(FILE *fp, Mesh **mp)
{
  int level,i,j,m,n;
  uipList ul;
  UIPoint *uip;

  getTranspose();
  fscanf(fp,"%d%d%d",&level,&m,&n);
  *mp = allocMesh(m,n);
  (*mp) -> level = level;
  fscanf(fp,"%d%d",&((*mp)->m0),&((*mp)->n0));
  fscanf(fp,"%f%f%f%f",&((*mp)->u0),&((*mp)->v0),&((*mp)->du),&((*mp)->dv));
  
  for(i=0; i< m+3; i++)
    for(j=0; j< n+3; j++)
      readPoint(fp, EOA(((*mp)->ref),i,j, (*mp)->m+3));
  for(i=0; i< m+3; i++)
    for(j=0; j< n+3; j++)
      readPoint(fp, EOA(((*mp)->off),i,j, (*mp)->m+3));

  getPolyRep(*mp);

  ul = uiplNewList();

  for(i=0; i<m+1; i++)
    for(j=0; j<n+1; j++){  
      uip = getUIPoint(*mp, i,j, 0.0,0.0);
      EOA(((*mp)->uipts), i,j,(*mp)->m+1) = uip;
      uiplAppend(ul,uip);
    }

  fscanf(fp,"%d",&level);
  while(level != 1){
    switch(level){
    case -1:
      (*mp)->lower = readNMesh(fp,*mp,ul);
      break;
    case  0:
      (*mp)->next = readNMesh(fp,NULL,ul);
      break;
    }
    fscanf(fp,"%d",&level);
  }

  return ul;
}
































