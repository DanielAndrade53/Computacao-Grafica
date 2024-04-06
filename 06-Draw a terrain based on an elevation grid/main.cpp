#include<stdio.h>
#include<stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <IL/il.h>			// adicionar include

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <iostream>

using namespace std;

/*
Adicionei eu
*/
GLuint buffers[1];
size_t imageHeight, imageWitdth;
/*
Ate aqui
*/

float camX = 00, camY = 30, camZ = 40;
int startX, startY, tracking = 0;

int alpha = 0, beta = 45, r = 50;

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45,ratio,1,1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}



void drawTerrain() {

    // colocar aqui o código de desnho do terreno usando VBOs com TRIANGLE_STRIPS
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);

	glVertexPointer(3, GL_FLOAT, 0, 0);

	for (int i = 0; i < imageHeight - 1 /* (ou strips) */; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, (imageWitdth) * 2 * i, (imageWitdth) * 2);
	}
}



void renderScene(void) {

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	glColor3ub(0, 255, 0);
	drawTerrain();

	// just so that it renders something before the terrain is built
	// to erase when the terrain is ready
	glTranslated(0, 1.5f, 0);
	glColor3ub(255, 0, 0);
	glutSolidTeapot(2.0);

// End of frame
	glutSwapBuffers();
}



void processKeys(unsigned char key, int xx, int yy) {

// put code to process regular keys in here
}



void processMouseButtons(int button, int state, int xx, int yy) {
	
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			
			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}


void processMouseMotion(int xx, int yy) {

	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux * 							     sin(betaAux * 3.14 / 180.0);
}


void init() {

	unsigned int t, tw, th;
	unsigned char* imageData;

// 	Load the height map "terreno.jpg"
	ilGenImages(1, &t);
	ilBindImage(t);
	// terreno.jpg is the image containing our height map
	ilLoadImage((ILstring)"terreno.jpg");
	/* 
	convert the image to single channel per pixel
	with values ranging between 0 and 255
	*/
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

	/*
	important: check tw and th values
	both should be equal to 256
	if not there was an error loading the image
	most  likely the image could not be found
	*/
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	// imageData is a LINEAR array with the pixel values
	imageData = ilGetData();

	cout << tw << " " << th << endl;

// 	Build the vertex arrays

	float halftw = tw / 2; // variacao dos valores
	float halfth = th / 2; // variacao dos valores

	int strips = th - 1; // O numero de strips é a altura da imagem menos 1
	int verticeStrip = 2 * tw;

	vector<float> p;

	for (size_t h = 0; h < th; h++) {
		for (size_t w = 0; w < tw; w++) {

			p.push_back(w - halftw);
			p.push_back(imageData[th * h + w] * (60.0 / 255.0));
			p.push_back(h - halfth);

			p.push_back(w - halftw);
			p.push_back(imageData[th * (h + 1) + w] * (60.0 / 255.0));
			p.push_back(h + 1.0 - halfth);

		}
	}

	imageHeight = th, imageWitdth = tw;

	glGenBuffers(1, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * p.size(),
		p.data(),
		GL_STATIC_DRAW);

// 	OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);
}


int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,1000);
	glutCreateWindow("CG@DI-UM");
		

// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	///////
	glewInit();
	glEnableClientState(GL_VERTEX_ARRAY);
	ilInit();
	///////

	init();	

// enter GLUT's main cycle
	glutMainLoop();
	
	return 0;
}

