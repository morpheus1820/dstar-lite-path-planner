#ifdef MACOS
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h> 
#else
#include <GL/glut.h>
#include <GL/gl.h> 
#include <GL/glu.h>
#endif

#include <unistd.h>
#include "Dstar.h"

#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>


using namespace cv;

int hh, ww;

int window; 
Dstar *dstar;

int scale = 2; //6
int mbutton = 0;
int mstate = 0;

bool b_autoreplan = true;

Mat streets;
bool showStreets=false;

void InitGL(int Width, int Height)
{
  hh = Height;
  ww = Width;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0);	

  glViewport(0,0,Width,Height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,Width,0,Height,-100,100);
  glMatrixMode(GL_MODELVIEW);

}

void ReSizeGLScene(int Width, int Height)
{
  hh = Height;
  ww = Width;

  glViewport(0,0,Width,Height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,Width,0,Height,-100,100);
  glMatrixMode(GL_MODELVIEW);

}

void drawStreets()
{
  glEnable (GL_BLEND);
glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    for(int i=0;i<streets.cols;i++)
      for(int j=0;j<streets.rows;j++)
      {
        Vec3b color=streets.at<Vec3b>(j,i);
        glColor4f(color[0]/255.0,color[1]/255.0,color[2]/255.0,0.4);
        printf("%f %f %f\n",color[0]/255.0,color[1]/255.0,color[2]/255.0);
        float x = i;
        float y = j;
        
        float size=0.45;
        glVertex2f(x - size, y - size);
        glVertex2f(x + size, y - size);
        glVertex2f(x + size, y + size);
        glVertex2f(x - size, y + size);

      }
  glEnd();
  glDisable (GL_BLEND);
}

void DrawGLScene()
{

  usleep(100);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  glPushMatrix();

  glScaled(scale,scale,1);

  //if (b_autoreplan) {dstar->replan();dstar->getWaypoints();}

  if(showStreets) drawStreets();

  dstar->draw();
  dstar->drawWaypoints();

  glPopMatrix();
  glutSwapBuffers();

}


void keyPressed(unsigned char key, int x, int y) 
{
  usleep(100);

  switch(key) {
  case 'q':
  case 'Q':
    glutDestroyWindow(window); 
    exit(0);
    break;
  case 'r':
  case 'R':
    dstar->replan();
    break;
  case 'a':
  case 'A':
    b_autoreplan = !b_autoreplan;
    break;
  case 'w':
  case 'W':
    dstar->getWaypoints();
    break;
  case 'c':
  case 'C':
    dstar->init(40,50,140, 90);
    break;
  }
    
}

void mouseFunc(int button, int state, int x, int y) {
  
  y = hh -y+scale/2;
  x += scale/2;

  mbutton = button;

  if ((mstate = state) == GLUT_DOWN) {
    if (button == GLUT_MIDDLE_BUTTON) {
      dstar->updateCell(x/scale, y/scale, -1); //-1
    } else if (button == GLUT_RIGHT_BUTTON) {
      dstar->updateStart(x/scale, y/scale);
    } else if (button == GLUT_LEFT_BUTTON) {
      dstar->updateGoal(x/scale, y/scale);
    }
  }
  dstar->replan();dstar->getWaypoints();
}

void mouseMotionFunc(int x, int y)  {

  y = hh -y+scale/2;
  x += scale/2;
  
  y /= scale;
  x /= scale;
  
  if (mstate == GLUT_DOWN) {
    if (mbutton == GLUT_MIDDLE_BUTTON) {
      dstar->updateCell(x, y, -1);
    } else if (mbutton == GLUT_RIGHT_BUTTON) {
      dstar->updateStart(x, y);
    } else if (mbutton == GLUT_LEFT_BUTTON) {
      dstar->updateGoal(x, y);
    }
  }

}

void loadBuildings(string filename)
{
  Mat buildings=imread(filename,0);
  threshold( buildings, buildings, 100, 255,0 );
  Mat element = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 5,5 ),
                                       Point( 2, 2 ) );
  /// Apply the dilation operation
  //erode( buildings, buildings, element );
  resize(buildings,buildings,Size(800/scale,600/scale));
  flip(buildings,buildings,1);

  dstar->setObstacles(buildings);
//imshow("build",buildings); cvWaitKey(0);
  for(int i=0;i<buildings.cols;i++)
      for(int j=0;j<buildings.rows;j++)
      {
        int x=i; int y=j;
  // y = hh -y+scale/2;
  // x += scale/2;
  
  // y /= scale;
  // x /= scale;
  
  unsigned char cost= buildings.at<unsigned char>(j,i);  
  if(cost<10)
      dstar->updateCell(x, y, -1);
  }

  for(int i=0;i<800;i++)
  	{dstar->updateCell(i, 0, -1);dstar->updateCell(i, 600/scale, -1);}

  for(int j=0;j<600;j++)
  	{dstar->updateCell(0, j, -1);dstar->updateCell(800/scale, j, -1);}

}

void loadLte(string filename)
{
  Mat lte=imread(filename,0);  
  resize(lte,lte,Size(800/scale,600/scale));
    flip(lte,lte,1);

//imshow("build",buildings); cvWaitKey(0);
  for(int i=0;i<lte.cols;i++)
      for(int j=0;j<lte.rows;j++)
      {
        int x=i; int y=j;
  // y = hh -y+scale/2;
  // x += scale/2;
  
  // y /= scale;
  // x /= scale;
  
  unsigned char cost= lte.at<unsigned char>(j,i);  
  if(cost < 20){
  	dstar->updateCell(x, y, -2);
      //dstar->updateCell(x, y, cost/500.0);
      //printf("cost %f\n",cost/500.0);
  }
  }
}

void loadStreets(string filename)
{
  streets=imread(filename,1);  
  resize(streets,streets,Size(800/scale,600/scale));
    flip(streets,streets,1);

}



int main(int argc, char **argv) {

  if(argc<2) 
  {

  	printf("Usage: program buildings_image [lte_image]\n");
  	return -1;
  }	

  glutInit(&argc, argv);  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);  
  glutInitWindowSize(800, 600);  
  glutInitWindowPosition(20, 20);  
    
  window = glutCreateWindow("ODOMI Path Planner");  
  
  glutDisplayFunc(&DrawGLScene);  
  glutIdleFunc(&DrawGLScene);
  glutReshapeFunc(&ReSizeGLScene);
  glutKeyboardFunc(&keyPressed);
  glutMouseFunc(&mouseFunc);
  glutMotionFunc(&mouseMotionFunc);

  InitGL(800, 600);

  dstar = new Dstar();
  dstar->init(30,50,120, 90);
  
  if(argc>3){ 
    loadStreets(argv[3]); showStreets=true;}
  if(argc>2)
  	loadLte(argv[2]);

  loadBuildings(argv[1]);

  printf("----------------------------------\n");
  printf("ODOMI Path Planner\n");
  printf("Commands:\n");
  printf("[q/Q] - Quit\n");
  printf("[r/R] - Replan\n");
  printf("[a/A] - Toggle Auto Replan\n");
  printf("[c/C] - Clear (restart)\n");
  printf("middle mouse click - make cell untraversable (cost -1)\n");
  printf("left mouse click - move goal to cell\n");
  printf("right mouse click - move start to cell\n");
  printf("----------------------------------\n");

  printf("Map size (km): %f %f\n", dstar->haversine_km(45.062319,7.662808,45.065402,7.662808),
  								dstar->haversine_km(45.062319,7.662808,45.062319,7.656784));

  glutMainLoop();  

  return 1;
}
