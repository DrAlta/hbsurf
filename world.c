/*
*  world.c - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#include "world.h"
#include "mesh.h"
#include "store.h"

Point lev_colors[4] = {{1.0, 1.0, 0.0, 1.0},
		       {0.0, 0.0, 1.0, 1.0},
		       {0.0, 1.0, 0.0, 1.0},
		       {1.0, 0.0, 0.0, 1.0}};
Point pt_color = {0, 0, 0, 1.0};
Point selpt_color = {1, 1, 1, 1.0};
Point axes_color = {1.0, 0.0, 0.0, 1.0};

void worldInit(World *w, char *readname)
{
  uipList uipl;
  int i;

  mSetIdentity(w->TopProj);
  mSetIdentity(w->FrontProj);
  mSetIdentity(w->SideProj);
  mSetIdentity(w->PerProj);

  orInit(&w->Orients[wqTop],wqTop);
  orInit(&w->Orients[wqFront],wqFront);
  orInit(&w->Orients[wqSide],wqSide);
  orInit(&w->Orients[wqPer],wqPer);

  /* Polygon Drawing Attributes */
  w->Solid = stSolid;

  /* Objects in our World */
  if (readname) 
    uipl = worldReadMesh(w,readname);
  else
    uipl = meshInit(&w->surf,5,5);
  w->numuipts = uiplNumElts(uipl);
  w->selected = -1;
  w->uipts = malloc(sizeof(UIPoint *) * w->numuipts);
  for (i=0; i<w->numuipts; i++) {
    w->uipts[i] = uiplDelFirst(uipl);
  }
  uiplFreeList(uipl);

  /* UI Suff */
  w->level = 0;
  w->DrawAxes = FALSE;
  w->MultiColor = TRUE;
  w->ConPoints = TRUE;
  w->CurrButton = INVALID_BUTTON;
  w->BCMode = bcSelect;
  w->Projection = pjtPerspective;
  w->StartX = 0;
  w->StartY = 0;
  w->LastX = 0;
  w->LastY = 0;
  w->Subdiv = 2;

  /* Composite Axes */
  w->AxesList = glGenLists(1);
  worldMakeAxesList(w->AxesList);

  /* Initialize GL */
  worldGLInit();

  /* Load Scene */
  worldLoad();
  
}

void worldReshapeViewPort(World *w, int sizex, int sizey, WinQuad domwq)
{
  int iWidth, iHeight;

  iWidth = sizex;
  iHeight = sizey;

  worldSetViewports(w,sizex,sizey,domwq);

  /* glViewport(0, 0, iWidth, iHeight); */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  /* First get parallel projections */
  if (iWidth < iHeight) 
    glOrtho(-1.2, 1.2, -1.2/w->Aspect, 1.2/w->Aspect, DIST_TO_CLIP, 100);
  else 
    glOrtho(-1.2*w->Aspect, 1.2*w->Aspect, -1.2, 1.2, DIST_TO_CLIP, 100);
  glGetFloatv(GL_PROJECTION_MATRIX,w->TopProj);
  glGetFloatv(GL_PROJECTION_MATRIX,w->FrontProj);
  glGetFloatv(GL_PROJECTION_MATRIX,w->SideProj);

  /* Now get the perspective projection */
  glLoadIdentity();
  gluPerspective(VIEW_ANGLE, w->Aspect, DIST_TO_CLIP, 100);
  glGetFloatv(GL_PROJECTION_MATRIX,w->PerProj);

  glMatrixMode(GL_MODELVIEW);

  glFlush();
}

