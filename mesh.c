/*
*  mesh.c - for HBSURF hierarchical B-Spline Surface Modeler
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
void getRef(Mesh *, int, int);


Matrix Bsp2Poly={1.0/6, -0.5, 0.5, -1.0/6,
                 4.0/6,    0,  -1,    0.5,
                 1.0/6,  0.5, 0.5,   -0.5,
                     0,    0,   0,  1.0/6
};

Matrix Bsp2PolyT;

Matrix Poly2Bez={    1,     1,     1,     1,
                     0, 1.0/3, 2.0/3,     1,
                     0,     0, 1.0/3,     1,
                     0,     0,     0,     1
};

Matrix Poly2BezT;

Matrix Bsp2Bez={1.0/6,     0,     0,     0,
                4.0/6, 4.0/6, 2.0/6, 1.0/6,
                1.0/6, 2.0/6, 4.0/6, 4.0/6,
                    0,     0,     0, 1.0/6
};

Matrix Bsp2BezT;

Matrix DisBsp1={0.5, 1.0/8,   0,     0,
                0.5,  0.75, 0.5, 1.0/8,
                  0, 1.0/8, 0.5,  0.75,
                  0,     0,   0, 1.0/8
};

Matrix DisBsp1T;

Matrix DisBsp2={1.0/8,   0,   0,   0,
                 0.75, 0.5, 1.0/8,  0,
                1.0/8, 0.5, 0.75, 0.5,
                    0,   0, 1.0/8, 0.5
};

Matrix DisBsp2T;

/* This allocates memory for a new mesh node and
   allocates memory for each data member
   inside, resets pointers to be NULL.
*/

Mesh * allocMesh(int m, int n)
{
  Mesh *tm;
  int i,j;

  tm = (Mesh *)malloc(sizeof(Mesh));
  tm -> ref = (Point *)malloc((m+3)*(n+3)*sizeof(Point));
  tm -> off = (Point *)malloc((m+3)*(n+3)*sizeof(Point));
  /* tm -> bzpts = (Matrix *)malloc(m*n*sizeof(Matrix));*/
  for(i=0;i<3;i++)
  tm -> poly[i] = (Matrix *)malloc((m+2)*(n+2)*sizeof(Matrix));
  tm -> uipts = (UIPoint **)malloc((m+1)*(n+1)*sizeof(UIPoint *));
  tm -> mask = (Mesh **)malloc(m*n*sizeof(Mesh *));
  for(i=0;i<m;i++)
    for(j=0;j<n;j++)
      EOA((tm->mask),i,j,m) = 0;
  for(i=0;i<=m;i++)
    for(j=0;j<=n;j++)
      EOA((tm->uipts),i,j,m+1) = 0;
  tm->m = m;
  tm->n = n;
  tm -> upper = tm ->lower = tm -> prev = tm -> next = 0;
  return tm;
}

void freeMesh(Mesh *mp)
{
  int i;

  free(mp->ref);
  free(mp->off);
  free(mp->uipts);
  free(mp->mask);
  for(i=0;i<3;i++)
    free(mp->poly[i]);
  free(mp);
}

/*This converts deBoor points for each patch to polynomial representation.*/

void getPolyComp(Mesh *mp, int pi,int pj, int xyz, Matrix M)
{
  Matrix mt;
  int i,j;

  if(pi<-1 || pi>=mp->m+1)
    Error("In getPolyRep, invalid pi \n");
  if(pj<-1 || pj>=mp->n+1)
    Error("In getPolyRep, invalid pj \n");

  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      if(pi+i>=0 && pi+i<=mp->m+2 && pj+j>=0 && pj+j<=mp->n+2)
      M[i+j*4]= EOA((mp->ref),pi+i,pj+j,mp->m+3)[xyz] +
                EOA((mp->off),pi+i,pj+j,mp->m+3)[xyz];
      else
        M[i+j*4] = 0;
  mmMult(Bsp2Poly,M,mt);
  mmMult(mt,Bsp2PolyT,M);
}

void getPolyRep(Mesh *mp)
{
  int i,j,k;

  for(i=0;i<mp->m+2;i++)
    for(j=0;j<mp->n+2;j++)
      for(k=0;k<3;k++)
        getPolyComp(mp,i-1,j-1,k,EOA((mp->poly[k]),i,j,mp->m+2));
}

void getBezCompFromPoly(Matrix Mp,Matrix Mb)
{
  Matrix mt;

  mmMult(Poly2Bez,Mp,mt);
  mmMult(mt,Poly2BezT,Mb);
}


