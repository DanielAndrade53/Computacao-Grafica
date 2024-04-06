#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

using namespace std;

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;
float camX, camY, camZ;

float slices = 100;

GLuint vertices = 1.0, verticeCount;

int timebase;
float frames;

void spherical2Cartesian() {

	camX = radius * cos(beta) * sin(alfa);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alfa);
}

void framerate() {
	char s[50];
	frames++;
	double time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		double fps = frames * 10000.0 / (time - timebase);
		timebase = time;
		frames = 0;
		sprintf(s, "FPS: %.2f", fps);
		glutSetWindowTitle(s);
	}
}

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

/*void prepareData() {

	// criar um vector com as coordenadas dos pontos
	vector <float> p;

	// primeiro ponto
	p.push_back(-1.0f);
	p.push_back(1.0f);
	p.push_back(0.0f);

	// primeiro ponto
	p.push_back(0.0f);
	p.push_back(0.0f);
	p.push_back(0.0f);

	// primeiro ponto
	p.push_back(1.0f);
	p.push_back(1.0f);
	p.push_back(0.0f);

	verticeCount = p.size() / 3;

	// criar o VBO
	glGenBuffers(1, &vertices);

	// copiar o vector para a mémoria grafica
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(
		GL_ARRAY_BUFFER, // tipo do buffer, só é relevante na altura do desenho
		sizeof(float) * p.size(), // tamanho do vector em bytes
		p.data(), // os dados do array associados ap vector
		GL_STATIC_DRAW); // indicativo da utilizacao (estatico e para desenho)


	// --------------------------------------------

	vector<unsigned int> i;

	glGenBuffers(1, &indices);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * i.size(),
		i.data(),
		GL_STATIC_DRAW);

	indexCount = i.size();
}*/


/*void generatePlane(float dimension, int divisions) {



	vector <float> d;

	float squareHeight = dimension / divisions;
	float halfDimension = dimension / 2.0f;

	for (int i = 0; i < divisions; i++) {
		for (int j = 0; j < divisions; j++) {
			float x = i * squareHeight - halfDimension;
			float z = j * squareHeight - halfDimension;

			// Primeiro Triangulo
			d.push_back(x);
			d.push_back(0.0f);
			d.push_back(z);

			d.push_back(x);
			d.push_back(0.0f);
			d.push_back(z + squareHeight);

			d.push_back(x + squareHeight);
			d.push_back(0.0f);
			d.push_back(z + squareHeight);

			// Segundo Triangulo
			d.push_back(x + squareHeight);
			d.push_back(0.0f);
			d.push_back(z);

			d.push_back(x + squareHeight);
			d.push_back(0.0f);
			d.push_back(z + squareHeight);

			d.push_back(x);
			d.push_back(0.0f);
			d.push_back(z + squareHeight);
		}
	}

	verticeCount = d.size() / 3;

	glGenBuffers(1, &vertices);

	// copiar o vector para a mémoria grafica
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(
		GL_ARRAY_BUFFER, // tipo do buffer, só é relevante na altura do desenho
		sizeof(float) * d.size(), // tamanho do vector em bytes
		d.data(), // os dados do array associados ap vector
		GL_STATIC_DRAW); // indicativo da utilizacao (estatico e para desenho)
}*/