void worldGLInit()
{
  /* Initialize GL */
  /* GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 }; */
  GLfloat light_position[] = { 0.3, 0.5, 0.8, 0.0 };
  GLfloat lm_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
  
  /* glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient); */
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  glEnable(GL_LIGHT0);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  /* glEnable(GL_NORMALIZE); */
  glShadeModel(GL_SMOOTH);
  glPointSize(6.0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  /* glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, head_diffuse); */

  glFlush();
}

void worldLoad()
{

}


void worldUpdate()
{
  Warning("worldUpdate not implemented yet");

}

void worldRender(World *w, WinQuad wq)
{
  SolidType solid;

  worldSetViewProj(w,wq,0,0,GL_RENDER);

  /* 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -3.0);
  glMultMatrixf(w->Orientation);
  */

  if ( w->DrawAxes ) {
    glCallList(w->AxesList);
  }

  /* Set up the lighting */
  switch (wq) {
  case wqTop:
  case wqFront:
  case wqSide:
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    solid = stWire;
    break;
  case wqPer:
    if (w->Solid == stSolid) {
      glEnable(GL_LIGHTING);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      solid = stSolid;
    } else {
      glDisable(GL_LIGHTING);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      solid = stWire;
    }
    break;
  default:
    Error("Bad WinQuad in worldRender");
  }

  /* glColor3f(1.0, 1.0, 0.0); */
  meshRender(w->surf,solid,w->Subdiv,w->MultiColor,FALSE,NULL);
  if (w->ConPoints)
    uiptsRender(w);

  /*
  glBegin(GL_POLYGON);
  glNormal3f(0.0, 0.0, 1.0);
  glVertex3f(-0.9, -0.9, 0.9);
  glVertex3f(0.9, -0.9, 0.0);
  glVertex3f(0.0, 0.9, 0.0);
  glEnd();
  */
}

void worldPerHandleButton(World *w, int iButton, int iState, int iX, int iY,
			  OrientMode om, WinQuad wq)
{
  int sizex, sizey;
  
  sizex = w->Orients[wq].vpwid;
  sizey = w->Orients[wq].vphei;

  if (iState == GLUT_DOWN && w->CurrButton == INVALID_BUTTON) {  
    w->LastX = w->StartX = iX;
    w->LastY = w->StartY = iY;
    w->CurrButton = iButton;
    
    orCaptureDown(&w->Orients[wq]);

    switch (iButton) {
    case GLUT_LEFT_BUTTON:
      /* printf("L down [%3d %3d]\n", iX, iY); */
      break;
    case GLUT_MIDDLE_BUTTON:
      /* printf("M down [%3d %3d]\n", iX, iY); */
      /* mDownScale = mCurrentScale; */
      w->selected = uiptsSelect(w,iX,iY,sizex,sizey,wq);
      break;
      
    case GLUT_RIGHT_BUTTON:
      /* printf("R down [%3d %3d]\n", iX, iY); */
      break;
      
    default:
      Error("Bad GL MOUSE BUTTON");
    }
    
  } else if (iState == GLUT_UP && iButton == w->CurrButton) {
    w->CurrButton = INVALID_BUTTON;
    
    switch (iButton) {
    case GLUT_LEFT_BUTTON:
      /* printf("L up   [%3d %3d]\n", iX, iY); */
      break;
      
    case GLUT_MIDDLE_BUTTON:
      /* printf("M up   [%3d %3d]\n", iX, iY); */
      break;
      
    case GLUT_RIGHT_BUTTON:
      /* printf("R up   [%3d %3d]\n", iX, iY); */
      break;
      
    default:
      Error("Bad GL MOUSE BUTTON");
    }
    glutPostRedisplay();
  }
}

void worldOrthHandleButton(World *w, int iButton, int iState, int iX, int iY,
			   OrientMode om, WinQuad wq)
{
  int sizex, sizey;
  
  sizex = w->Orients[wq].vpwid;
  sizey = w->Orients[wq].vphei;

  if (iState == GLUT_DOWN && w->CurrButton == INVALID_BUTTON) {  
    w->LastX = w->StartX = iX;
    w->LastY = w->StartY = iY;
    w->CurrButton = iButton;

    orCaptureDown(&w->Orients[wq]);

    switch (wq) {
    case wqTop:
      break;
    case wqFront:
      break;
    case wqSide:
      break;
    default:
      Error("Bad WinQuad in worldOrthHandleButton");
    }

    switch (iButton) {
    case GLUT_LEFT_BUTTON:
      /* printf("L down [%3d %3d]\n", iX, iY); */
      break;
    case GLUT_MIDDLE_BUTTON:
      w->selected = uiptsSelect(w,iX,iY,sizex,sizey,wq);
      break;
      
    case GLUT_RIGHT_BUTTON:
      /* printf("R down [%3d %3d]\n", iX, iY); */
      break;
      
    default:
      Error("Bad GL MOUSE BUTTON");
    }
    
  } else if (iState == GLUT_UP && iButton == w->CurrButton) {
    w->CurrButton = INVALID_BUTTON;
    
    switch (iButton) {
    case GLUT_LEFT_BUTTON:
      /* printf("L up   [%3d %3d]\n", iX, iY); */
      break;
      
    case GLUT_MIDDLE_BUTTON:
      /* printf("M up   [%3d %3d]\n", iX, iY); */
      break;
      
    case GLUT_RIGHT_BUTTON:
      /* printf("R up   [%3d %3d]\n", iX, iY); */
      break;
      
    default:
      Error("Bad GL MOUSE BUTTON");
    }
    glutPostRedisplay();
  }
}


void worldOrthHandleMotion(World *w, int iX, int iY, OrientMode om, WinQuad wq)
{
  int iDX, iDY;
  int iDXinc, iDYinc;
  float fWinSize;
  float mag;
  Vector v1;
  UIPoint *selpt;
  int sizex, sizey;
  
  sizex = w->Orients[wq].vpwid;
  sizey = w->Orients[wq].vphei;

  fWinSize = (float)MIN(sizex, sizey);

  iDX = iX - w->StartX;
  iDY = iY - w->StartY;

  iDXinc = iX - w->LastX;
  iDYinc = iY - w->LastY;

  switch ( w->CurrButton ) {
  case GLUT_LEFT_BUTTON:
    if (w->selected != -1) {
      vZero(v1);
      worldGetMoveVector(iDXinc, iDYinc, sizex, sizey, wq, w->Aspect,
			 &w->Orients[wq], v1, v1);
      selpt = w->uipts[w->selected];
      meshMove(selpt->mesh, selpt->m, selpt->n, w->level, v1);
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    switch (om) {
    case omScale:
      mag = pow(2,5*iDY/(float)sizey);
      w->Orients[wq].scale = mag * w->Orients[wq].downscale;
      break;
    case omTrans:
      vZero(v1);
      worldGetMoveVector(iDX, iDY, sizex, sizey, wq, w->Aspect, 
			 &w->Orients[wq],v1, v1);
      vAdd(v1,w->Orients[wq].downtrans,w->Orients[wq].trans);
    default:
    }
    break;
  default:
    Error("Bad GL MOUSE BUTTON in worldOrthHandleMotion");
  }
  
  glutPostRedisplay();
  w->LastX = iX;
  w->LastY = iY;
}



void worldPerHandleMotion(World *w, int iX, int iY, OrientMode om, WinQuad wq)
{
  int iDX, iDY;
  int iDXinc, iDYinc;
  /* Quater q; */
  Vector v1, v2, v3, vNorm;
  float mag;
  float fWinSize;
  double theta, sint, cost;
  Quater q;
  UIPoint *selpt;
  int sizex, sizey;
  
  sizex = w->Orients[wq].vpwid;
  sizey = w->Orients[wq].vphei;

  fWinSize = (float)MIN(sizex, sizey);

  iDX = iX - w->StartX;
  iDY = iY - w->StartY;
  
  iDXinc = iX - w->LastX;
  iDYinc = iY - w->LastY;

  switch (w->CurrButton) {
  case GLUT_LEFT_BUTTON:
    if (w->selected != -1) {
      selpt = w->uipts[w->selected];
      worldGetMoveVector(iDXinc, iDYinc, sizex, sizey, wq, w->Aspect,
			 &w->Orients[wq], selpt->pt, v1);
      meshMove(selpt->mesh, selpt->m, selpt->n, w->level, v1);
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    switch (om) {
    case omBall:
      /* General Rotations over axes in XY-plane */
      /* printf("L move [%3d %3d][%3d %3d]\n", iX, iY, iDXinc, iDYinc); */
      
      vZero(v1);
      v1[0] = iDX;
      v1[1] = iDY;
      mag = vMagnitude(v1);
      
      /* from arclength = theta * radius */
      theta = ( (mag*PI) / ( fWinSize * 0.8f ) );
      
      vZero(vNorm);
      vNorm[0] = -v1[1];
      vNorm[1] = v1[0];
      
      /*If we can't normalize, then vector is 0, and we should set */
      /*  w->Orientation to w->StartOrient. */
      if (!vNormalize(vNorm)) {
	orUnCaptureDown(&w->Orients[wq]);
      } else {
	qtSet(q,theta,vNorm);
	qtMult(q,w->Orients[wq].downrot,w->Orients[wq].rot);
      }
      break;
      
    case omScale:
      /* Scaling */
      /* printf("M move [%3d %3d][%3d %3d]\n", iX, iY, iDX, iDY); */
      mag = pow(2,5*iDY/(float)sizey);
      w->Orients[wq].scale = mag * w->Orients[wq].downscale;
      break;
      
    case omSpin:
      /* Rotations around Z-axis */
      /* printf("R move [%3d %3d][%3d %3d]\n", iX, iY, iDX, iDY); */
      
      vZero(v1);
      v1[0] = w->StartX-(sizex/2);
      v1[1] = w->StartY-(sizey/2);
      
      vZero(v2);
      v2[0] = iX-(sizex/2);
      v2[1] = iY-(sizey/2);
      
      if (vNormalize(v1) && vNormalize(v2)) {
	cost = vDot(v1,v2);
	vCross(v1,v2,v3);
	sint = v3[2];
	theta = atan2(sint,cost);
	vSet(v1,0,0,1,0);
	qtSet(q,theta,v1);
	qtMult(q,w->Orients[wq].downrot,w->Orients[wq].rot);
      } else {
	/*If we can't normalize, then vector is 0, and we should set */
	/*  w->Orientation to w->StartOrient. */
	orUnCaptureDown(&w->Orients[wq]);
      }
      break;

    case omTrans:
      vCopy(w->Orients[wq].trans,v2);
      v2[3] = 1.0;
      worldGetMoveVector(iDX, iDY, sizex, sizey, wq, w->Aspect, 
			 &w->Orients[wq],w->Orients[wq].trans, v1);
      vAdd(v1,w->Orients[wq].downtrans,w->Orients[wq].trans);
      break;
      
    default:
      Error("Bad Orientation Mode in worldPerHandleMotion");
    }
    break;
  default:
    Error("Bad GL MOUSE BUTTON in worldPerHandleMotion");
  }
  
  glutPostRedisplay();
  w->LastX = iX;
  w->LastY = iY;
}

void worldToggleDrawAxes(World *w)
{
  w->DrawAxes = !w->DrawAxes;
}

void worldToggleColorScheme(World *w)
{
  w->MultiColor = !w->MultiColor;
}

void worldTogglePoints(World *w)
{
  w->ConPoints = !w->ConPoints;
  w->selected = -1;
}


void worldToggleProjection(World *w)
{
  switch ( w->Projection ) {
  case pjtParallel:
    w->Projection = pjtPerspective;
    break;
  case pjtPerspective:
    w->Projection = pjtParallel;
    break;
  default:
    Error("Bad Projection type (worldToggleDrawAxes).");
  }
} 

void worldToggleBControl(World *w)
{
  switch ( w->BCMode ) {
  case bcSelect:
    w->BCMode = bcMove;
    break;
  case bcMove:
    w->BCMode = bcSelect;
    break;
  default:
    Error("Bad Button Control type (worldToggleBControl).");
  }
} 


void worldToggleSolid(World *w)
{
  switch ( w->Solid ) {
  case stSolid:
    w->Solid = stWire;
    break;
  case stWire:
    w->Solid = stSolid;
    break;
  default:
    Error("Bad Solid type (worldToggleSolid).");
  }
}


void worldIncLevel(World *w)
{
  UIPoint *pt;
  uipList uipl;
  int i, first;

  if (w->selected != -1) {
    pt = w->uipts[w->selected];
    if (pt->mesh->level == w->level) {

      /* Refine the mesh here, and add new uipts to array. */
      uipl =  meshRefine(pt->mesh,pt->m,pt->n);
      first = w->numuipts;
      w->numuipts = w->numuipts + uiplNumElts(uipl);
      w->uipts = realloc(w->uipts, sizeof(UIPoint *) * w->numuipts);
      for (i=first; i<w->numuipts; i++) {
	w->uipts[i] = uiplDelFirst(uipl);
      }
      uiplFreeList(uipl);
    }
  }
  w->level++;
  /* printf("Current level is now %d.\n",w->level); */
}

void worldDecLevel(World *w)
{
  if (w->level != 0)
    w->level--;
  /* printf("Current level is now %d.\n",w->level); */
}

void worldIncSubdiv(World *w)
{
  w->Subdiv++;
}


void worldDecSubdiv(World *w)
{
  w->Subdiv--;
  if (w->Subdiv < 1)
    w->Subdiv = 1;
}

void worldSetViewProj(World *w, WinQuad wq, int iX, int iY, GLenum mode)
{
  Matrix orient, proj;
  GLuint viewport[4];

  switch (wq) {
  case wqTop:
    mCopy(w->TopProj, proj);
    break;
  case wqFront:
    mCopy(w->FrontProj, proj);
    break;
  case wqSide:
    mCopy(w->SideProj, proj);
    break;
  case wqPer:
    mCopy(w->PerProj, proj);
    break;
  default:
    Error("Bad Window Quadrant (worldSetViewProj).");
  }

  if (mode == GL_SELECT) {
    glViewport(0,0,w->Orients[wq].vpwid,w->Orients[wq].vphei);
    glGetIntegerv(GL_VIEWPORT, viewport);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPickMatrix((GLdouble)iX, (GLdouble)iY, 9.0, 9.0, viewport);
    glMultMatrixf(proj);
  } else {
    glViewport(w->Orients[wq].vpllx,w->Orients[wq].vplly,
	       w->Orients[wq].vpwid,w->Orients[wq].vphei);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(proj);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -DIST_FROM_EYE);
  glScalef(w->Orients[wq].scale,w->Orients[wq].scale,w->Orients[wq].scale);
  qtRotMatrix(w->Orients[wq].rot,orient);
  glMultMatrixf(orient);
  glTranslatef(w->Orients[wq].trans[0],w->Orients[wq].trans[1],
	       w->Orients[wq].trans[2]);
}



void uiptsRender(World *w)
{
  glDisable(GL_LIGHTING);

  uiptsRenderCore(w, GL_RENDER);

  glEnable(GL_LIGHTING);
}


void uiptsRenderCore(World *w, GLenum mode)
{
  GLuint i;

  glPointSize(6.0);
  glColor4fv(pt_color);
  for (i=0; i < w->numuipts; i++) {
    if (mode == GL_SELECT)
      glLoadName(i);
    if (i == w->selected && mode == GL_RENDER) {
      glColor4fv(selpt_color);
      glBegin(GL_POINTS);
      glVertex4fv(w->uipts[i]->pt);
      glEnd();
      glColor4fv(pt_color);
    } else {
      glBegin(GL_POINTS);
      glVertex4fv(w->uipts[i]->pt);
      glEnd();
    }
  }
  if (mode == GL_RENDER) {
    glPointSize(4.0);
    glColor4fv(selpt_color);
    for (i=0; i < w->numuipts; i++) {
      if (i == w->selected) {
	glColor4fv(pt_color);
	glBegin(GL_POINTS);
	glVertex4fv(w->uipts[i]->pt);
	glEnd();
	glColor4fv(selpt_color);
      } else {
	glBegin(GL_POINTS);
	glVertex4fv(w->uipts[i]->pt);
	glEnd();
      }
    }
  }
}

int uiptsSelect(World *w, int iX, int iY, int width, int height, WinQuad wq)
{
  GLuint *selectBuf, *ptr, lastz, sel=-1;
  GLint hits;
  int bufsize, i;
  UIPoint *pt;

  if (!w->ConPoints)
    return -1;

  bufsize = 4 * w->numuipts;
  selectBuf = malloc(sizeof(GLuint) * bufsize);
  if (selectBuf == NULL)
    Error("Could not allocate select buffer memory (uiptsSelect).");
  
  glSelectBuffer(bufsize, selectBuf);
  glRenderMode(GL_SELECT);

  glInitNames();
  glPushName(-1);

  worldSetViewProj(w,wq,iX,iY,GL_SELECT);
  
  uiptsRenderCore(w,GL_SELECT);
  hits = glRenderMode(GL_RENDER);


  /* Now Process the hits to find out what the selected uipt is. */
  ptr = selectBuf;
  for (i=0; i<hits; i++) {
    /* If the first element is greater than one, we made a mistake. */
    if (*ptr != 1)
      Error("Too many names on stack in uiptsSelect.");
    ptr++;
    if (i==0) {
      lastz = *ptr;
      ptr += 2;
      sel = *ptr;
    } else {
      if (*ptr < lastz) {
	lastz = *ptr;
	sel = *(ptr+2);
      }
      ptr += 2;
    }
    ptr++;
  }

  free(selectBuf);

  if (sel != -1) {
    pt = w->uipts[sel];
    if (pt->mesh->level < w->level) {
      w->level = pt->mesh->level;
      /* printf("Current level is now %d.\n",w->level); */
    }
  }
    
  return sel;
}


void worldGetMoveVector(int iDX, int iDY, int width, int height, WinQuad wq, 
			float aspect, Orient *o, Point pt, Vector v)
{
  float dx, dy, dist;
  Vector v1, vdist, vnorm;
  Matrix m;
  Quater q;
  Point transp;

  qtConjugate(o->rot,q);
  qtRotMatrix(q,m);

  switch (wq) {
  case wqTop:
  case wqFront:
  case wqSide:
    if (aspect <= 1) {
      dx = (iDX/(float)width) * 2.4;
      dy = (iDY/(float)height) * (2.4/aspect);
    } else {
      dx = (iDX/(float)width) * (2.4*aspect);
      dy = (iDY/(float)height) * 2.4;
    }
    break;
  case wqPer:
    /* Must get distance of pt from translate pt along normal vector */
    vCopy(o->trans,transp);
    vScale(transp,-1);
    transp[3] = 1;
    vSubtr(transp,pt,vdist);
    vSet(v1,0,0,1,0);
    mvMult(m,v1,vnorm);
    dist = vDot(vnorm,vdist);
    /* Since the view angle is in y,z plane, we divide x AND y by height */
    dx = (iDX/(float)(height/2))*(dist + DIST_FROM_EYE)*
      tan(VIEW_ANGLE*PI/(double)360);
    dy = (iDY/(float)(height/2))*(dist + DIST_FROM_EYE)*
      tan(VIEW_ANGLE*PI/(double)360);
    break;
  default:
    Error("Bad WinQuad in worldGetMoveVector");
  }

  vSet(v1,dx,dy,0,0);
  vScale(v1,1/o->scale);
  mvMult(m,v1,v);
}

void orInit(Orient *o, WinQuad wq)
{
  Vector v1;
  float theta;

  o->scale = 1.0;
  o->downscale = 1.0;
  vZero(o->trans);
  vZero(o->downtrans);
  switch (wq) {
  case wqTop:
    vSet(v1,1,0,0,0);
    theta = PI/2.0;
    break;
  case wqFront:
    vSet(v1,0,1,0,0);
    theta = 0;
    break;
  case wqSide:
    vSet(v1,0,1,0,0);
    theta = PI/2;
    break;
  case wqPer:
    vSet(v1,0,1,0,0);
    theta = 0;
    break;
  default:
    Error("Bad WinQuad in otInit.");
  }
  qtSet(o->rot,theta,v1);
  qtSet(o->downrot,theta,v1);
}

void orCaptureDown(Orient *o)
{
  o->downscale = o->scale;
  vCopy(o->rot,o->downrot);
  vCopy(o->trans,o->downtrans);
}

void orUnCaptureDown(Orient *o)
{
  o->scale = o->downscale;
  vCopy(o->downrot,o->rot);
  vCopy(o->downtrans,o->trans);
}


void worldSetViewports(World *w, int sizex, int sizey, WinQuad domwq)
{
  w->Orients[wqFront].vpllx = 0;
  w->Orients[wqFront].vplly = sizey*PORT_TOP_LOW;
  w->Orients[wqFront].vpwid = sizex/2;
  w->Orients[wqFront].vphei = sizey*(PORT_TOP_HI - PORT_TOP_LOW);

  w->Orients[wqSide].vpllx = sizex/2;
  w->Orients[wqSide].vplly = sizey*PORT_TOP_LOW;
  w->Orients[wqSide].vpwid = sizex/2;
  w->Orients[wqSide].vphei = sizey*(PORT_TOP_HI - PORT_TOP_LOW);

  w->Orients[wqTop].vpllx = 0;
  w->Orients[wqTop].vplly = sizey*PORT_BOT_LOW;
  w->Orients[wqTop].vpwid = sizex/2;
  w->Orients[wqTop].vphei = sizey*(PORT_BOT_HI - PORT_BOT_LOW);
  
  w->Orients[wqPer].vpllx = sizex/2;
  w->Orients[wqPer].vplly = sizey*PORT_BOT_LOW;
  w->Orients[wqPer].vpwid = sizex/2;
  w->Orients[wqPer].vphei = sizey*(PORT_BOT_HI - PORT_BOT_LOW);

  w->Aspect = ((GLdouble)sizex)/(sizey*
			       ((PORT_TOP_HI - PORT_TOP_LOW)+
				(PORT_TOP_HI - PORT_TOP_LOW)));
  
  if (domwq != wqNone) {
    w->Orients[domwq].vpllx = 0;
    w->Orients[domwq].vplly = 0;
    w->Orients[domwq].vpwid = sizex;
    w->Orients[domwq].vphei = sizey;
    w->Aspect = ((GLdouble)sizex)/((GLdouble)sizey);
  }
}


void worldSaveMesh(World *w, char *fname)
{
  FILE *f;
  
  f = fopen(fname,"w");
  if (f == NULL) {
    Warning("Could not open file for output!");
  } else {
    saveMesh(f,w->surf);
    fclose(f);
  }
}

void worldSavePOVMesh(World *w, char *fname)
{
  FILE *f;
  Point transp, eyep;
  Quater q;
  Matrix m;
  Vector v1,vnorm,vsky,vlight;
  
  f = fopen(fname,"w");
  if (f == NULL) {
    Warning("Could not open POV file for output!");
  } else {
    fprintf(f,"//This is an \"hbsurf\" generated POV file.\n\n");
    fprintf(f,"#declare PATCH_TYPE = 1\n");
    fprintf(f,"#declare FLATNESS_VALUE = .01\n");
    fprintf(f,"#declare NUM_U_STEPS = 2\n");
    fprintf(f,"#declare NUM_V_STEPS = 2\n");
    fprintf(f,"\n");

    /* Now we get the eye position from Perspective view */
    qtConjugate(w->Orients[wqPer].rot,q);
    qtRotMatrix(q,m);
    vCopy(w->Orients[wqPer].trans,transp);
    vScale(transp,-1);
    transp[3] = 1;
    vSet(v1,0,1,0,0);
    mvMult(m,v1,vsky);
    vSet(v1,.3,.5,.8,0);
    mvMult(m,v1,vlight);
    vScale(vlight,10);
    vSet(v1,0,0,1,0);
    mvMult(m,v1,vnorm);
    vScale(vnorm,DIST_FROM_EYE);
    vAdd(transp,vnorm,eyep);

    fprintf(f,"camera {\n");
    fprintf(f,"   location <%f, %f, %f>\n",eyep[0],eyep[1],eyep[2]);
    fprintf(f,"   sky      <%f, %f, %f>\n",vsky[0],vsky[1],vsky[2]);
    fprintf(f,"   up       <0, 1, 0>\n");
    fprintf(f,"   right    <-1, 0, 0>\n");
    fprintf(f,"   look_at  <%f, %f, %f>\n",transp[0],transp[1],transp[2]);
    fprintf(f,"}\n\n");

    fprintf(f,"light_source {\n");
    fprintf(f,"   <%f, %f, %f>\n",vlight[0], vlight[1], vlight[2]);
    fprintf(f,"   color red 1 green 1 blue 1\n");
    fprintf(f,"}\n\n\n\n");

    fprintf(f,"union {\n");
    meshRender(w->surf,stSolid,0,FALSE,TRUE,f);
    fprintf(f,"   pigment { Brown }\n");
    fprintf(f,"   finish  { shiny} \n");
    fprintf(f,"   scale %f\n",w->Orients[wqPer].scale);
    fprintf(f,"}\n");
    fclose(f);
  }
}



uipList worldReadMesh(World *w,char *readname)
{
  FILE *f;
  uipList uipl;

  f = fopen(readname,"r");
  if (f == NULL) {
    Error("Could not open file for input!");
  } else {
    uipl = readMesh(f,&w->surf);
    fclose(f);
  }
  return uipl;
}

void worldMakeAxesList(GLuint list)
{
  glNewList(list, GL_COMPILE);
  glDisable(GL_LIGHTING);
  glColor4fv(axes_color);

  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  glRotatef(-90,0,0,1);
  glTranslatef(0,.9,0);
  glBegin(GL_LINES);
    glVertex3f(0, .1, 0); glVertex3f(0, -.9, 0);
    glVertex3f(0, .1, 0); glVertex3f(-.05, 0, 0);
    glVertex3f(0, .1, 0); glVertex3f(.05, 0, 0);
    glVertex3f(0, .1, 0); glVertex3f(0, 0, -.05);
    glVertex3f(0, .1, 0); glVertex3f(0, 0, .05);
  glEnd();
  glTranslatef(.15,0,-.15);
  glRotatef(-45,0,1,0);
  glBegin(GL_LINES);
    glVertex3f(-.1, 0, 0); glVertex3f(.1, .1, 0);
    glVertex3f(-.1, .1, 0); glVertex3f(.1, 0, 0);
  glEnd();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0,.9,0);
  glBegin(GL_LINES);
    glVertex3f(0, .1, 0); glVertex3f(0, -.9, 0);
    glVertex3f(0, .1, 0); glVertex3f(-.05, 0, 0);
    glVertex3f(0, .1, 0); glVertex3f(.05, 0, 0);
    glVertex3f(0, .1, 0); glVertex3f(0, 0, -.05);
    glVertex3f(0, .1, 0); glVertex3f(0, 0, .05);
  glEnd();
  glTranslatef(.15,0,-.15);
  glRotatef(-45,0,1,0);
  glBegin(GL_LINES);
    glVertex3f(0, -.1, 0); glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 0); glVertex3f(-.1, .1, 0);
    glVertex3f(0, 0, 0); glVertex3f(.1, .1, 0);
  glEnd();
  glPopMatrix();

  glPushMatrix();
  glRotatef(90,1,0,0);
  glTranslatef(0,.9,0);
  glBegin(GL_LINES);
    glVertex3f(0, .1, 0); glVertex3f(0, -.9, 0);
    glVertex3f(0, .1, 0); glVertex3f(-.05, 0, 0);
    glVertex3f(0, .1, 0); glVertex3f(.05, 0, 0);
    glVertex3f(0, .1, 0); glVertex3f(0, 0, -.05);
    glVertex3f(0, .1, 0); glVertex3f(0, 0, .05);
  glEnd();
  glTranslatef(.15,0,.15);
  glRotatef(-45,0,1,0);
  glBegin(GL_LINES);
    glVertex3f(.1, -.1, 0); glVertex3f(.1, .1, 0);
    glVertex3f(.1, -.1, 0); glVertex3f(-.1, .1, 0);
    glVertex3f(-.1, -.1, 0); glVertex3f(-.1, .1, 0);
  glEnd();
  glPopMatrix();

  glEndList();  
}


void patchPOVOutput(GLfloat pts[4][4][3], int level, int subdiv, FILE *f)
{
  int i,j;

  fprintf(f,"\n   bicubic_patch {\n");
  fprintf(f,  "      type PATCH_TYPE\n");
  fprintf(f,  "      flatness FLATNESS_VALUE\n");
  fprintf(f,  "      u_steps NUM_U_STEPS\n");
  fprintf(f,  "      v_steps NUM_V_STEPS\n");
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      fprintf(f,"      <%.10f, %.10f, %.10f>\n",
	      pts[i][j][0],pts[i][j][1],pts[i][j][2]);
    }
    fprintf(f,"   \n");
  }
  fprintf(f,"   }\n\n");
}

