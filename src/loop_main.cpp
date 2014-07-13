#include <GL/glut.h>
#include <GL/gl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<iostream>

#include "Utility.h"

using namespace std;

float rX=0.0,rY=0.0;
char fileName[25];
int plyFaceDimensionCount=0;
int displayControlMesh=0, n=0, subdivisionLevel = 0, X, Y;
float tx=-2.0,ty=-2.0,tz=0.0, scale=0.2;
GLuint program  = 0;
GLchar *ebuffer;
GLsizei elength;

	
void reshape(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.01f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void display() {
  

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 0.5f,0.0f, 0.0f,  0.0f,0.0f, 1.0f,  0.0f);
	
	glScalef(scale,scale, 0);
	glTranslatef(tx,ty,tz);
	glRotatef(rX,0,1,0);
	glRotatef(rY,1,0,0);
	
	Utility objUtitlity;
	objUtitlity.render(displayControlMesh, n);
	
	glFlush();
	glutSwapBuffers();
	
}

void mouseRotate(int x,int y){
	if ( displayControlMesh == 0 )
	{
		X=x;
		Y=y;
		glutPostRedisplay();
	}
}

void moveMouse(int x,int y){
	const float  s=1;
	if( displayControlMesh == 0 )
	{
		rX+=(X-x)/s;
		rY+=(Y-y)/s;
		mouseRotate(x,y);
		glutPostRedisplay();
	}
}

void init_lighting()
{
	GLfloat light0_ambient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat light0_diffuse[] = {0.8, 0.8, 0.8, 1.0};
	GLfloat light0_position[] =	{-150, -10.0, -730, 1.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_LIGHTING);
  	glEnable(GL_LIGHT0);  	  
  	    
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
}

void keyPressed(unsigned char key, int x, int y) {  
  
  Utility objUtitlity3;
  
  
		if(key == 'W' || key == 'w') // translate up ; key = 'w'
		{ 				
			
			if (displayControlMesh == 1)
			{
				
				objUtitlity3.interactivity(n, 1);
			}
			else
				ty += 0.05;
										
		}
		  
		if(key == 'a' || key == 'A')  // translate right; key = 'd'
		{
			if (displayControlMesh == 1)
			{
				objUtitlity3.interactivity(n, 2);
			}
			else
				tx -= 0.05;							
		}
		
		if(key == 's' || key == 'S')  // traslate down; key = 's'
		{			
			if (displayControlMesh == 1)
			{
				objUtitlity3.interactivity(n, 3);
			}
			else
				ty -= 0.05;					
		}
		
		if(key == 'd' || key == 'D')  // translate left; key = 'd'
		{
			if (displayControlMesh == 1)
			{
				objUtitlity3.interactivity(n, 4);
			}
			else
				tx += 0.05;	
		}
		
		if(key == 'i' || key == 'I')  // zoom in: key 'i' 
		{
			scale += 0.05;								
		}
		if(key == 'o' || key == 'O')  // zoom out: key 'o'
		{
			scale -= .05;									
		}
		if(key == 27 ) // esc. to exit ..
		{
			exit(0);
		}
		if(key == '+')
		{
			
				Utility objUtitlity2;
				objUtitlity2.subdivision();
				subdivisionLevel += 1;
		}
		if(key == 'e' || key == 'E' )  // edit mode
		{
	
			if(displayControlMesh == 1)	
				displayControlMesh = 0;
			else	
				displayControlMesh = 1;
				
			n=0;  // resetting			
		}
		if( key == 'n' || key == 'N' )   // to jump next vertex
		{
			if ( displayControlMesh == 0)
				return;
				
			n++;
		}		
		glutPostRedisplay();	
}

	

int main(int argc, char** argv)
{
	
	if (argc != 2) 
	{
		std::cout << "Provide ply filename as argument.\n Ex: ./PlyRenderer bunny.ply %d" << argc;
		return 0;
	} 
	else 
	{
		strcpy(fileName, argv[1]);
		Utility obj;
	//  call to loadplyfile
		obj.loadPlyFileToWingedEdged(fileName);
	}
	
	init_lighting();
	
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(900,900);
    glutInitWindowPosition(100,100);
    glutCreateWindow("  ::Interactive mesh subdivision::  ");
    glutMotionFunc(moveMouse);
	glutPassiveMotionFunc(mouseRotate);
    glutKeyboardFunc(keyPressed);
    glutDisplayFunc(display);
	glutMainLoop();    
	return 0;
}

