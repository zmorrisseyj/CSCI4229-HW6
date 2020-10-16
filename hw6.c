
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

#include "CSCIx229.h"
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
double zh=0;      //  Rotation of stage polygon
double yh=0;      //  Ball position
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=60;   //  Size of world
int distance  =   55;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   5;  // Emission intensity (%)
int ambient   =  40;  // Ambient intensity (%)
int diffuse   =  30;  // Diffuse intensity (%)
int specular  =  30;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
float ylight  =   -10;  // Elevation of light
int ntex = 0;
int lm = 0;      //light mode
int bx = 0;      //manual position of light
int by = 20;
int bz = 0;
int obj = 1;
int light = 1;
unsigned int texture[7]; // Texture names

//  Macro for sin & cos in degrees - BORROWED FROM EX9
#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
 static void cube(double x,double y,double z, //from ex13
                  double dx,double dy,double dz,
                  double th)
 {
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,ntex?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   //  Front
   glColor3f(1,0,0);
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   glColor3f(0,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glColor3f(1,1,0);
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glColor3f(0,1,0);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glColor3f(0,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[5]);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   glColor3f(1,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
 }

static void sphere(double x,double y,double z,
                   double r, int tex) //FROM ex8
{
   const int d=15;
   int th,ph;
   if (tex<0 || tex>7){
     printf("texture does not exist");
     exit(0);
   }

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,ntex?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[tex]);

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         glNormal3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
         glTexCoord2f(Sin(th)*Cos(ph) , Sin(ph)); glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
         glNormal3d(Sin(th)*Cos(ph+d) , Sin(ph+d) , Cos(th)*Cos(ph+d));
         glTexCoord2f(Sin(th)*Cos(ph+d) , Sin(ph+d)); glVertex3d(Sin(th)*Cos(ph+d) , Sin(ph+d) , Cos(th)*Cos(ph+d));
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}


void frame(double x, double y, double z,
           double dx, double dy, double dz,
           double th)
{
  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);
  glColor3f(1,0,0);
  cube(0,2.5,0,  1,2.5,2, 0);
  cube(0,-2.5,1.2,  1,2.5,0.8, 0); //legs
  cube(0,-2.5,-1.2,  1,2.5,0.8,   0);
  cube(0,4.5,-3  ,0.7,0.7,2,   0);//arms
  cube(0,4.5,3  ,0.7,0.7,2,   0);
  sphere(0,5,0,2.5,0);
  glPopMatrix();
}

