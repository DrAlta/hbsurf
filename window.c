/*
*  window.c - for HBSURF hierarchical B-Spline Surface Modeler
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

BOOL winInit(int argc, char **argv)
{
  win.WindowID = 0;
  win.RedisplayWindowID = 0;

  win.AllowIdle = FALSE;

  /* Are the window coords oriented the same way our coords are? */
  win.SizeX = WIN_SIZE_X;
  win.SizeY = WIN_SIZE_Y;
  win.OrientedX = TRUE;
  win.OrientedY = FALSE;

  win.wqdown = wqNone;
  win.wqdom = wqNone;
  win.om = omBall;
  win.ReadName = NULL;

  /* Parse Command Line Arguments */
  if ( !winParseCommandLine(argc, argv) ) {
    Warning("Problem parsing command line arguments.");
    return(FALSE);
  }
  
  /* Initialize the GLUT window manager */
  winInitGLUT(argc, argv);

  /* Initialize the Object World */
  worldInit(&win.world, win.ReadName);
  worldSetViewports(&win.world,WIN_SIZE_X,WIN_SIZE_Y,win.wqdom);
  worldReshapeViewPort(&win.world, win.SizeX, win.SizeY, win.wqdom);

  return TRUE;
}

void winPrintUsage()
{
  printf("Usage: hbsurf [-read \"filename\"]\n");
  exit(-1);
}

BOOL winParseCommandLine(int argc, char **argv)
{
  int argnum=1;

  while (argnum < argc) {
    if (!strcmp(argv[argnum],"-read")) {
      argnum++;
      if (argnum != argc) {
	win.ReadName = argv[argnum];
	argnum++;
      } else {
	winPrintUsage();
      }
    } else {
      winPrintUsage();
    }
  }
  return(TRUE);
}

void winInitGLUT(int argc, char **argv)
{
  /* Initialize the GLUT Window */
  glutInit(&argc, argv);
  glHintPGI( GL_PREFER_DOUBLEBUFFER_HINT_PGI, GL_TRUE );

  glutInitWindowSize(win.SizeX, win.SizeY);
  glutInitWindowPosition(40, 150);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  win.WindowID = glutCreateWindow(
		"HBSURF - Hierarchical B-Spline Surface Modeler");
  glHintPGI( GL_ALWAYS_FAST_HINT_PGI, GL_TRUE );

  /* Set Up Callbacks */
  glutMouseFunc(winHandleButton);
  glutMotionFunc(winHandleMotion);
  glutKeyboardFunc(winHandleKeys);
  glutDisplayFunc(winDisplay);
  glutReshapeFunc(winReshape);

  win.RedisplayWindowID = glutGetWindow();

  /* Set Allow Idle which sets the Idle callback function */
  winSetAllowIdle(TRUE);

}

void winMainLoop()
{
  glutMainLoop();
}


/* Call Back Handlers */
void winHandleButton(int iButton, int iState, int ipxX, int ipxY)
{
  int iX, iY;
  WinQuad wq;

  iX = winDecodePixelX(ipxX);
  iY = winDecodePixelY(ipxY);
  wq = winWhichQuad(iX,iY,win.wqdown,win.wqdom,&iX,&iY);

  switch (wq) {
  case wqTop:
  case wqFront:
  case wqSide:
    worldOrthHandleButton(&win.world,iButton, iState, iX, iY, win.om, wq);
    break;
  case wqPer:
    worldPerHandleButton(&win.world,iButton, iState, iX, iY, win.om, wq);
    break;
  default:
  }

  if (iState == GLUT_DOWN && win.wqdown == wqNone)
    win.wqdown = wq;
  else if (iState == GLUT_UP && win.wqdown == wq)
    win.wqdown = wqNone;
}

void winHandleMotion(int ipxX, int ipxY)
{
  int iX, iY;
  WinQuad wq;

  iX = winDecodePixelX(ipxX);
  iY = winDecodePixelY(ipxY);
  wq = winWhichQuad(iX,iY,win.wqdown,win.wqdom,&iX,&iY);

  switch (wq) {
  case wqTop:
  case wqFront:
  case wqSide:
    worldOrthHandleMotion(&win.world, iX, iY, win.om, wq);
    break;
  case wqPer:
    worldPerHandleMotion(&win.world, iX, iY, win.om, wq);
    break;
  default:
  }
}