void getSurfPoint(Mesh *mp,int pi,int pj,float u,float v,Point p)
{
  Vector up,vp,tp;
  int i;

  if(pi<-1 || pi>= mp->m+1){
    Error("In getSurfPoint, invalid pi \n");
  }
  if(pj<-1 || pj>= mp->n+1){
    Error("In getSurfPoint, invalid pj \n");
  }
  if(u<0 || u>1.0)
    Error("In getSurfPoint, invalid para u \n");
  if(v<0 || v>1.0)
    Error("In getSurfPoint, invalid para v \n");

  vSet(up,1.0,u,u*u,u*u*u);
  vSet(vp,1.0,v,v*v,v*v*v);
  for(i=0;i<3;i++){
    mvMult(EOA((mp->poly[i]),pi+1,pj+1,mp->m+2),vp,tp);
    p[i] = up[0]*tp[0]+up[1]*tp[1]+up[2]*tp[2]+up[3]*tp[3];
  }
  p[3]=1.0;
}

UIPoint *getUIPoint(Mesh *mp,int pi,int pj,float u,float v)
{
  UIPoint *uip;

  uip = (UIPoint *)malloc(sizeof(UIPoint));
  uip -> mesh = mp;
  uip -> m = pi;
  uip -> n = pj;
  getSurfPoint(mp,pi,pj,u,v,uip->pt);
  return uip;
}

void getTranspose()
{
  mTranspose(Bsp2Poly,Bsp2PolyT);
  mTranspose(Poly2Bez,Poly2BezT);
  mTranspose(Bsp2Bez,Bsp2BezT);
  mTranspose(DisBsp1,DisBsp1T);
  mTranspose(DisBsp2,DisBsp2T);
}  
  

/* This function takes a pointer to a Mesh pointer and modifies the 
   Mesh Pointer to point to a freshly allocated Mesh object with 
   m*n patches (m is in the u param, n is in the v param).  The 
   function also creates and returns a new uipList object containing 
   pointers to an initial set of User Interface points.  */
uipList meshInit(Mesh **mesh, int m, int n)
{
  int i,j;
  uipList ul;
  UIPoint *uip;

  getTranspose();
  *mesh = allocMesh(m,n);
  (*mesh) -> level = 0;
  (*mesh) -> m = m;
  (*mesh) -> n = n;
  (*mesh) -> m0 = 0;
  (*mesh) -> n0 = 0;
  (*mesh) -> u0 = (*mesh) -> v0 = 0;
  (*mesh) -> du = 1.0/m;
  (*mesh) -> dv = 1.0/n;
  for(i=0;i<m+3;i++)
    for(j=0;j<n+3;j++)
      ptZero(EOA(((*mesh)->ref),i,j,m+3));
  for(i=0;i<m+3;i++)
    for(j=0;j<n+3;j++)
      vSet(EOA(((*mesh)->off),i,j,m+3),
           (1.0*i/(m+2))*2.0-1,  (1.0*j/(n+2))*2.0-1, 0, 0);

  getPolyRep(*mesh);

  ul = uiplNewList();
  for(i=0;i<=m;i++)
    for(j=0;j<=n;j++){
      uip = getUIPoint(*mesh, i,j,0.0,0.0);
      EOA(((*mesh)->uipts),i,j,(*mesh)->m+1)= uip;
      uiplAppend(ul, uip);
    }
  return ul;
}

void updateSurf(Mesh *mp, int m0,int n0,int m1,int n1)
{
  Mesh *tm;
  int i,j,k;
  int p0,q0,p1,q1;

  for(i=m0;i<m1;i++)
    for(j=n0;j<n1;j++){
      for(k=0;k<3;k++)
        getPolyComp(mp,i,j,k,EOA((mp->poly[k]),i+1,j+1,mp->m+2));
                                          /*Update surface polynomial*/
    } 
  p1=MIN(m1,mp->m);
  q1=MIN(n1,mp->n);
  for(i=MAX(m0,0);i<=p1;i++)
    for(j=MAX(n0,0);j<=q1;j++)
      if(EOA((mp->uipts),i,j,mp->m+1) && 
         EOA((mp->uipts),i,j,mp->m+1)!=(UIPoint *)-1){
        getSurfPoint(mp,i,j,0.0,0.0,EOA((mp->uipts),i,j,mp->m+1)->pt);
                                                  /*Update UI points*/
      }
           
  tm=mp->lower;
  while(tm){
    p0=(m0-tm->m0)*2;
    q0=(n0-tm->n0)*2;
    p1=(m1-tm->m0)*2;
    q1=(n1-tm->n0)*2;
    p0= MAX(p0,-1);
    q0= MAX(q0,-1);
    p1= MIN(p1,tm->m+1);
    q1= MIN(q1,tm->n+1);
    if(p0 < p1 && q0 < q1){
      for(i=p0;i<=p1;i++)
        for(j=q0;j<=q1;j++){
          getRef(tm,i,j);
	}
        updateSurf(tm,p0,q0,p1,q1);
    }
    tm=tm->next;
  }
}
 

  
/* This instructs the mesh to move a control point.  It is called
   whenever a user moves the UIPoint that is contained in "mesh" at
   index (m,n) (remember, this is the mesh at the "most refined" level 
   for that UIPoint).  The user's current level, the level at which the
   UIPoint is moved, is given in "level"  The vector along which the
   UIPoint is moved is given in "v". */