void stagePoly(double x, double y, double z,
               double dx, double dy, double dz,
               double th, double s){
  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,ntex?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[2]);


  glRotated(zh*s,0,1,0);
  glScaled(0.99,1,1);
  glColor3f(0.1,0.075,0.1);
  glBegin(GL_QUAD_STRIP);

  glNormal3d(0,0,1); //front surface, same normal throughout
  glTexCoord2f(1,1);           glVertex3d(30,0,3);
  glTexCoord2f(0.9167,1);      glVertex3d(25,0,3);

  glTexCoord2f(1,0.5);         glVertex3d(30,-15,3);
  glTexCoord2f(0.9167,0.5);    glVertex3d(25,-15,3);

  glTexCoord2f(0.8333,0.1667); glVertex3d(20,-25,3);
  glTexCoord2f(0.75,0.1667);   glVertex3d(15,-25,3);

  glTexCoord2f(0.5,0);         glVertex3d(0,-30,3);
  glTexCoord2f(0.5,0.1);       glVertex3d(0,-27,3);

  glTexCoord2f(0.25,0.1667);   glVertex3d(-15,-25,3);
  glTexCoord2f(0.1667,0.1667); glVertex3d(-20,-25,3);

  glTexCoord2f(0.0833,0.5);    glVertex3d(-25,-15,3);
  glTexCoord2f(0,0.5);         glVertex3d(-30,-15,3);

  glTexCoord2f(0.0833,1);      glVertex3d(-25,0,3);
  glTexCoord2f(0,1);           glVertex3d(-30,0,3);

  glEnd();


  glBegin(GL_QUAD_STRIP);

  glNormal3d(0,0,-1);//back surface, same normal throughout
  glTexCoord2f(1,1);           glVertex3d(-30,0,-3);
  glTexCoord2f(0.9167,1);      glVertex3d(-25,0,-3);

  glTexCoord2f(1,0.5);         glVertex3d(-30,-15,-3);
  glTexCoord2f(0.9167,0.5);    glVertex3d(-25,-15,-3);

  glTexCoord2f(0.8333,0.1667); glVertex3d(-20,-25,-3);
  glTexCoord2f(0.75,0.1667);   glVertex3d(-15,-25,-3);

  glTexCoord2f(0.5,0);         glVertex3d(0,-30,-3);
  glTexCoord2f(0.5,0.1);       glVertex3d(0,-27,-3);

  glTexCoord2f(0.25,0.1667);   glVertex3d(15,-25,-3);
  glTexCoord2f(0.1667,0.1667); glVertex3d(20,-25,-3);

  glTexCoord2f(0.0833,0.5);    glVertex3d(25,-15,-3);
  glTexCoord2f(0,0.5);         glVertex3d(30,-15,-3);

  glTexCoord2f(0.0833,1);      glVertex3d(25,0,-3);
  glTexCoord2f(0,1);           glVertex3d(30,0,-3);

  glEnd();

  glColor3f(0.2,0.1,0.2);
  glBegin(GL_QUADS);

  glNormal3d(1,0,0); //Outer surface, needs several normals
  glTexCoord2f(1,1); glVertex3d(30,0,-3);
  glTexCoord2f(0,1); glVertex3d(30,0,3);
  glTexCoord2f(0,0); glVertex3d(30,-15,3);
  glTexCoord2f(1,0); glVertex3d(30,-15,-3);

  glNormal3d(1,-1,0);
  glTexCoord2f(1,1); glVertex3d(30,-15,-3);
  glTexCoord2f(0,1); glVertex3d(30,-15,3);
  glTexCoord2f(0,0); glVertex3d(20,-25,3);
  glTexCoord2f(1,0); glVertex3d(20,-25,-3);

  glNormal3d(1,-4,0);
  glTexCoord2f(1,1); glVertex3d(20,-25,-3);
  glTexCoord2f(0,1); glVertex3d(20,-25,3);
  glTexCoord2f(0,0); glVertex3d(0,-30,3);
  glTexCoord2f(1,0); glVertex3d(0,-30,-3);

  glNormal3d(-1,-4,0);
  glTexCoord2f(0,0); glVertex3d(0,-30,-3);
  glTexCoord2f(1,0); glVertex3d(0,-30,3);
  glTexCoord2f(1,1); glVertex3d(-20,-25,3);
  glTexCoord2f(0,1); glVertex3d(-20,-25,-3);

  glNormal3d(-1,-1,0);
  glTexCoord2f(0,0); glVertex3d(-20,-25,-3);
  glTexCoord2f(1,0); glVertex3d(-20,-25,3);
  glTexCoord2f(1,1); glVertex3d(-30,-15,3);
  glTexCoord2f(0,1); glVertex3d(-30,-15,-3);

  glNormal3d(-1,0,0);
  glTexCoord2f(0,0); glVertex3d(-30,-15,-3);
  glTexCoord2f(1,0); glVertex3d(-30,-15,3);
  glTexCoord2f(1,1); glVertex3d(-30,0,3);
  glTexCoord2f(0,1); glVertex3d(-30,0,-3);

  glEnd();



  glBegin(GL_QUADS);

  glBindTexture(GL_TEXTURE_2D,texture[0]);
  glColor3f(0.1,0.075,0.1);
  glNormal3d(1,0,0); //inner surface, needs several normals
  glTexCoord2f(1,1); glVertex3d(-25,0,-3);
  glTexCoord2f(0,1); glVertex3d(-25,0,3);
  glTexCoord2f(0,0); glVertex3d(-25,-15,3);
  glTexCoord2f(1,0); glVertex3d(-25,-15,-3);

  glNormal3d(1,1,0);
  glTexCoord2f(1,1); glVertex3d(-25,-15,-3);
  glTexCoord2f(0,1); glVertex3d(-25,-15,3);
  glTexCoord2f(0,0); glVertex3d(-15,-25,3);
  glTexCoord2f(1,0); glVertex3d(-15,-25,-3);

  glNormal3d(2,15,0);
  glTexCoord2f(1,1); glVertex3d(-15,-25,-3);
  glTexCoord2f(0,1); glVertex3d(-15,-25,3);
  glTexCoord2f(0,0); glVertex3d(0,-27,3);
  glTexCoord2f(1,0); glVertex3d(0,-27,-3);

  glNormal3d(-2,15,0);
  glTexCoord2f(0,0); glVertex3d(0,-27,-3);
  glTexCoord2f(1,0); glVertex3d(0,-27,3);
  glTexCoord2f(1,1); glVertex3d(15,-25,3);
  glTexCoord2f(0,1); glVertex3d(15,-25,-3);

  glNormal3d(-1,1,0);
  glTexCoord2f(0,0); glVertex3d(15,-25,-3);
  glTexCoord2f(1,0); glVertex3d(15,-25,3);
  glTexCoord2f(1,1); glVertex3d(25,-15,3);
  glTexCoord2f(0,1); glVertex3d(25,-15,-3);

  glNormal3d(-1,0,0);
  glTexCoord2f(0,0); glVertex3d(25,-15,-3);
  glTexCoord2f(1,0); glVertex3d(25,-15,3);
  glTexCoord2f(1,1); glVertex3d(25,0,3);
  glTexCoord2f(0,1); glVertex3d(25,0,-3);

  glEnd();

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINE_STRIP);
  glColor3f(1,0,1); //Outer surface highlight
  glVertex3d(30,-1,-3);

  glVertex3d(30,-15,-3);
  glVertex3d(30,-15,3);

  glVertex3d(20,-25,3);
  glVertex3d(20,-25,-3);

  glVertex3d(0,-30,-3);
  glVertex3d(0,-30,3);

  glVertex3d(-20,-25,3);
  glVertex3d(-20,-25,-3);

  glVertex3d(-30,-15,-3);
  glVertex3d(-30,-15,3);

  glVertex3d(-30,-1,3);
  glEnd();

  glRotated(45,0,1,0);
  glBegin(GL_LINE_STRIP);
  glColor3f(1,0.5,0); //Outer wire highlight
  glVertex3d(30,-1,-3);

  glVertex3d(30,-15,-3);
  glVertex3d(30,-15,3);

  glVertex3d(20,-25,3);
  glVertex3d(20,-25,-3);

  glVertex3d(0,-30,-3);
  glVertex3d(0,-30,3);

  glVertex3d(-20,-25,3);
  glVertex3d(-20,-25,-3);

  glVertex3d(-30,-15,-3);
  glVertex3d(-30,-15,3);

  glVertex3d(-30,-1,3);
  glEnd();

  glPopMatrix();

}

