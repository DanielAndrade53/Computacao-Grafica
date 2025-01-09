#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdlib.h>
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
#include <GL/glew.h>
#include <GL/glut.h>
#endif


float verticesNumber = 0.0f;

float camCoords[3] = { 300.0f, 300.0f, 300.0f };
float lookAtCoords[3] = { 0.0f, 0.0f, 0.0f };
float upCoords[3] = { 0.0f, 1.0f, 0.0f };

float fovy = 60.0f, zNear = 1.0f, zFar = 5000.0f;

float alpha = 0, beta = 35, radius = 50;
int startX, startY, tracking = 0;

GLuint vertexCount, vertices, normals;
int vertex = 0;
int timebase = 0, frame = 0;

bool axisBoolean = true;
bool showOrbit = true;
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
        cout << "Translation: (" << x << ", " << y << ", " << z << ")" << endl;
    }
};

class RotationByAngle : public Transformation {
    float angle, x, y, z;

public:

    RotationByAngle() {
        this->angle = 0;
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    RotationByAngle(float ang, float cx, float cy, float cz) {
        this->angle = ang;
        this->x = cx;
        this->y = cy;
        this->z = cz;
    }

    void apply() {
        glRotatef(this->angle, this->x, this->y, this->z);
    }

    void print() const {
        cout << "RotA: (" << angle << ", " << x << ", " << y << ", " << z << ")" << endl;
    }
};

class RotationByTime : public Transformation {
    float time, x, y, z;

public:

    RotationByTime() {
        this->time = 0;
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    RotationByTime(float tim, float cx, float cy, float cz) {
        // mili
        this->time = tim * 1000.0f;
        // secomds
        // this->time = tim;
        this->x = cx;
        this->y = cy;
        this->z = cz;
    }

    void apply() {
        // mili
        float angle = glutGet(GLUT_ELAPSED_TIME) * 360 / this->time;
        // seconds
        // float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * 360 / this->time;
        glRotatef(angle, this->x, this->y, this->z);
    }

    void print() const {
        cout << "RotT: (" << time << "," << x << ", " << y << ", " << z << ")" << endl;
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
        cout << "Scale: (" << x << ", " << y << ", " << z << ")" << endl;
    }
};

void buildRotMatrix(float* x, float* y, float* z, float* m) {

    m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
    m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
    m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void cross(float* a, float* b, float* res) {

    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(float* a) {

    float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0] / l;
    a[1] = a[1] / l;
    a[2] = a[2] / l;
}

float length(float* v) {

    float res = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    return res;

}

void multMatrixVector(float* m, float* v, float* res) {

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }

}

void getCatmullRomPoint(float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv) {

    // catmull-rom matrix
    float catmull[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
                           { 1.0f, -2.5f,  2.0f, -0.5f},
                           {-0.5f,  0.0f,  0.5f,  0.0f},
                           { 0.0f,  1.0f,  0.0f,  0.0f} };

    float tt[4] = { t * t * t, t * t, t, 1 }; // = { pow(t,3), pow(t,2), t, 1 }
    float tt_deriv[4] = { 3 * t * t, 2 * t, 1, 0 }; // = { 3 * pow(t,2), 2 * t, 1, 0 }


    for (int i = 0; i < 3; i++) { // x = 0, y = 1 e z = 2

        float p[4] = { p0[i], p1[i], p2[i], p3[i] };
        float a[4];
        // Compute A = M * P
        multMatrixVector(*catmull, p, a);

        // Compute pos = T * A
        pos[i] = tt[0] * a[0] + tt[1] * a[1] + tt[2] * a[2] + tt[3] * a[3];
        deriv[i] = tt_deriv[0] * a[0] + tt_deriv[1] * a[1] + a[2];
        // ...
    }

}

void convertPoints(Point p, float pont[3]) {
    pont[0] = p.getX();
    pont[1] = p.getY();
    pont[2] = p.getZ();
}

// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv, vector<Point> p) {

    int POINT_COUNT = p.size();
    float t = gt * POINT_COUNT; // this is the real global t
    int index = floor(t);  // which segment
    t = t - index; // where within  the segment

    // indices store the points
    int indices[4];
    indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
    indices[1] = (indices[0] + 1) % POINT_COUNT;
    indices[2] = (indices[1] + 1) % POINT_COUNT;
    indices[3] = (indices[2] + 1) % POINT_COUNT;

    float p0[3], p1[3], p2[3], p3[3];
    convertPoints(p[indices[0]], p0); // p[indices[0]] = p0
    convertPoints(p[indices[1]], p1); // p[indices[1]] = p1
    convertPoints(p[indices[2]], p2); // p[indices[2]] = p2
    convertPoints(p[indices[3]], p3); // p[indices[3]] = p3