void meshMove(Mesh *mesh, int m, int n, int level, Vector v)
{
  /* printf(" in meshMove,level<%d>\n",level); */
  if(level <0)
    Error("In meshMove, invalid level \n");
  if(m<0 || m> mesh->m)
    Error("In meshMove, invalid m \n");
  if(n<0 || n> mesh->n)
    Error("In meshMove, invalid n \n");
  if( EOA((mesh->mask),m,n,mesh->m) && EOA((mesh->mask),m-1,n,mesh->m) &&
      EOA((mesh->mask),m,n-1,mesh->m) && EOA((mesh->mask),m-1,n-1,mesh->m))
    Warning("In meshMove, picked point is not at the top level. \n");
  if( mesh->level < level)
    Warning("In meshMove, this point is not allowed 
             to be picked at the current level.\n");
  else while(mesh->level != level){
    m = mesh->m0 + m/2;
    n = mesh->n0 + n/2;
    mesh= mesh->upper;
  }

  vScale(v,36.0/16);
  /* m ? or m+1 ? */
  vAdd(EOA((mesh->off),m+1,n+1,mesh->m+3),v,
       EOA((mesh->off),m+1,n+1,mesh->m+3));
  updateSurf(mesh,MAX(m-2,-1),MAX(n-2,-1),MIN(m+2,mesh->m+1),MIN(n+2,mesh->n+1));
 
}


void insertMesh(Mesh *mesh,Mesh *tm)
{
  if(!(mesh->lower))
    mesh->lower = tm;
  else{
    tm->next=mesh->lower;
    tm->next->prev= tm;
    mesh->lower = tm;
  }
}

void deleteMesh(Mesh *mesh,Mesh *tm)
{
  if(!(tm->prev)){
    mesh->lower = tm->next;
    if(mesh->lower)
      mesh->lower->prev=0;
  }
  else{
    tm->prev->next= tm->next;
    if(tm->next)
      tm->next->prev= tm->prev;
  }
  tm->next = tm->prev = NULL;
}

float c00[3][3]={{(1.0/8)*(1.0/8),(1.0/8)*(3.0/4),(1.0/8)*(1.0/8)},
                 {(1.0/8)*(3.0/4),(3.0/4)*(3.0/4),(1.0/8)*(3.0/4)},
                 {(1.0/8)*(1.0/8),(1.0/8)*(3.0/4),(1.0/8)*(1.0/8)}
};

float c01[3][3]={{(1.0/8)*(1.0/2),(1.0/8)*(1.0/2), 0},
                 {(3.0/4)*(1.0/2),(3.0/4)*(1.0/2), 0},
                 {(1.0/8)*(1.0/2),(1.0/8)*(1.0/2), 0}
};

float c10[3][3]={{(1.0/8)*(1.0/2),(3.0/4)*(1.0/2),(1.0/8)*(1.0/2)},
                 {(1.0/8)*(1.0/2),(3.0/4)*(1.0/2),(1.0/8)*(1.0/2)},
                 { 0, 0, 0}
};

float c11[3][3]={{(1.0/2)*(1.0/2),(1.0/2)*(1.0/2), 0},
		 {(1.0/2)*(1.0/2),(1.0/2)*(1.0/2), 0},
                 { 0, 0, 0}
};
                 
                
/* Get a new DeBoor point and offset at a smaller scale.*/

void getRef(Mesh *mesh, int pu, int pv)
{
  int pi,pj,au,av;
  int i,j;
  Vector sp,tp;
  float *cf[3];

  /*Get starting indices at upper level.*/
          if(pu<0){
            pi=mesh->m0;
            au=1;
          }
          else{
            au= pu%2;
            pi=mesh->m0+1+pu/2-(1-au);
            if(pi-1 > mesh->upper->m)
              pi = mesh->upper->m;
          }
          if(pv<0){
            pj=mesh->n0;
            av=1;
          }
          else{
            av=pv%2;
            pj=mesh->n0+1+pv/2-(1-av);
            if(pj-1 > mesh->upper->n)
              pj = mesh->upper->n;
          }
	  /* Set appropriate coefficient array.*/
          if(au==0 && av==0)
            for(i=0;i<3;i++) 
              cf[i] = c00[i];
          if(au==0 && av==1)
            for(i=0;i<3;i++) 
              cf[i] = c01[i];
          if(au==1 && av==0)
            for(i=0;i<3;i++) 
              cf[i] = c10[i];
          if(au==1 && av==1) 
            for(i=0;i<3;i++)
              cf[i] = c11[i];

          ptZero(sp);
	  for(i=0;i<3-au;i++)
            for(j=0;j<3-av;j++){
              vAdd(EOA((mesh->upper->ref),pi+i,pj+j,mesh->upper->m+3),
                   EOA((mesh->upper->off),pi+i,pj+j,mesh->upper->m+3),tp);
              vScale(tp,cf[i][j]);
              vAdd(sp,tp,sp);
            }
          sp[3]=1;
          vCopy(sp,EOA((mesh->ref),pu+1,pv+1,mesh->m+3));
}
  
/*This merges two meshes and returns a new one.*/
Mesh *meshMerge(Mesh *M1,Mesh *M2)
{
  Mesh *pm,*tm;
  UIPoint *uip;
  int m0,n0,m1,n1;
  int i,j;

  if(!(M1->upper)) Error("In meshMerge, m1 has no upper level.\n");
  pm=M1->upper;

  /* printf(" in meshMerge "); */
  m0=MIN(M1->m0,M2->m0);
  n0=MIN(M1->n0,M2->n0);
  m1=MAX(M1->m0+M1->m/2, M2->m0+M2->m/2);
  n1=MAX(M1->n0+M1->n/2, M2->n0+M2->n/2);

  tm=allocMesh(2*(m1-m0),2*(n1-n0));
  tm->m0=m0;
  tm->n0=n0;
  tm->level=M1->level;
  tm->upper=pm;
  tm->u0 = MIN(M1->u0,M2->u0);
  tm->v0 = MIN(M1->v0,M2->v0);
  tm->du = M1->du;
  tm->dv = M2->dv;

  m0=2*(M1->m0-tm->m0);
  n0=2*(M1->n0-tm->n0);
  m1=2*(M2->m0-tm->m0);
  n1=2*(M2->n0-tm->n0);

  /* Get merged ref and off points. */
  for(i=-1;i<tm->m+2;i++)
    for(j=-1;j<tm->n+2;j++)
      if(i>=m0-1 && i<=m0+M1->m+1 && j>=n0-1 && j<=n0+M1->n+1)
        vCopy(EOA((M1->ref),i+1-m0,j+1-n0,M1->m+3), 
              EOA((tm->ref),i+1,j+1,tm->m+3));
      else if(i>=m1-1 && i<=m1+M2->m+1 && j>=n1-1 && j<=n1+M2->n+1)
        vCopy(EOA((M2->ref),i+1-m1,j+1-n1,M2->m+3),
              EOA((tm->ref),i+1,j+1,tm->m+3));
      else{
        getRef(tm,i,j);
      }

  for(i=-1;i<tm->m+2;i++)
    for(j=-1;j<tm->n+2;j++){
      vSet(EOA((tm->off),i+1,j+1,tm->m+3), 0, 0, 0, 0);
      if(i>=m0-1 && i<=m0+M1->m+1 && j>=n0-1 && j<=n0+M1->n+1)
        vAdd(EOA((M1->off),i+1-m0,j+1-n0,M1->m+3), 
              EOA((tm->off),i+1,j+1,tm->m+3),
              EOA((tm->off),i+1,j+1,tm->m+3));
      if(i>=m1-1 && i<=m1+M2->m+1 && j>=n1-1 && j<=n1+M2->n+1)
        vAdd(EOA((M2->off),i+1-m1,j+1-n1,M2->m+3),
              EOA((tm->off),i+1,j+1,tm->m+3),
              EOA((tm->off),i+1,j+1,tm->m+3));
    }

  /* Get merged uipts. */
  for(i=0;i<=tm->m;i++)
    for(j=0;j<=tm->n;j++){
      if(i>=m0 && i<=m0+M1->m && j>=n0 && j<=n0+M1->n)
        if(EOA((M1->uipts),i-m0,j-n0,M1->m+1)){
          EOA((tm->uipts),i,j,tm->m+1)=
          EOA((M1->uipts),i-m0,j-n0,M1->m+1); 
          uip=EOA((tm->uipts),i,j,tm->m+1);
          EOA((M1->uipts),i-m0,j-n0,M1->m+1)=NULL;
          if(uip != (UIPoint *)-1){
            uip->mesh = tm;
            uip->m = i;
            uip->n = j;
          }
        }
      if(i>=m1 && i<=m1+M2->m && j>=n1 && j<=n1+M2->n)
        if(EOA((M2->uipts),i-m1,j-n1,M2->m+1)){
          EOA((tm->uipts),i,j,tm->m+1)=
          EOA((M2->uipts),i-m1,j-n1,M2->m+1); 
          uip=EOA((tm->uipts),i,j,tm->m+1);
          EOA((M2->uipts),i-m1,j-n1,M2->m+1)=NULL;
          if(uip != (UIPoint *)-1){
            uip->mesh = tm;
            uip->m = i;
            uip->n = j; 
          }
        }
    }

  /* Get merged mask.*/
  for(i=0;i<tm->m;i++)
    for(j=0;j<tm->n;j++){
      if(i>=m0 && i<m0+M1->m && j>=n0 && j<n0+M1->n){
        if(EOA((M1->mask),i-m0,j-n0,M1->m))
          EOA((tm->mask),i,j,tm->m)=
          EOA((M1->mask),i-m0,j-n0,M1->m); 
      }
      if(i>=m1 && i<m1+M2->m && j>=n1 && j<n1+M2->n){
        if(EOA((M2->mask),i-m1,j-n1,M2->m))
          EOA((tm->mask),i,j,tm->m)=
          EOA((M2->mask),i-m1,j-n1,M2->m); 
      }
    }

  pm=M1->lower;
  while(pm){
    deleteMesh(M1,pm);
    insertMesh(tm,pm);
    pm->m0 += m0;
    pm->n0 += n0;
    pm->upper = tm;
    pm = M1->lower;
  }

  pm=M2->lower;
  while(pm){
    deleteMesh(M2,pm);
    insertMesh(tm,pm);
    pm->m0 += m1;
    pm->n0 += n1;
    pm->upper = tm;
    pm = M2->lower;
  }

  return tm;
}


/* This instructs the mesh to add one more level of refinement around
   the UIPoint that is contained in "mesh" at index (m,n) (remember,
   this is the mesh at the "most refined" level for that UIPoint).
   If this causes any new User Interface Points to be created, the
   function also creates and returns a new uipList object containing 
   pointers to the additional UIPoints.  If there are no new UIPoints,
   NULL is returned. */
uipList meshRefine(Mesh *mesh, int m, int n)
{
  Mesh *tm,*mm,*om,*nm;
  UIPoint *uip;
  int i,j,flag;
  int p0,q0,p1,q1;
  uipList ul;

  if(m<0 || m> mesh->m)
    Error("In meshRefine, invalid m \n");
  if(n<0 || n> mesh->n)
    Error("In meshRefine, invalid n \n");
  tm=allocMesh(4,4);
  tm->upper=mesh;
  tm->level= mesh->level + 1;
  tm->m0 = m-1;
  tm->n0 = n-1;
  tm->du = mesh->du/2;
  tm->dv = mesh->dv/2;
  tm->u0 = mesh->u0 + tm->m0 * mesh->du;
  tm->v0 = mesh->v0 + tm->n0 * mesh->dv;

      p0=-1;
      q0=-1;
      p1=5;
      q1=5;

      /*get reference points and offsets here*/
      for(i=p0;i<=p1;i++)
        for(j=q0;j<=q1;j++){
          getRef(tm,i,j);
          vSet(EOA((tm->off),i+1,j+1,tm->m+3), 0, 0, 0, 0);
        }


  uip = EOA((tm->uipts),2,2,tm->m+1)= EOA((mesh->uipts),m,n,mesh->m+1);
  EOA((mesh->uipts),m,n,mesh->m+1)=(UIPoint *)-1;
  uip -> mesh = tm;
  uip -> m = 2;
  uip -> n = 2;
  
  flag=0;
    p0 = MAX(tm->m0,0);
    q0 = MAX(tm->n0,0);
    p1 = MIN(tm->m0+tm->m/2-1, mesh->m-1);
    q1 = MIN(tm->n0+tm->n/2-1, mesh->n-1);
    for(i=p0;i<=p1;i++)
      for(j=q0;j<=q1;j++)
        if(!flag && (om=EOA((mesh->mask),i,j,mesh->m))){
          flag=1;
        }
  while(flag){
    flag=0;

      mm=meshMerge(om,tm);

      p0 = MAX(mm->m0,0);
      q0 = MAX(mm->n0,0);
      p1 = MIN(mm->m0+mm->m/2-1, mesh->m-1);
      q1 = MIN(mm->n0+mm->n/2-1, mesh->n-1);
      for(i=p0;i<=p1;i++)
        for(j=q0;j<=q1;j++)
          if(EOA((mesh->mask),i,j,mesh->m)==om)
            EOA((mesh->mask),i,j,mesh->m) = NULL;
          else if(!flag)
            if((nm=EOA((mesh->mask),i,j,mesh->m))){
              flag=1;
            }

      deleteMesh(mesh,om);
      freeMesh(om);
      freeMesh(tm);
      tm=mm;
      om=nm;
  }
        
  getPolyRep(tm);
  
  ul=uiplNewList();
  for(i=2;i<=tm->m-2;i++)
    for(j=2;j<=tm->n-2;j++)
      if(!(EOA((tm->uipts),i,j,tm->m+1)))
        if(i%2 || j%2){
            EOA((tm->uipts),i,j,tm->m+1)= getUIPoint(tm,i,j,0.0,0.0);
            uiplAppend(ul,EOA((tm->uipts),i,j,tm->m+1));
        }
        else{
          EOA((tm->uipts),i,j,tm->m+1)=
          EOA((mesh->uipts),tm->m0+i/2,tm->n0+j/2,mesh->m+1);
          uip=EOA((tm->uipts),i,j,tm->m+1);
          /* Use (UIPoint)-1 to indicate this uiPoint has been moved to a lower level.*/
          EOA((mesh->uipts),tm->m0+i/2,tm->n0+j/2,mesh->m+1) = (UIPoint *)-1;
          if(uip==NULL)
            Warning("In meshRefine, uip==NULL\n");
          if(uip==(UIPoint *)-1)
            Warning("In meshRefine, uip== -1\n");
          uip -> mesh = tm;
          uip -> m = i;
          uip -> n = j;
        }
        

  tm -> upper = mesh;
  insertMesh(mesh,tm);

  p0 = MAX(tm->m0,0);
  q0 = MAX(tm->n0,0);
  p1 = MIN(tm->m0+tm->m/2, mesh->m);
  q1 = MIN(tm->n0+tm->n/2, mesh->n);
  for(i=p0; i<p1; i++)
    for(j=q0; j<q1; j++)
        EOA((mesh->mask),i,j,mesh->m) = tm;
  
  return ul;
}



/* This instructs the mesh to render itself.  It relies on the function
   patchRender (whos definition appears in "window.h") to render
   each patch. */
void meshRender(Mesh *mesh, SolidType st, int subdiv, BOOL MultiC,
		BOOL pov, FILE *f)
{
  Mesh *tm;
  Matrix bezpts;
  float pts[4][4][3];
  float um,vm;
  int i,j,k,l,c;

  if(!mesh) return;
  for(i=0;i<mesh->m;i++)
    for(j=0;j<mesh->n;j++){
      um=mesh->u0+(i+0.5)*mesh->du;
      vm=mesh->v0+(j+0.5)*mesh->dv;
      if(um>0 && um<1 && vm>0 && vm<1) 
      if(! EOA((mesh->mask),i,j,mesh->m)){
        for(c=0;c<3;c++){
          getBezCompFromPoly(EOA((mesh->poly[c]),i+1,j+1,mesh->m+2),bezpts);
          for(k=0;k<4;k++)
            for(l=0;l<4;l++)
              pts[k][l][c]= EOA(bezpts,k,l,4);
        }
	if (pov)
	  patchPOVOutput(pts, mesh->level, subdiv,f);
	else
	  patchRender(pts, mesh->level, MultiC, st, subdiv);
      }
    }
  tm=mesh->lower;
  while(tm){
    meshRender(tm,st,subdiv,MultiC,pov,f);
    tm = tm->next;
  }

}


