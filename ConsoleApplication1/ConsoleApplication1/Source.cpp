/****************************************************************************************************
* Influence map example using convolution with Gaussian filter to spread out influence.
#define GRIDSIZE is used to set map size - here its 32 by 32.
Press space bar to trigger a pass of the filter over the map.
Some messy techniques here! - be careful.
****************************************************************************************************/


////// Includes
#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <math.h>
#include <cmath>
#include <iostream>
using namespace std;
#define GRIDSIZE 32


////// Global Variables
typedef struct rgbtag {
	float r;
	float g;
	float b;
}rgb;

typedef struct vector {
	float x; // position of grid x
	float y; // position of grid y

	float redinfluencex;  // position of red influence x axis
	float redinfluencey;  // position of red influence y axis
	float blueinfluencex; // position of blue influence x axis
	float blueinfluencey; // position of blue influence y axis

	float radius; 
	float influence; 
}vec;

typedef struct AIbot {
	int x; // position of AI bot x axis
	int y; // position of AI bot y axis
}ai;

ai aibot;
vec pos;

float id1 = 1.0f; // Influence calculation
float id2 = 1.0f;
float r = 2.0; // Radius
float d1; // Distance calculation
float d2;
float maxinfluence = 0.0;
float redinf = 0.0; // red influence
float blueinf = 0.0; // blue influence

rgb grid[GRIDSIZE][GRIDSIZE];
rgb gridtemp[GRIDSIZE*GRIDSIZE];
//function prototypes
void init(void);
void display(void);
void drawBox(float w, float h, float l);
void reshape(int w, int h);
void aibotmove();

void init()
// Initialize
//************************************************************************
// desc: initializes OpenGL
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// clears colour buffer 
	glEnable(GL_DEPTH_TEST);					// hidden surface removal
	glEnable(GL_CULL_FACE);						// do not calculate inside of poly's
	glFrontFace(GL_CCW);						// counter clock-wise polygons are out

	for (int i = 0; i<GRIDSIZE; i++) {
		for (int j = 0; j<GRIDSIZE; j++) {

			grid[i][j].r = 0;
			grid[i][j].g = 0;
			grid[i][j].b = 0;
		}
	}

	// Positions for grid
	pos.x = 0.0;
	pos.y = 0.0;

	// positions for red influence
	pos.redinfluencex = 10.0;
	pos.redinfluencey = 10.0;

	// positions for blue influence
	pos.blueinfluencex = 20.0;
	pos.blueinfluencey = 20.0;

	pos.radius = 4.0;
	pos.influence = 2.0;

	// Ai bot positions
	aibot.x = 15.0;
	aibot.y = 15.0;
}

void drawBox(float w, float h, float l)
//drawBox
//***************************************************************************
//This function first translates a further x,y,z units then draws a box of
//width w, height h, length l
{
	glPushMatrix();       //save modelview 
	w = w / 2.0; h = h / 2.0; l = l / 2.0; //adjust values so centre is in middle of box
										   //draw faces of box
	glBegin(GL_POLYGON);
	glVertex3f(w, l, h);
	glVertex3f(w, l, -h);
	glVertex3f(-w, l, -h);
	glVertex3f(-w, l, h);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(w, l, h);
	glVertex3f(-w, l, h);
	glVertex3f(-w, -l, h);
	glVertex3f(w, -l, h);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(w, l, h);
	glVertex3f(w, -l, h);
	glVertex3f(w, -l, -h);
	glVertex3f(w, l, -h);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-w, l, h);
	glVertex3f(-w, l, -h);
	glVertex3f(-w, -l, -h);
	glVertex3f(-w, -l, h);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-w, -l, h);
	glVertex3f(-w, -l, -h);
	glVertex3f(w, -l, -h);
	glVertex3f(w, -l, h);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(w, -l, -h);
	glVertex3f(-w, -l, -h);
	glVertex3f(-w, l, -h);
	glVertex3f(w, l, -h);
	glEnd();
	glPopMatrix();   //restore previous modelview matrix so leaving things as you found them 
}


// display
//************************************************************************
// desc: handles drawing of scene
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear screen and depth buffer
	glLoadIdentity();										// reset modelview matrix


	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //reset camera position
	glColor3f(0, 0, 0);  //set colour
	glTranslatef(0.5, 0.5, 0.5);  //move by x,y and z units
								  //tests grid by applying greyscale values to each position
								  //
	for (int i = 0; i<GRIDSIZE; i++) {
		for (int j = 0; j<GRIDSIZE; j++) {
			if (grid[i][j].r>grid[i][j].b)
				{
					glColor3f(grid[i][j].r, 0, 0);
				}
				else
				{
					glColor3f(0, 0, grid[i][j].b);  //set colour		  }
				}
				
			// create ai bot on screen
			if (aibot.x == i && aibot.y == j)
			{
				glColor3f(1, 1, 0);
			}

			glPushMatrix(); //save current position/orientation, etc.
			glTranslatef(i, j, 0);
			drawBox(1, 1, 1);  //draw 
			glPopMatrix(); //restore previous "current" position/orientation etc.
		}
	}


	glFlush();  //force drawing

	glutSwapBuffers();			// bring backbuffer to foreground
								//any errors then display error codes 
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		errString = gluErrorString(errCode);
		fprintf(stderr, "OpenGL error : %s\n", errString);
	}

}

