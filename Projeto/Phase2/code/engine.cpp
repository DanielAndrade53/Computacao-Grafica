#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <tuple>
#include <vector>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;
using namespace std;

const char* filename = "creator.xml";

XMLDocument doc;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

float verticesNumber = 0.0f;

float camCoords[3] = { 0.0f, 30.0f, 40.0f };
float lookAtCoords[3] = { 0.0f, 0.0f, 0.0f };
float upCoords[3] = { 0.0f, 1.0f, 0.0f };

float fovy = 45.0f, zNear = 1.0f, zFar = 1000.0f;

bool axisBoolean = true;

float alpha = 0, beta = 35, radius = 50;

GLenum mode = GL_FILL;

class Point {

    float x, y, z;

public:

    Point() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Point(float cx, float cy, float cz) {
        this->x = cx;
        this->y = cy;
        this->z = cz;
    }

    float getX() const {
        return this->x;
    }

    float getY() const {
        return this->y;
    }

    float getZ() const {
        return this->z;
    }

    // Setters
    void setX(float cx) {
        this->x = cx;
    }

    void setY(float cy) {
        this->y = cy;
    }

    void setZ(float cz) {
        this->z = cz;
    }
};

class Transformation {
public:
    virtual void apply() = 0;
    virtual void print() const = 0; 

};

class Translation : public Transformation {
    float x, y, z;

public:

    Translation() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Translation(float cx, float cy, float cz) {
        this->x = cx;
        this->y = cy;
        this->z = cz;
    }

    void apply() {
        glTranslatef(this->x, this->y, this->z);
    }

    void print() const {
        std::cout << "Translation: (" << x << ", " << y << ", " << z << ")" << std::endl;
    }
};

class Rotation : public Transformation {
    float angle, x, y, z;

public:

    Rotation() {
        this->angle = 0;
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Rotation(float ang, float cx, float cy, float cz) {
        this->angle = ang;
        this->x = cx;
        this->y = cy;
        this->z = cz;
    }

    void apply() {
        glRotatef(this->angle, this->x, this->y, this->z);
    }

    void print() const {
        std::cout << "Rot: (" << angle << ", " << x << ", " << y << ", " << z << ")" << std::endl;
    }
};

class Scale : public Transformation {
    float x, y, z;

public:

    Scale() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Scale(float cx, float cy, float cz) {
        this->x = cx;
        this->y = cy;
        this->z = cz;
    }

    void apply() {
        glScalef(this->x, this->y, this->z);
    }

    void print() const {
        std::cout << "Scale: (" << x << ", " << y << ", " << z << ")" << std::endl;
    }
};

class Model {

    vector<Point> points;
    vector<Transformation*> transformations;

public:
    Model() {
        points = vector<Point>();
        transformations = vector<Transformation*>();
    }

    Model(vector<Point> p, vector<Transformation*> tr) {
        this->points = p;
        this->transformations = tr;
    }

    const vector<Point>& getPoints() const {
        return this->points;
    }

    const vector<Transformation*>& getTransformations() const {
        return this->transformations;
    }
};

vector<Model> models;

void spherical2Cartesian() {

    camCoords[0] = radius * cos(beta) * sin(alpha);
    camCoords[1] = radius * sin(beta);
    camCoords[2] = radius * cos(beta) * cos(alpha);
}

vector<Point> read3dFile(string figureName) {

    string line;
    vector<Point> model;

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
                Point p1(x, y, z);
                model.push_back(p1);
            }
        }
    }
    file.close();
    return model;
}

void readXMLFileGroup(XMLElement* group, vector<Transformation*> tr) {

    vector<Transformation*> originalTransformations = tr;

    while (group) {

        printf("oladknia\n");

        tr = originalTransformations;

        XMLElement* transformElement = group->FirstChildElement("transform");

        if (transformElement) {
            XMLElement* ts = transformElement->FirstChildElement();
            while (ts) {

                std::string transfName = std::string(ts->Name());

                if (!strcmp(transfName.c_str(), "translate")) {
                    float x, y, z;
                    x = atof(ts->FindAttribute("x")->Value());
                    y = atof(ts->FindAttribute("y")->Value());
                    z = atof(ts->FindAttribute("z")->Value());
                    tr.push_back(new Translation(x, y, z));
                }

                if (!strcmp(transfName.c_str(), "rotate")) {
                    float angle, x, y, z;
                    angle = atof(ts->FindAttribute("angle")->Value());
                    x = atof(ts->FindAttribute("x")->Value());
                    y = atof(ts->FindAttribute("y")->Value());
                    z = atof(ts->FindAttribute("z")->Value());
                    tr.push_back(new Rotation(angle, x, y, z));
                }

                if (!strcmp(transfName.c_str(), "scale")) {
                    float x, y, z;
                    x = atof(ts->FindAttribute("x")->Value());
                    y = atof(ts->FindAttribute("y")->Value());
                    z = atof(ts->FindAttribute("z")->Value());
                    tr.push_back(new Scale(x, y, z));

                }

                /*for (int k = 0; k < tr.size(); k++) {
                    tr[k]->print();
                }*/

                ts = ts->NextSiblingElement();
            }
        }
        XMLElement* modelsElement = group->FirstChildElement("models");

        if (modelsElement) {
            for (XMLElement* m = modelsElement->FirstChildElement("model"); m; m = m->NextSiblingElement()) {
                vector<Point> poin = read3dFile(m->Attribute("file"));
                models.push_back(Model(poin, tr));
            }
        }
        XMLElement* childGroup = group->FirstChildElement("group");
        readXMLFileGroup(childGroup, tr);
        group = group->NextSiblingElement("group");
    }
}

