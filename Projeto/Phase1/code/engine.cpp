#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <cstring>
#include <tuple>
#include <vector>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;
using namespace std;

XMLDocument doc;
const char* filename = "creator.xml";

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//path tinyxml2 -> C:\Users\danie\Desktop\Uni\CG\Computacao - Grafica\VCPKG\vcpkg\installed\x86 - windows\include\tinyxml2.h

vector<float> vertices;
float verticesNumber = 0.0f;

float alpha = 0.0f, beta = 0.0f, radius = 7.0f;
float camRX =  0.0f, camRY = 0.0f, camRZ = 5.0f;
float camX = 0.0f, camY = 0.0f, camZ = 5.0f;
float lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;
float upX = 0.01f, upY = 1.0f, upZ = 0.0f;

float fovy = 45.0f, zNear = 1.0f, zFar = 1000.0f;

GLenum mode = GL_FILL;

void spherical2Cartesian() {
    camRX = radius * cos(beta) * sin(alpha);
    camRY = radius * sin(beta);
    camRZ = radius * cos(beta) * cos(alpha);
}

void read3dFile(const char* figureName) {

    string line;

    stringstream filePath;
    filePath << "../file3d/" << figureName;

    ifstream file(filePath.str());

    if (file.is_open()) {
        if (getline(file, line)) {
            verticesNumber = stoi(line);
        }
        while (getline(file, line)) {
            stringstream ss(line);
            float x, y, z;
            char comma;
            if (ss >> x >> comma >> y >> comma >> z) {
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }
        file.close();
    }
}

void readXMLFile() {

    XMLError eResult = doc.LoadFile(filename);
    if (eResult != XML_SUCCESS) {
        cerr << "Nao foi possivel abrir o ficheiro XML: " << filename << endl;
        return;
    }

    XMLElement* worldElement = doc.FirstChildElement("World");

    if (worldElement) {

        XMLElement* windowElement = worldElement->FirstChildElement("window");

        XMLElement* cameraElement = worldElement->FirstChildElement("camera");

        if (cameraElement) {
            // Extract position
            XMLElement* positionElement = cameraElement->FirstChildElement("position");
            if (positionElement) {
                camX = atof(positionElement->Attribute("x"));
                camY = atof(positionElement->Attribute("y"));
                camZ = atof(positionElement->Attribute("z"));
            }

            // Extract lookAt
            XMLElement* lookAtElement = cameraElement->FirstChildElement("lookAt");
            if (lookAtElement) {
                lookAtX = atof(lookAtElement->Attribute("x"));
                lookAtY = atof(lookAtElement->Attribute("y"));
                lookAtZ = atof(lookAtElement->Attribute("z"));
            }

            // Extract up
            XMLElement* upElement = cameraElement->FirstChildElement("up");
            if (upElement) {
                upX = atof(upElement->Attribute("x"));
                upY = atof(upElement->Attribute("y"));
                upZ = atof(upElement->Attribute("z"));
            }

            // Extract projection
            XMLElement* projectionElement = cameraElement->FirstChildElement("projection");
            if (projectionElement) {
                fovy = atof(projectionElement->Attribute("fov"));
                zNear = atof(projectionElement->Attribute("near"));
                zFar = atof(projectionElement->Attribute("far"));
            }
        }

        XMLElement* groupElement = worldElement->FirstChildElement("group");

        if (groupElement) {

            XMLElement* modelsElement = groupElement->FirstChildElement("models");

            if (modelsElement) {
                XMLElement* existingModel = modelsElement->FirstChildElement("model");

                while (existingModel) {
                    const char* figureName = existingModel->Attribute("file");
                    if (figureName) {
                        read3dFile(figureName);
                    }
                    existingModel = existingModel->NextSiblingElement("model");
                }
            }
        }
    }

}

// Funcaoo para lidar com a redimensionamento da janela
void changeSize(int w, int h) {

    // Prevent a divide by zero when window is too short
    // (you cant make a window with zero width)
    if (h == 0) h = 1;

    // Compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(fovy, ratio, zNear, zFar);

    // Return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

// Function to render the scene
void renderScene(void) {
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the camera
    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
              lookAtX, lookAtY, lookAtZ, 
              upX, upY, upZ);

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
    glColor3f(1.0f, 1.0f, 1.0f);  // cor da primitiva

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();

    // Fim do quadro
    glutSwapBuffers();
}

// write function to process keyboard events
void processKeys(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        mode = GL_FILL;
        break;
    case '2':
        mode = GL_LINE;
        glLineWidth(1.0f);
        break;
    case '3':
        mode = GL_POINT;
        glPointSize(7.0f);
        break;
    }
    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		alpha -= 0.1; 
        camX = camRX;
        camY = camRY;
        camZ = camRZ;
        break;

	case GLUT_KEY_LEFT:
		alpha += 0.1; 
        camX = camRX;
        camY = camRY;
        camZ = camRZ;
        break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
        camX = camRX;
        camY = camRY;
        camZ = camRZ;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
        camX = camRX;
        camY = camRY;
        camZ = camRZ;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;
        camX = camRX;
        camY = camRY;
        camZ = camRZ;
		break;

	case GLUT_KEY_PAGE_UP: 
		radius += 0.1f; 
        camX = camRX;
        camY = camRY;
        camZ = camRZ;
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


int main(int argc, char** argv) {

    readXMLFile();

    // Initialize GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@DI-UM-GRUPO14");

    srand((time(NULL)));

    // Register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    // put here the registration of the keyboard callbacks
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    spherical2Cartesian();

    printInfo();

    // Enter GLUT's main loop
    glutMainLoop();

    return 1;
}