/*-----------------------------------------------------------------------------------
	Draw Cylinder (using vector)

		parameters: radius, height, sides

-----------------------------------------------------------------------------------*/
void drawCylinder(float baseRadius, float height, float sides) {

	vector <float> p;

	float halfHeight = height / 2;

	for (int i = 0; i < sides; ++i) {
		float alpha1 = 2.0f * M_PI / sides * i;
		float alpha2 = 2.0f * M_PI / sides * (i + 1);

		// Bottom triangle

		p.push_back(0.0f);
		p.push_back(-halfHeight);
		p.push_back(0.0f);
		
		p.push_back(baseRadius * cos(alpha1));
		p.push_back(-halfHeight);
		p.push_back(baseRadius * sin(alpha1));

		p.push_back(baseRadius * cos(alpha2));
		p.push_back(-halfHeight);
		p.push_back(baseRadius * sin(alpha2));

		// Top triangle

		p.push_back(0.0f);
		p.push_back(halfHeight);
		p.push_back(0.0f);

		p.push_back(baseRadius * cos(alpha2));
		p.push_back(halfHeight);
		p.push_back(baseRadius * sin(alpha2));

		p.push_back(baseRadius * cos(alpha1));
		p.push_back(halfHeight);
		p.push_back(baseRadius * sin(alpha1));

		// Side triangles

		// primeiro triangulo
		p.push_back(baseRadius * cos(alpha1));
		p.push_back(-halfHeight);
		p.push_back(baseRadius * sin(alpha1));

		p.push_back(baseRadius * cos(alpha1));
		p.push_back(halfHeight);
		p.push_back(baseRadius * sin(alpha1));

		p.push_back(baseRadius * cos(alpha2));
		p.push_back(-halfHeight);
		p.push_back(baseRadius * sin(alpha2));

		// segundo triangulo 

		p.push_back(baseRadius * cos(alpha2));
		p.push_back(-halfHeight);
		p.push_back(baseRadius * sin(alpha2));

		p.push_back(baseRadius * cos(alpha1));
		p.push_back(halfHeight);
		p.push_back(baseRadius * sin(alpha1));

		p.push_back(baseRadius * cos(alpha2));
		p.push_back(halfHeight);
		p.push_back(baseRadius * sin(alpha2));

	}

	verticeCount = p.size() / 3;

	glGenBuffers(1, &vertices);

	// copiar o vector para a mémoria grafica
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(
		GL_ARRAY_BUFFER, // tipo do buffer, só é relevante na altura do desenho
		sizeof(float) * p.size(), // tamanho do vector em bytes
		p.data(), // os dados do array associados ap vector
		GL_STATIC_DRAW); // indicativo da utilizacao (estatico e para desenho)

}




/*----------------------------------------------------------------------------------- 
	Draw Cylinder with strips and fans

	  parameters: radius, height, sides

-----------------------------------------------------------------------------------*/


void cylinder0(float radius, float height, int sides) {

	int i;
	float step;

	step = 360.0/sides;

	glColor3f(1,0,0);
	glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0,height*0.5,0);
		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius,height*0.5,-sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();

	glColor3f(0,1,0);
	glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0,-height*0.5,0);
		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius,-height*0.5,sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();

	glColor3f(0,0,1);
	glBegin(GL_TRIANGLE_STRIP);

		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius, height*0.5,-sin(i * step *M_PI/180.0)*radius);
			glVertex3f(cos(i * step * M_PI/180.0)*radius,-height*0.5,-sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();
}


/*-----------------------------------------------------------------------------------
	Draw Cylinder

		parameters: radius, height, sides

-----------------------------------------------------------------------------------*/


void cylinder(float radius, float height, int sides) {

	int i;
	float step;

	step = 360.0 / sides;

	glBegin(GL_TRIANGLES);

	// top
	for (i = 0; i < sides; i++) {
		glVertex3f(0, height*0.5, 0);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i+1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
	}

	// bottom
	for (i = 0; i < sides; i++) {
		glVertex3f(0, -height*0.5, 0);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, -height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
	}

	// body
	for (i = 0; i <= sides; i++) {

		glVertex3f(cos(i * step * M_PI / 180.0)*radius, height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);

		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, -height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
	}
	glEnd();
}


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
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

	cylinder0(1,2,10);
	//cylinder(1, 2, 10);


	// guiao
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, verticeCount);


	// time
	framerate();

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char key, int x, int y) {

	switch (key) {
	case '+':
		if (slices == 0) slices = 1.0f;
		slices = (int)slices * 2;
		drawCylinder(1, 2, slices);
		printf("slices = %f\n", slices);
		break;
	case '-':
		slices = (int)slices / 2;
		drawCylinder(1, 2, slices);
		printf("slices = %f\n", slices);
		break;
	}

	glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		alfa -= 0.1; break;

	case GLUT_KEY_LEFT:
		alfa += 0.1; break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;
		break;

	case GLUT_KEY_PAGE_UP: 
		radius += 0.1f; 
		break;
	}
	spherical2Cartesian();
	glutPostRedisplay();

}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Page Up and Page Down control the distance from the camera to the origin\n");
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

	// init GLEW
	glewInit();

	glEnableClientState(GL_VERTEX_ARRAY);


//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);

	// Prepare data before entering the main loop
	// prepareData();

	spherical2Cartesian();

	printInfo();

	// generatePlane(1, 3);
	// drawCylinder(1, 2, slices);

// enter GLUT's main cycle
	timebase = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();
	
	return 1;
}