void stage(double x,double y, double z,
           double dx, double dy, double dz,
           double th)
{
  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,ntex?GL_REPLACE:GL_MODULATE);
  glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[1]); //gradient texture for octagonal surfaces

  glColor3f(0.2,0.2,0.2);
  glBegin(GL_POLYGON); //Top octagon
  glNormal3d(0,1,0);
  glTexCoord2f(1,0.333); glVertex3d(50,0,10);
  glTexCoord2f(1,0.667); glVertex3d(50,0,-10);
  glTexCoord2f(0.8,1);   glVertex3d(30,0,-30);
  glTexCoord2f(0.2,1);   glVertex3d(-30,0,-30);
  glTexCoord2f(0,0.667); glVertex3d(-50,0,-10);
  glTexCoord2f(0,0.333); glVertex3d(-50,0,10);
  glTexCoord2f(0.2,0);   glVertex3d(-30,0,30);
  glTexCoord2f(0.8,0);   glVertex3d(30,0,30);
  glTexCoord2f(1,0.333); glVertex3d(50,0,10);
  glEnd();


  glBegin(GL_POLYGON); //bottom octagon
  glNormal3d(0,-1,0);
  glTexCoord2f(1,0.333); glVertex3d(50,-1,10);
  glTexCoord2f(1,0.667); glVertex3d(50,-1,-10);
  glTexCoord2f(0.8,1);   glVertex3d(30,-1,-30);
  glTexCoord2f(0.2,1);   glVertex3d(-30,-1,-30);
  glTexCoord2f(0,0.667); glVertex3d(-50,-1,-10);
  glTexCoord2f(0,0.333); glVertex3d(-50,-1,10);
  glTexCoord2f(0.2,0);   glVertex3d(-30,-1,30);
  glTexCoord2f(0.8,0);   glVertex3d(30,-1,30);
  glTexCoord2f(1,0.333); glVertex3d(50,-1,10);
  glEnd();


  glBindTexture(GL_TEXTURE_2D,texture[0]); //want matte texture for these
  glBegin(GL_QUADS); //border quads to complete octagonal disk
  glNormal3d(1,0,0);
  glTexCoord2f(0,1); glVertex3d(50,0,10);
  glTexCoord2f(0,0); glVertex3d(50,-1,10);
  glTexCoord2f(1,0); glVertex3d(50,-1,-10);
  glTexCoord2f(1,1); glVertex3d(50,0,-10);

  glNormal3d(1,0,-1);
  glTexCoord2f(0,0); glVertex3d(50,-1,-10);
  glTexCoord2f(0,1); glVertex3d(50,0,-10);
  glTexCoord2f(1,1); glVertex3d(30,0,-30);
  glTexCoord2f(1,0); glVertex3d(30,-1,-30);

  glNormal3d(0,0,-1);
  glTexCoord2f(0,1); glVertex3d(30,0,-30);
  glTexCoord2f(0,0); glVertex3d(30,-1,-30);
  glTexCoord2f(1,0); glVertex3d(-30,-1,-30);
  glTexCoord2f(1,1); glVertex3d(-30,0,-30);

  glNormal3d(-1,0,-1);
  glTexCoord2f(1,0); glVertex3d(-50,-1,-10);
  glTexCoord2f(1,1); glVertex3d(-50,0,-10);
  glTexCoord2f(0,1); glVertex3d(-30,0,-30);
  glTexCoord2f(0,0); glVertex3d(-30,-1,-30);

  glNormal3d(-1,0,0);
  glTexCoord2f(1,1); glVertex3d(-50,0,10);
  glTexCoord2f(1,0); glVertex3d(-50,-1,10);
  glTexCoord2f(0,0); glVertex3d(-50,-1,-10);
  glTexCoord2f(0,1); glVertex3d(-50,0,-10);

  glNormal3d(-1,0,1);
  glTexCoord2f(0,0); glVertex3d(-50,-1,10);
  glTexCoord2f(0,1); glVertex3d(-50,0,10);
  glTexCoord2f(1,1); glVertex3d(-30,0,30);
  glTexCoord2f(1,0); glVertex3d(-30,-1,30);

  glNormal3d(0,0,1);
  glTexCoord2f(1,1); glVertex3d(30,0,30);
  glTexCoord2f(1,0); glVertex3d(30,-1,30);
  glTexCoord2f(0,0); glVertex3d(-30,-1,30);
  glTexCoord2f(0,1); glVertex3d(-30,0,30);

  glNormal3d(1,0,1);
  glTexCoord2f(0,0); glVertex3d(50,-1,10);
  glTexCoord2f(0,1); glVertex3d(50,0,10);
  glTexCoord2f(1,1); glVertex3d(30,0,30);
  glTexCoord2f(1,0); glVertex3d(30,-1,30);

  glColor3f(0.1,0.1,0.1); //Sloped quads under stage
  glNormal3d(1,-1,0);
  glTexCoord2f(0,1); glVertex3d(40,0,10);
  glTexCoord2f(1,1); glVertex3d(40,0,-10);
  glTexCoord2f(1,0); glVertex3d(30,-10,-10);
  glTexCoord2f(0,0); glVertex3d(30,-10,10);

  glNormal3d(-1,-1,0);
  glTexCoord2f(1,1); glVertex3d(-40,0,10);
  glTexCoord2f(0,1); glVertex3d(-40,0,-10);
  glTexCoord2f(0,0); glVertex3d(-30,-10,-10);
  glTexCoord2f(1,0); glVertex3d(-30,-10,10);

  glNormal3d(0,-1,1);
  glTexCoord2f(1,1); glVertex3d(30,0,20);
  glTexCoord2f(0,1); glVertex3d(-30,0,20);
  glTexCoord2f(0,0); glVertex3d(-30,-10,10);
  glTexCoord2f(1,0); glVertex3d(30,-10,10);

  glNormal3d(0,-1,-1);
  glTexCoord2f(0,1); glVertex3d(30,0,-20);
  glTexCoord2f(1,1); glVertex3d(-30,0,-20);
  glTexCoord2f(1,0); glVertex3d(-30,-10,-10);
  glTexCoord2f(0,0); glVertex3d(30,-10,-10);

  glNormal3d(0,-1,0);
  glTexCoord2f(1,1); glVertex3d(30,-10,10);
  glTexCoord2f(1,0); glVertex3d(30,-10,-10);
  glTexCoord2f(0,0); glVertex3d(-30,-10,-10);
  glTexCoord2f(0,1); glVertex3d(-30,-10,10);

  glEnd();


  glBegin(GL_TRIANGLES);//corner tri's for sloped quads
  glNormal3d(1,-1,1);
  glTexCoord2f(0,1);   glVertex3d(40,0,10);
  glTexCoord2f(0.5,0); glVertex3d(30,-10,10);
  glTexCoord2f(1,1);   glVertex3d(30,0,20);

  glNormal3d(-1,-1,1);
  glTexCoord2f(0,1);   glVertex3d(-40,0,10);
  glTexCoord2f(0.5,0); glVertex3d(-30,-10,10);
  glTexCoord2f(1,1);   glVertex3d(-30,0,20);

  glNormal3d(1,-1,-1);
  glTexCoord2f(0,1);   glVertex3d(40,0,-10);
  glTexCoord2f(0.5,0); glVertex3d(30,-10,-10);
  glTexCoord2f(1,1);   glVertex3d(30,0,-20);

  glNormal3d(-1,-1,-1);
  glTexCoord2f(0,1);   glVertex3d(-40,0,-10);
  glTexCoord2f(0.5,0); glVertex3d(-30,-10,-10);
  glTexCoord2f(1,1);   glVertex3d(-30,0,-20);
  glEnd();


  float purple[] = {0.5,0,0.5,1.0};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT,GL_SPECULAR,purple);
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
  //Under Stage Rotating Polyons with Highlights
  stagePoly(0,0,0,0.99,1,1,th,1);
  glEnable(GL_LIGHTING);
  stagePoly(0,0,0,0.99,1,1,th+90,1);
  glEnable(GL_LIGHTING);
  stagePoly(0,0,0,0.95,0.95,0.95,th,-0.5);
  glEnable(GL_LIGHTING);
  stagePoly(0,0,0,0.95,0.95,0.95,th+90,-0.5);


  glColor3f(0.1,0.075,0.5);
  glRotated(0.5*zh,0,1,0);
  sphere(0, -15, 0, 10,2);
  glRotated(-0.5*zh,0,1,0);


  glDisable(GL_TEXTURE_2D); //only highlight lines from here, no lighting or textures
  glDisable(GL_LIGHTING);

  glColor3f(1,0,1);
  glLineWidth(2);
  glBegin(GL_LINE_STRIP); //Top stage outline
  glVertex3d(50.1,0,10.1);
  glVertex3d(50.1,0,-10.1);
  glVertex3d(30.1,0,-30.1);
  glVertex3d(-30.1,0,-30.1);
  glVertex3d(-50.1,0,-10.1);
  glVertex3d(-50.1,0,10.1);
  glVertex3d(-30.1,0,30.1);
  glVertex3d(30.1,0,30.1);
  glVertex3d(50.1,0,10.1);
  glEnd();

  glBegin(GL_LINE_STRIP); //Middle stage outline
  glVertex3d(50.1,-1,10.1);
  glVertex3d(50.1,-1,-10.1);
  glVertex3d(30.1,-1,-30.1);
  glVertex3d(-30.1,-1,-30.1);
  glVertex3d(-50.1,-1,-10.1);
  glVertex3d(-50.1,-1,10.1);
  glVertex3d(-30.1,-1,30.1);
  glVertex3d(30.1,-1,30.1);
  glVertex3d(50.1,-1,10.1);
  glEnd();

  glBegin(GL_LINE_STRIP); //Lower stage outline
  glVertex3d(50.1,-2,10.1);
  glVertex3d(50.1,-2,-10.1);
  glVertex3d(30.1,-2,-30.1);
  glVertex3d(-30.1,-2,-30.1);
  glVertex3d(-50.1,-2,-10.1);
  glVertex3d(-50.1,-2,10.1);
  glVertex3d(-30.1,-2,30.1);
  glVertex3d(30.1,-2,30.1);
  glVertex3d(50.1,-2,10.1);
  glEnd();

  glPopMatrix();
}