void winHandleKeys(unsigned char ucKey, int ipxX, int ipxY)
{
  int iX, iY;
  WinQuad wq;

  iX = winDecodePixelX(ipxX);
  iY = winDecodePixelY(ipxY);

  switch ( ucKey ) 
    {
    case '':
      /* Quit */
      Close();
      break;
      
    case  'a':
      /* Toggle Axes Drawing */
      worldToggleDrawAxes(&win.world);
      break;
      
    case  'o':
      /* Cycle Through Orientation modes */
      win.om = (win.om+1)%4;
      break;
      
    case  'r':                           
      /* Reset View */
      wq = winWhichQuad(iX,iY,wqNone,win.wqdom,&iX,&iY);
      if (wq != wqNone)
	orInit(&win.world.Orients[wq],wq);
      break;

    case  'f':                           
      /* Toggle Fill Model */
      worldToggleSolid(&win.world);
      break;

    case 's':
      /* Save hbs file */
      worldSaveMesh(&win.world,"save.hbs");
      sprintf(win.message,"Saved file \"save.hbs\"");
      break;
      
    case 'p':
      /* Save POV file */
      worldSavePOVMesh(&win.world,"save.pov");
      sprintf(win.message,"Saved POV file \"save.pov\"");
      break;
      
    case  'i':                           
      /* Increase Detail */
      worldIncSubdiv(&win.world);
      sprintf(win.message,"Increased Detail");
      break;

    case  'd':                           
      /* Decrease Detail */
      worldDecSubdiv(&win.world);
      sprintf(win.message,"Decreased Detail");
      break;

    case  'm':                           
      /* Turn off Multiple Colors */
      worldToggleColorScheme(&win.world);
      sprintf(win.message,"Increased Detail");
      break;

    case  'c':                           
      /* Toggle Control Points */
      worldTogglePoints(&win.world);
      break;

    case '1':
      win.wqdom = wqFront;
      worldReshapeViewPort(&win.world, win.SizeX, win.SizeY, win.wqdom);
      break;

    case '2':
      win.wqdom = wqSide;
      worldReshapeViewPort(&win.world, win.SizeX, win.SizeY, win.wqdom);
      break;

    case '3':
      win.wqdom = wqTop;
      worldReshapeViewPort(&win.world, win.SizeX, win.SizeY, win.wqdom);
      break;

    case '4':
      win.wqdom = wqPer;
      worldReshapeViewPort(&win.world, win.SizeX, win.SizeY, win.wqdom);
      break;

    case '0':
      win.wqdom = wqNone;
      worldReshapeViewPort(&win.world, win.SizeX, win.SizeY, win.wqdom);
      break;

    case  '+':                           
    case  '=':                           
      /* Add a level of refinement */
      worldIncLevel(&win.world);
      break;

    case  '_':                           
    case  '-':                           
      /* Subtract a level of refinement */
      worldDecLevel(&win.world);
      break;

    default:
      /* Ignore Bad Input */
      sprintf(win.message,"Invalid Key");
      break;
  }

  glutPostRedisplay();
}

void winDisplay()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glFlush();

  if (win.wqdom == wqNone) {
    worldRender(&win.world, wqTop);
    worldRender(&win.world, wqFront);
    worldRender(&win.world, wqSide);
    worldRender(&win.world, wqPer);
    worldRenderDecor(&win.world, win.SizeX, win.SizeY, win.om, win.message);
  } else {
    worldRender(&win.world, win.wqdom);
  }

  glutSwapBuffers();
  glFlush();  
}

void winReshape(int iWidth, int iHeight)
{
  win.SizeX = iWidth;
  win.SizeY = iHeight;
  /* m_rWindow.Display();*/
  worldReshapeViewPort(&win.world, iWidth, iHeight, win.wqdom);
  glutPostRedisplay();
}

void winIdle()
{
  /* This is here for the day that I actually implement an idle function. */
  /*
    if (win.AllowIdle) {
    glutSetWindow(RedisplayWindow);
    glutPostRedisplay();
    }
    */
}


/* Helpful Print Functions */
void winPrintHello()
{
  printf("Mouse Commands:\n\n");
  printf("     L: Move Selected Point\n");
  printf("     M: Select Point\n");
  printf("     R: Change View (depending on Orient mode)\n\n");

  printf("Keyboard Commands:\n\n");
  printf("  o   : Cycle _O_rientation mode (Rotate->Zoom->Spin->Translate)\n");
  printf("  s   : _S_ave surface to \"save.hbs\"\n");
  printf("  p   : Save _P_OV file to \"save.pov\"\n");
  printf("  r   : _R_eset Current Window Orientation\n");
  printf("  +   : Increment current level\n");
  printf("  -   : Decrement current level\n");
  printf("  i   : _I_ncrease Detail\n");
  printf("  d   : _D_decrease Detail\n");
  printf("  f   : Toggle Polygon _F_ill Model ([Solid] | Wireframe)\n");
  printf("  m   : Toggle _M_ulti-Color mode ([On] | Off)\n");
  printf("  c   : Toggle _C_ontrol points ([On] | Off)\n");
  printf("  a   : Toggle _A_xes ([Off] | On)\n");
  printf("  1   : Make The Front View (view 1) Fill The Screen\n");
  printf("  2   : Make The Side  View (view 2) Fill The Screen\n");
  printf("  3   : Make The Top   View (view 3) Fill The Screen\n");
  printf("  4   : Make The Perspective View (view 4) Fill The Screen\n");
  printf("  0   : Make All Views Share The Screen\n");
  /*printf("  c   : Toggle mouse _C_ontrol (change view | [change cube])\n");*/
  /*printf("  p   : Toggle _P_rojection (Orthographic | [Perspective])\n");*/
  printf("  ESC : Quit\n");
  printf("\n");
}