void readXMLFile() {

    XMLError eResult = doc.LoadFile(filename);
    if (eResult != XML_SUCCESS) {
        cerr << "Nao foi possivel abrir o ficheiro XML: " << filename << endl;
        return;
    }

    XMLElement* worldElement = doc.FirstChildElement("world");

    if (worldElement) {

        XMLElement* windowElement = worldElement->FirstChildElement("window");

        XMLElement* cameraElement = worldElement->FirstChildElement("camera");

        if (cameraElement) {

            // Extract position
            XMLElement* positionElement = cameraElement->FirstChildElement("position");
            if (positionElement) {
                camCoords[0] = atof(positionElement->Attribute("x"));
                camCoords[1] = atof(positionElement->Attribute("y"));
                camCoords[2] = atof(positionElement->Attribute("z"));
            }

            // Extract lookAt
            XMLElement* lookAtElement = cameraElement->FirstChildElement("lookAt");
            if (lookAtElement) {
                lookAtCoords[0] = atof(lookAtElement->Attribute("x"));
                lookAtCoords[1] = atof(lookAtElement->Attribute("y"));
                lookAtCoords[2] = atof(lookAtElement->Attribute("z"));
            }

            // Extract up
            XMLElement* upElement = cameraElement->FirstChildElement("up");
            if (upElement) {
                upCoords[0] = atof(upElement->Attribute("x"));
                upCoords[1] = atof(upElement->Attribute("y"));
                upCoords[2] = atof(upElement->Attribute("z"));
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

        vector<Transformation*> transf;

        readXMLFileGroup(groupElement, transf);
    }
}

void drawAxis() {

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

}

void drawModel() {


    for (Model model : models) {

        glColor3ub(rand() % 256, rand() % 256, rand() % 256);

        glPushMatrix();
        for (Transformation* t : model.getTransformations()) {
            t->apply();
        }

        glBegin(GL_TRIANGLES);
        for (Point p : model.getPoints()) {
            glVertex3f(p.getX(), p.getY(), p.getZ());
        }
        glEnd();
        glPopMatrix();

    }

}

void printInfo() {

    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));

    printf("\nUse Arrows to move the camera up/down and left/right\n");
    printf("Page Up and Page Down control the distance from the camera to the origin\n");
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
    gluLookAt(camCoords[0], camCoords[1], camCoords[2],
        lookAtCoords[0], lookAtCoords[1], lookAtCoords[2],
        upCoords[0], upCoords[1], upCoords[2]);

    if (axisBoolean) drawAxis();
    drawModel();

    glPolygonMode(GL_FRONT_AND_BACK, mode);



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
    case '4':
        axisBoolean = true;
        break;
    case '5':
        axisBoolean = false;
        break;
    }
    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) {

    switch (key) {

    case GLUT_KEY_RIGHT:
        alpha -= 0.1; break;

    case GLUT_KEY_LEFT:
        alpha += 0.1; break;

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

int main(int argc, char** argv) {

    readXMLFile();

    float dx = lookAtCoords[0] - camCoords[0];
    float dy = lookAtCoords[1] - camCoords[1];
    float dz = lookAtCoords[2] - camCoords[2];

    radius = sqrt(dx * dx + dy * dy + dz * dz);
    alpha = atan2(dy, dx);
    beta = atan2(sqrt(dx * dx + dy * dy), dz);

    printf("beta: %f\n", beta);

    // Initialize GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@DI-UM-GRUPO14");

    srand((time(NULL)));

    // Register callbacks
    glutDisplayFunc(renderScene);
    // glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    // put here the registration of the keyboard callbacks
    glutKeyboardFunc(processKeys);
    //glutMouseFunc(processMouseButtons);
    glutSpecialFunc(processSpecialKeys);
    //glutMotionFunc(processMouseMotion);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    spherical2Cartesian();

    printInfo();

    // Enter GLUT's main loop
    glutMainLoop();

    return 0;
}