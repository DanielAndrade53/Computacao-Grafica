#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

float rotateCamX = 0.0f;
float rotateCamY = 0.0f;
float rotateCamZ = 0.0f;

struct CoordPolar {
	float radius;
	float alpha;
	float beta;
};

CoordPolar camPos = { 7.0f, M_PI_4, M_PI_4 };

float polarX(CoordPolar polar) {
	return polar.radius * cos(polar.beta) * cos(polar.alpha);
}
float polarY(CoordPolar polar) {
	return polar.radius * sin(polar.beta);
}
float polarZ(CoordPolar polar) {
	return polar.radius * cos(polar.beta) * sin(polar.alpha);
}

GLenum mode = GL_FILL;


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

// put instructions to draw cylinder here

void drawCylinder(float baseRadius, float height, int slices) {
	float halfHeight = height / 2;

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < slices; ++i) {
		float alpha1 = 2.0f * M_PI / slices * i;
		float alpha2 = 2.0f * M_PI / slices * (i + 1);

		// Bottom triangle
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, -halfHeight, 0.0f);  // Centro do círculo no plano XY
		glVertex3f(baseRadius * cos(alpha1), -halfHeight, baseRadius * sin(alpha1));
		glVertex3f(baseRadius * cos(alpha2), -halfHeight, baseRadius * sin(alpha2));

		// Top triangle
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, halfHeight, 0.0f);  // Centro do círculo no plano XY
		glVertex3f(baseRadius * cos(alpha2), halfHeight, baseRadius * sin(alpha2));
		glVertex3f(baseRadius * cos(alpha1), halfHeight, baseRadius * sin(alpha1));

		// Side triangles
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(baseRadius * cos(alpha1), -halfHeight, baseRadius * sin(alpha1));
		glVertex3f(baseRadius * cos(alpha1), halfHeight, baseRadius * sin(alpha1));
		glVertex3f(baseRadius * cos(alpha2), -halfHeight, baseRadius * sin(alpha2));
		
		glVertex3f(baseRadius * cos(alpha2), -halfHeight, baseRadius * sin(alpha2));
		glVertex3f(baseRadius * cos(alpha1), halfHeight, baseRadius * sin(alpha1));
		glVertex3f(baseRadius * cos(alpha2), halfHeight, baseRadius * sin(alpha2));

	}
	glEnd();
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(polarX(camPos), polarY(camPos), polarZ(camPos),
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	// Draw axes
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, mode);
	glColor3f(0.5f, 0.5f, 0.5f);

	// put the geometric transformations here
	glRotatef(rotateCamX, 0.0f, 1.0f, 0.0f);
	glRotatef(rotateCamY, 0.0f, 0.0f, 1.0f);
	glRotatef(rotateCamZ, 1.0f, 0.0f, 0.0f);


	drawCylinder(1,2,10);

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char key, int xx, int yy) {

// put code to process regular keys in here
	switch (key) {
	case 'q':
		rotateCamY += 1.0f;
		break;
	case 'w':
		rotateCamY -= 1.0f;
		break;
	case 'a':
		rotateCamX += 1.0f;
		break;
	case 's':
		rotateCamX -= 1.0f;
		break;
	case 'z':
		rotateCamZ += 1.0f;
		break;
	case 'x':
		rotateCamZ -= 1.0f;
		break;
	case '1':
		mode = GL_FILL;
		break;
	case '2':
		mode = GL_LINE;
		glLineWidth(2.0f);
		break;
	case '3':
		mode = GL_POINT;
		glPointSize(7.0f);
		break;
	}
	glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {

// put code to process special keys in here
	switch (key) {
	case GLUT_KEY_UP:
		camPos.beta += M_PI / 16;
		break;
	case GLUT_KEY_DOWN:
		camPos.beta -= M_PI / 16;
		break;
	case GLUT_KEY_LEFT:
		camPos.alpha -= M_PI / 16;
		break;
	case GLUT_KEY_RIGHT:
		camPos.alpha += M_PI / 16;
		break;
	}

	/*if (camPos.alpha < 0) camPos.alpha += M_PI * 2;
	else if (camPos.alpha > M_PI * 2) camPos.alpha -= M_PI * 2;

	if (camPos.beta < -M_PI) camPos.beta += M_PI * 2;
	else if (camPos.beta > M_PI) camPos.beta -= M_PI * 2;*/

	glutPostRedisplay();
}


int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	
// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