void winSetAllowIdle(BOOL bAllowIdle)
{
  win.AllowIdle = bAllowIdle;

  if ( win.AllowIdle ) {
      glutIdleFunc(winIdle);
  } else  {
    glutIdleFunc(NULL);
  }
}

/* Abstracts the Screen Coordinant system to be Right Handed */
int winDecodePixelX(int iX)
{
  if ( win.OrientedX ) {
    return iX;
  }
  return win.SizeX - iX;
}

int winDecodePixelY(int iY)
{
  if ( win.OrientedY ) {
    return iY;
  }
  return win.SizeY - iY;
}

int winEncodePixelX(int iX)
{
  return(winDecodePixelX(iX));
}

int winEncodePixelY(int iY)
{
  return(winDecodePixelY(iY));
}



/* Abstracts full screen coordinates to viewport coordinates.  
   If (wqdown == wqNone), takes (iX,iY) and returns the WinQuad 
   the point lies in.  If (wqdown != wqNone), assumes the point
   stays in the same WinQuad.  If neither riX nor riY are NULL, 
   places new, translated values in *riX,*riY. */
WinQuad winWhichQuad(int iX, int iY, WinQuad wqdown, WinQuad wqdom, 
		     int* riX, int *riY)
{
  int midX, bot_low, bot_hi, top_low, top_hi;
  WinQuad wq;

  midX = win.SizeX/2;
  bot_low = win.world.Orients[wqPer].vplly;
  bot_hi = win.world.Orients[wqPer].vplly + win.world.Orients[wqPer].vphei;
  top_low = win.world.Orients[wqSide].vplly;
  top_hi = win.world.Orients[wqSide].vplly + win.world.Orients[wqSide].vphei;

  if (wqdom != wqNone) {
    if (riX != NULL && riY != NULL) {
      *riX = iX;
      *riY = iY;
    }
    return wqdom;
  }

  if (wqdown == wqNone) {
    if (iX >= midX) {
      if (iY >= bot_low && iY < bot_hi) {
	wq = wqPer;
      } else if (iY >= top_low && iY < top_hi) {
	wq = wqSide;
      } else {
	wq = wqNone;
      }
    } else {
      if (iY >= bot_low && iY < bot_hi) {
	wq = wqTop;
      } else if (iY >= top_low && iY < top_hi) {
	wq = wqFront;
      } else {
	wq = wqNone;
      }
    }
  } else 
    wq = wqdown;
  
  if (riX != NULL && riY != NULL) {
    switch (wq) {
    case wqFront:
      *riX = iX;
      *riY = iY - top_low;
      break;
    case wqSide:
      *riX = iX-midX;
      *riY = iY - top_low;
      break;
    case wqTop:
      *riX = iX;
      *riY = iY - bot_low;
      break;
    case wqPer:
      *riX = iX-midX;
      *riY = iY - bot_low;
      break;
    default:
    }
  }
  return wq;
}




/* This is a Bicubic B-spline patch rendering function.  It takes a 
   3-D array of floats (i,j,k) organized as follows:

        ^ (0,3,k) (1,3,k) (2,3,k) (3,3,k)
 incr.  | (0,2,k) (1,2,k) (2,2,k) (3,2,k)    k=(0,1,2)=(x,y,z)
  v     | (0,1,k) (1,1,k) (2,1,k) (3,1,k)
        | (0,0,k) (1,0,k) (2,0,k) (3,0,k)
	  ------->
	  incr. u

   ...and renders the patch associated with those 16 points.  The value
   "st" determines whether or not the patch will be rendered as a solid 
   or a wireframe.  The value "level" may be used to determine the color
   of the patch.
 */
void patchRender(GLfloat pts[4][4][3], int level, BOOL MultiC,
		 SolidType st, int subdiv)
{
  /*
  int i,j;
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      printf("point (%d,%d) <%2.3f, %2.3f, %2.3f>\n",i,j,
	     pts[i][j][0], pts[i][j][1], pts[i][j][2]);
    }
  }
  */
  if (MultiC)
    glColor4fv(lev_colors[level%4]);
  else
    glColor4fv(lev_colors[0]);
  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 12, 4, 0, 1, 3, 4, &pts[0][0][0]); 
  if (st == stSolid) {
    glEnable(GL_MAP2_VERTEX_3);
    glEnable(GL_AUTO_NORMAL);
    glFrontFace(GL_CW);
    glMapGrid2f(subdiv, 0, 1, subdiv, 0, 1);
    if (st == stSolid)
      glEvalMesh2(GL_FILL,0,subdiv,0,subdiv);
    else
      glEvalMesh2(GL_LINE,0,subdiv,0,subdiv);
    glDisable(GL_AUTO_NORMAL);

  } else {
    glEnable(GL_MAP2_VERTEX_3);
    glMapGrid2f(subdiv, 0, 1, subdiv, 0, 1);
    glEvalMesh2(GL_LINE,0,subdiv,0,subdiv);
  }

  glDisable(GL_MAP2_VERTEX_3);
}