static void Vertex(double th,double ph) //BORROWED FROM EX13
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}
/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r) //BORROWED FROM EX13
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

void display()
{
   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0); //Perspective view looking at origin, will always be the case in my scene, may add other point to look at later once more functionality is implemented

   if (light) //if block borrowed from ex13
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(yh),ylight,distance*Sin(yh),1.0};
      //  Draw light position as ball (still no lighting here)
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);

      switch (lm){
        case 0:
          glColor3f(1,1,1);
          ball(Position[0],Position[1],Position[2] , 2);
          glLightfv(GL_LIGHT0,GL_POSITION,Position);
          break;
        case 1:
          Position[0] = bx;
          Position[1] = by;
          Position[2] = bz;
          ball(bx,by,bz,2);
          glLightfv(GL_LIGHT0,GL_POSITION,Position);
      }
   }
   else
     glDisable(GL_LIGHTING);

   switch (obj){
     case 0:
       frame(0,0,0,5,5,5,0);
       break;
     case 1:
       frame(-10,5,0,  1,1,1, 0);
       frame(20,10,0,  1,2,1, 80);
       stage(1,1,1,1,1,1,0);
       break;
   }
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed - partially BORROWED FROM EX9
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;

   //  Update projection
   Project(fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed - BORROWED FROM EX9
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle and manual light
   else if (ch == '0')
   {
      th = ph = bx = bz = 0;
      by = 20;
      emission  =   5;
      ambient   =  10;
      diffuse   =  50;
      specular  =   5;
      shininess =   0;
    }
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = (mode + 1)%3;
   //  Change field of view angle
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   else if (ch=='[')
      ylight -= 1;
   else if (ch==']')
      ylight += 1;
   else if (ch == 'x')
      bx -= 1;
   else if (ch == 'X')
      bx += 1;
   else if (ch == 'y')
      by -= 1;
   else if (ch == 'Y')
      by += 1;
   else if (ch == 'z')
      bz -= 1;
   else if (ch == 'Z')
      bz += 1;
   else if (ch == 'b'||ch == 'B')
      lm = 1-lm;
   else if (ch == 't'||ch == 'T')
      ntex = 1-ntex;
   else if (ch == 'o'||ch == 'O')
      obj = 1-obj;
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
  //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
  //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
  //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   //  Reproject
   Project(fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}
/*
 *  GLUT calls this routine when the window is resized - BORROWED FROM EX9
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(fov,asp,dim);
}

void idle()// borrowed from ex8 !!!!
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   yh = fmod(90*t,360);
   glutPostRedisplay();
}

 int main(int argc, char* argv[])
 {
   glutInit(&argc,argv); //Initialize glut with user input
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); //Initialize RGB, double buffering, and depth testing
   glutInitWindowSize(1000,1000); //Initialize window size
   glutCreateWindow("Homework 6 - Zach Morrissey - Textures"); //Create window with input string as title
   glutIdleFunc(idle);
   glutDisplayFunc(display); //calls the display function which draws the program
   glutReshapeFunc(reshape);
   glutSpecialFunc(special); //calls a function for arrow key interaction
   glutKeyboardFunc(key); //calls a function for keyboard interaction

   texture[0] = LoadTexBMP("matte.bmp");
   texture[1] = LoadTexBMP("mattegradient.bmp");
   texture[2] = LoadTexBMP("static.bmp");
   texture[3] = LoadTexBMP("img3.bmp");
   texture[4] = LoadTexBMP("img4.bmp");
   texture[5] = LoadTexBMP("img5.bmp");
   texture[6] = LoadTexBMP("img6.bmp");

   glutMainLoop();

   return 0;
 }