// Distance Checks with Red influence and Blue influence
void keyboardread(unsigned char key, int x, int y)
{


	switch (key) {
	case 'u':

		break;
	case ' ':

		maxinfluence = 0.0;

		for (int i = 1; i < GRIDSIZE - 1; i++) {
			for (int j = 1; j < GRIDSIZE - 1; j++)
			{
				pos.x = i;
				pos.y = j;
				d1 = 0;
				d2 = 0;
				id1 = 0;
				id2 = 0;
				
				d1 = sqrt(((pos.redinfluencex - pos.x)*(pos.redinfluencex - pos.x)) + ((pos.redinfluencey - pos.y)*(pos.redinfluencey - pos.y)));

				id1 = (pos.influence / exp(d1 - pos.radius) + 1.0);

				printf("red influence: %f \n", id1);

				// Setting the red colour of the grid
				grid[i][j].r = id1;

				// Normalize influence
				if (maxinfluence < id1)
				{
					maxinfluence = id1;
				}

				redinf = id1;

				
				d2 = sqrt(((pos.blueinfluencex - pos.x)*(pos.blueinfluencex - pos.x)) + ((pos.blueinfluencey - pos.y)*(pos.blueinfluencey - pos.y)));

				id2 = (pos.influence / exp(d2 - pos.radius) + 1.0);

				printf("red influence: %f \n", id2);

				// Setting the blue colour of the grid
				grid[i][j].b = id2;

				// Normalize influence
				if (maxinfluence < id2)
				{
					maxinfluence = id2;
				}

				blueinf = id2;

			} 
		}

		// Normalizing max influence by colour
		for (int i = 1; i < GRIDSIZE - 1; i++)
		{
			for (int j = 1; j < GRIDSIZE - 1; j++)
			{
				grid[i][j].r /= maxinfluence;
			}

			for (int j = 1; j < GRIDSIZE - 1; j++)
			{
				grid[i][j].b /= maxinfluence;
			}
		}
	
		aibotmove(); 

		glutPostRedisplay();
				break;
			}
		
	
}

// Make the ai move to max influence
void aibotmove()
{
	float influence = -999;
	ai temp;
	float tempUp = 0.0, tempDown = 0.0, tempLeft = 0.0, tempRight = 0.0;

	rgb lookUp, lookDown, lookLeft, lookRight; // Make ai bot look up, down, left and right
	lookUp = grid[aibot.x][aibot.y + 1]; 
	lookDown = grid[aibot.x][aibot.y - 1];
	lookLeft = grid[aibot.x - 1][aibot.y];
	lookRight = grid[aibot.x + 1][aibot.y];

	// Move AI bot to highest influence
	tempUp = lookUp.b - lookUp.r;
	tempDown = lookDown.b - lookDown.r;
	tempLeft = lookLeft.b - lookLeft.r;
	tempRight = lookRight.b - lookRight.r;


		if (tempUp > influence )
			{
				influence = tempUp;
				temp.x = aibot.x;
				temp.y = aibot.y + 1;
			}
		if (tempDown > influence)
			{
				influence = tempDown;
				temp.x = aibot.x;
				temp.y = aibot.y - 1;
			}
		if (tempLeft > influence)
			{
				influence = tempLeft;
				temp.x = aibot.x - 1;
				temp.y = aibot.y;
			}
		if (tempRight > influence)
			{
				influence = tempRight;
				temp.x = aibot.x + 1;
				temp.y = aibot.y;
			}

		aibot = temp;
}

void reshape(int w, int h)
//reshape
//************************************************************************
//OpenGL reshape callback is called when the window is resized
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h); //respecify size of view port to match new w and h
	glMatrixMode(GL_PROJECTION);  //go into projection mode
	glLoadIdentity();   //reset projection matrix
	glOrtho(0.0, GRIDSIZE, 0.0, GRIDSIZE, 0.0, 100.0); //defines orthoganal projection (parrallel
													   // viewing volume
													   // window limits of display
													   // left =0.0, right =2.0,
													   // bottom = 0.0, top = 2.0
													   // near =0.0, far = 100.0
													   //gluPerspective(60.0,(GLfloat)w/(GLfloat)h,1.0,200.0);  //redefine projection matrix to match changes in w and h
	glMatrixMode(GL_MODELVIEW);  //switch back to modelview mode
	glLoadIdentity(); //reset modelview matrix
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //reset camera position
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardread);
	glutMainLoop();
	return 0;
}