    getCatmullRomPoint(t, p0, p1, p2, p3, pos, deriv);
}

void renderCatmullRomCurve(vector<Point> cPonts) {
    // draw curve using line segments with GL_LINE_LOOP
    float pos[3], deriv[3];

    float t_step = 1.0 / (float)100;

    if (showOrbit) {
        glColor3f(0.3f, 0.3f, 0.3f);
    }
    else {
        glColor3f(0.0f, 0.0f, 0.0f);
    }
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {

        float my_t = i * t_step;
        getGlobalCatmullRomPoint(my_t, pos, deriv, cPonts);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
}

class Curve : public Transformation {

    float time, t, currentTime;
    bool align;
    vector<Point> cPoints;
    float prev_y[3];

public:

    Curve() {
        this->time = 0;
        this->t = 0;
        this->currentTime = 0;
        this->align = true;
        this->cPoints = vector<Point>();
        this->prev_y[0] = 0;
        this->prev_y[1] = 1;
        this->prev_y[2] = 0;
    }

    Curve(float ti, bool ali, vector<Point> cp) {
        this->time = ti * 1000;
        this->t = 0;
        this->currentTime = 0;
        this->align = ali;
        this->cPoints = cp;
        this->prev_y[0] = 0;
        this->prev_y[1] = 1;
        this->prev_y[2] = 0;
    }

    float getTime() const {
        return this->time;
    }

    void setTime(float duration) {
        this->time = duration;
    }

    void apply() {

        float pos[3], deriv[3];

        renderCatmullRomCurve(this->cPoints);
        getGlobalCatmullRomPoint(this->t, pos, deriv, this->cPoints);
        glTranslatef(pos[0], pos[1], pos[2]);

        if (this->align) {
            float x[3] = { deriv[0], deriv[1], deriv[2] };

            normalize(x);
            float z[3];
            cross(x, prev_y, z);
            normalize(z);
            float y[3];
            cross(z, x, y);
            normalize(y);
            memcpy(prev_y, y, 3 * sizeof(float));

            float m[16];

            buildRotMatrix(x, y, z, m);

            glMultMatrixf(m);
        }

        float new_time = glutGet(GLUT_ELAPSED_TIME);
        float diff = new_time - this->currentTime;

        this->t += diff / this->time;
        this->currentTime = new_time;
    }

    void print() const {
        cout << "Curve: time= " << time << ", align= " << align << endl;
    }
};

class Model {

    vector<Point> points;
    vector<Transformation*> transformations;

public:
    Model() {
        this->points = vector<Point>();
        this->transformations = vector<Transformation*>();
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

vector<Point> read3dFile(string figureName) {

    string line;
    vector<Point> model;

    stringstream filePath;
    filePath << "../file3d/" << figureName;

    ifstream file(filePath.str());

    if (file.is_open()) {
        if (getline(file, line)) {
            verticesNumber = stoi(line);
            // printf("Vertices= %f\n", verticesNumber);
        }
        while (getline(file, line)) {
            stringstream ss(line);
            float x, y, z;
            char comma;
            if (ss >> x >> comma >> y >> comma >> z) {
                // printf("x=%f, y=%f, z=%f\n", x, y, z);
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

                string transfName = string(ts->Name());

                if (!strcmp(transfName.c_str(), "translate")) {
                    const XMLAttribute* timeAttr = ts->FindAttribute("time");
                    if (timeAttr == nullptr) {
                        // No time attribute, treat as translation
                        float x, y, z;
                        x = atof(ts->FindAttribute("x")->Value());
                        y = atof(ts->FindAttribute("y")->Value());
                        z = atof(ts->FindAttribute("z")->Value());
                        tr.push_back(new Translation(x, y, z));
                    }
                    else {
                        float time = atof(timeAttr->Value());
                        const char* align = ts->FindAttribute("align")->Value();

                        vector<Point> curve;
                        XMLElement* pt = ts->FirstChildElement("point");
                        while (pt) {
                            float x = atof(pt->FindAttribute("x")->Value());
                            float y = atof(pt->FindAttribute("y")->Value());
                            float z = atof(pt->FindAttribute("z")->Value());

                            curve.push_back(Point(x, y, z));
                            pt = pt->NextSiblingElement("point");
                        }

                        tr.push_back(new Curve(time, align == "True", curve));
                    }
                }


                if (!strcmp(transfName.c_str(), "rotate")) {
                    const XMLAttribute* timeAttr = ts->FindAttribute("time");
                    float angle, x, y, z, time;
                    x = atof(ts->FindAttribute("x")->Value());
                    y = atof(ts->FindAttribute("y")->Value());
                    z = atof(ts->FindAttribute("z")->Value());
                    if (timeAttr == nullptr) {
                        angle = atof(ts->FindAttribute("angle")->Value());
                        tr.push_back(new RotationByAngle(angle, x, y, z));
                    }
                    else {
                        time = atof(ts->FindAttribute("time")->Value());
                        tr.push_back(new RotationByTime(time, x, y, z));
                    }
                }

                if (!strcmp(transfName.c_str(), "scale")) {
                    float x, y, z;
                    x = atof(ts->FindAttribute("x")->Value());
                    y = atof(ts->FindAttribute("y")->Value());
                    z = atof(ts->FindAttribute("z")->Value());
                    tr.push_back(new Scale(x, y, z));

                }

                for (int k = 0; k < tr.size(); k++) {
                    tr[k]->print();
                }

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


        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, (model.getPoints().size() * 2) * sizeof(Point), nullptr, GL_STATIC_DRAW);

        /* glColor3ub(rand() % 256, rand() % 256, rand() % 256); */
        /* glColor3ub(0, 256, 0); */

        glPushMatrix();
        for (Transformation* t : model.getTransformations()) {
            t->apply();
        }

        /*for (Point p : model.getPoints()) {

            glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(Point) * 2, sizeof(Point), &model.getPoints());


        }
        glEnd();*/

        for (size_t i = 0; i < model.getPoints().size(); ++i) {
            glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(Point) * 2, sizeof(Point), &model.getPoints()[i]);
            Point c = { 0,1,0 };
            glBufferSubData(GL_ARRAY_BUFFER, (i * sizeof(Point) * 2) + sizeof(Point), sizeof(Point), &c);
        }

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(Point) * 2, nullptr);
        glColorPointer(3, GL_FLOAT, sizeof(Point) * 2, (void*)(sizeof(Point)));

        glPushMatrix();

        glDrawArrays(GL_TRIANGLES, 0, model.getPoints().size());

        glPopMatrix();
        glDeleteBuffers(1, &vertexBuffer);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

    }

    glDisableClientState(GL_VERTEX_ARRAY);

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

    int fps;
    char s[64];

    // Clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set lights

    // Set the camera
    glLoadIdentity();
    gluLookAt(camCoords[0], camCoords[1], camCoords[2],
        lookAtCoords[0], lookAtCoords[1], lookAtCoords[2],
        upCoords[0], upCoords[1], upCoords[2]);


    if (axisBoolean) drawAxis();
    drawModel();

    glPolygonMode(GL_FRONT_AND_BACK, mode);

    frame++;
    double time = glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        double fps = frame * 10000.0 / (time - timebase);
        timebase = time;
        frame = 0;
        sprintf_s(s, "FPS: %6.2f", fps);
        glutSetWindowTitle(s);
    }


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
        axisBoolean = !axisBoolean;
        break;
    case '5':
        showOrbit = !showOrbit;
        break;
        /* case f1 diminuir velocidade rotacao
           case f2 parar rotacao
           case f3 aumentar velocidade rotacao
        */
    }
    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) {

    switch (key) {

    case GLUT_KEY_F1:
        // diminuir velocidade rotacao
        for (Model& model : models) {
            for (Transformation* t : model.getTransformations()) {
                Curve* cur = dynamic_cast<Curve*>(t);
                if (cur && cur->getTime() < 50000) {
                    cur->setTime(cur->getTime() * 2); // Increase time (decrease speed)
                    printf("time= %f\n", cur->getTime());
                }
            }
        }
        break;
    case GLUT_KEY_F2:
        // aumentar velocidade rotacao
        for (Model& model : models) {
            for (Transformation* t : model.getTransformations()) {
                Curve* cur = dynamic_cast<Curve*>(t);
                if (cur && cur->getTime() > 700) {
                    cur->setTime(cur->getTime() * 0.5); // Increase time (decrease speed)
                    printf("time= %f\n", cur->getTime());
                }
            }
        }
        break;
    }
}

void processMouseButtons(int button, int state, int xx, int yy) {
    if (state == GLUT_DOWN) {
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
            radius -= yy - startY;
            if (radius < 3) radius = 3.0;
        }
        tracking = 0;
    }
}

void processMouseMotion(int xx, int yy) {
    int deltaX, deltaY;
    int alphaAux, betaAux;
    int rAux;

    if (!tracking) return;

    deltaX = xx - startX;
    deltaY = yy - startY;

    if (tracking == 1) {
        alphaAux = alpha + deltaX;
        betaAux = beta + deltaY;

        if (betaAux > 85.0)
            betaAux = 85.0;
        else if (betaAux < -85.0)
            betaAux = -85.0;

        rAux = radius;
    }
    else if (tracking == 2) {
        alphaAux = alpha;
        betaAux = beta;
        rAux = radius - deltaY;
        if (rAux < 3) rAux = 3;
    }
    camCoords[0] = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    camCoords[1] = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    camCoords[2] = rAux * sin(betaAux * 3.14 / 180.0);
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

    glewInit();

    // Register callbacks
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    // put here the registration of the keyboard callbacks
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    printInfo();

    // Enter GLUT's main loop
    glutMainLoop();

    return 0;
}