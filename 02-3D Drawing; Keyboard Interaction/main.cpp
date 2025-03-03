#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
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
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

float pyramidX = 0.0f;
float pyramidY = 0.0f;
float pyramidZ = 0.0f;
float rotateCam = 0.0f;

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(10.0f, 10.0f, 10.0f,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	// put axis drawing in here
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


	// put the geometric transformations here
	glTranslatef(1.0f, 0.0f, 0.0f);
	glRotatef(rotateCam, 0.0f, 1.0f , 0.0f);

	// put pyramid drawing instructions here
	glBegin(GL_TRIANGLES);
	// Front face
	glColor3f(1.0f, 0.0f, 0.0f); // Red
	glVertex3f(0.0f + pyramidX, 1.0f + pyramidY, 0.0f + pyramidZ);
	glVertex3f(-1.0f + pyramidX, -1.0f + pyramidY, 1.0f + pyramidZ);
	glVertex3f(1.0f + pyramidX, -1.0f + pyramidY, 1.0f + pyramidZ);
	// Right face
	glColor3f(0.0f, 1.0f, 0.0f); // Green
	glVertex3f(0.0f + pyramidX, 1.0f + pyramidY, 0.0f + pyramidZ);
	glVertex3f(1.0f + pyramidX, -1.0f + pyramidY, 1.0f + pyramidZ);
	glVertex3f(1.0f + pyramidX, -1.0f + pyramidY, -1.0f + pyramidZ);
	// Back face
	glColor3f(0.0f, 0.0f, 1.0f); // Blue
	glVertex3f(0.0f + pyramidX, 1.0f + pyramidY, 0.0f + pyramidZ);
	glVertex3f(1.0f + pyramidX, -1.0f + pyramidY, -1.0f + pyramidZ);
	glVertex3f(-1.0f + pyramidX, -1.0f + pyramidY, -1.0f + pyramidZ);
	// Left face
	glColor3f(1.0f, 1.0f, 0.0f); // Yellow
	glVertex3f(0.0f + pyramidX, 1.0f + pyramidY, 0.0f + pyramidZ);
	glVertex3f(-1.0f + pyramidX, -1.0f + pyramidY, -1.0f + pyramidZ);
	glVertex3f(-1.0f + pyramidX, -1.0f + pyramidY, 1.0f + pyramidZ);
	glEnd();


	// End of frame
	glutSwapBuffers();
}

// write function to process keyboard events
void keyboardEvents(unsigned char key, int x, int y) {
	switch (key) {
		case 'q':
			pyramidX += 1.0f;
			break;
		case 'a':
			pyramidX -= 1.0f;
			break;
		case 'w':
			pyramidY += 1.0f;
			break;
		case 's':
			pyramidY -= 1.0f;
			break;
		case 'e':
			pyramidZ += 1.0f;
			break;
		case 'd':
			pyramidZ -= 1.0f;
			break;
		case 'r':
			rotateCam += 1.0f;
			break;
	}
	glutPostRedisplay();
}


int main(int argc, char** argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keyboardEvents);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}