char orient_names[4][20] = {"\"Rotate\"", "\"Zoom\"", 
			    "\"Spin\"", "\"Translate\""}; 

void worldRenderDecor(World *w, int width, int height, OrientMode om,
		      char *message)
{
  float midx, bot_low, bot_hi, top_low, top_hi;
  float band1, band2, band3, band4;
  float strip1, strip2, strip3;
  float rstartx, rstarty, rwid, rhei;
  int i;
  char c[256];

  midx = .5;
  bot_low = (w->Orients[wqPer].vplly)/(float)height;
  bot_hi = (w->Orients[wqPer].vplly + w->Orients[wqPer].vphei)/(float)height;
  top_low = w->Orients[wqSide].vplly/(float)height;
  top_hi = (w->Orients[wqSide].vplly + w->Orients[wqSide].vphei)/(float)height;

  band1 = .02;
  band2 = .35;
  band3 = .5;
  band4 = .7;
  
  strip1 = .01;
  strip2 = .05;
  strip3 = .09;

  rstartx = band1;
  rstarty = strip3;
  rwid = .05;
  rhei = .05;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1.0, 0, 1.0, -0.5, 0.5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0,0,width,height);
  glEnable(GL_LIGHTING);

  /* Decorate the windows */
  glColor3f(1,1,1);
  glBegin(GL_LINES);
    glVertex2f(0,1); glVertex2f(1,1); 
    glVertex2f(0,top_hi); glVertex2f(1,top_hi); 
    glVertex2f(0,top_low); glVertex2f(1,top_low); 
    glVertex2f(0,bot_hi); glVertex2f(1,bot_hi); 
    glVertex2f(0,bot_low); glVertex2f(1,bot_low); 
    glVertex2f(midx,1); glVertex2f(midx,bot_low);
  glEnd();
  
  decorWrite(.005,top_hi+.01,width,height,"Front (1)");
  decorWrite(midx+.005,top_hi+.01,width,height,"Side (2)");
  decorWrite(.005,bot_hi+.01,width,height,"Top (3)");
  decorWrite(midx+.005,bot_hi+.01,width,height,"Perspective (4)");

  /* Add the color key */
  for (i=0; i<5; i++) {
    glColor4fv(lev_colors[i%4]);
    glBegin(GL_POLYGON);
    glVertex2f(rstartx+(i*rwid),rstarty);
    glVertex2f(rstartx+(i*rwid),rstarty+rhei);
    glVertex2f(rstartx+(i+1)*rwid,rstarty+rhei);
    glVertex2f(rstartx+(i+1)*rwid,rstarty);
    glEnd();
    if (i!=4) { c[0] = 0x30 + i; c[1] = '\0'; }
    else { c[0] = '-'; c[1] = '>'; c[2] = '\0'; }
    glColor3f(1,1,1);
    decorWrite(rstartx+(i*rwid)+.01,strip2,width,height,c);
  }
  decorWrite(rstartx+.01,strip1,width,height,"Level Key");

  /* Display Current Level and color */
  glColor4fv(lev_colors[w->level%4]);
  glBegin(GL_POLYGON);
  glVertex2f(band2,rstarty);
  glVertex2f(band2,rstarty+rhei);
  glVertex2f(band2+rwid,rstarty+rhei);
  glVertex2f(band2+rwid,rstarty);
  glEnd();
  glColor3f(1,1,1);
  sprintf(c,"%d",w->level);
  decorWrite(band2+rwid+.01,rstarty+.01,width,height,c);
  decorWrite(band2,strip2,width,height,"Current");
  decorWrite(band2,strip1,width,height,"Level (+/-)");

  /* Display Orientation Mode */
  decorWrite(band3,strip1,width,height,orient_names[om]);
  decorWrite(band3,strip3,width,height,"Orientation");
  decorWrite(band3,strip2,width,height,"Mode (BUT3)");

  decorWrite(band4,strip2,width,height,message);
  *message = '\0';
}

void decorWrite(float rx, float ry, int width, int height, const char *str)
{
  glRasterPos2f(rx,ry);

  if (width > 800 && height > 800) {
    while (*str != '\0') {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*str);
      str++;
    }
  } else {
    while (*str != '\0') {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*str);
      str++;
    }
  }
}
