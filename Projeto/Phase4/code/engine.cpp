#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <time.h>
#include <IL/il.h> 

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


float camX = -0.740200, camY = 1.182951, camZ = 1.376347;
float lookAtX = 0.0f, lookAtY = .0f, lookAtZ = .0f;
float upX = .0f, upY = 1.0f, upZ = 0.0f;
float fovy = 60, zNear = 1, zFar = 4000;
float alpha = 45.0f, beta = 45.0f, radius = 3.0f;

int vertex = 0;
int timebase = 0, frame = 0;

bool axisBoolean = true;
bool showOrbit = true;
bool drawNormals = false;
GLenum mode = GL_FILL;
GLuint texIDFloor;

GLuint texIDPlane;
unordered_map<string, GLuint> textureMap;

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

class Textu {

    float x, y;

public:

    Textu() {
        this->x = 0;
        this->y = 0;
    }

    Textu(float cx, float cy) {
        this->x = cx;
        this->y = cy;
    }

    float getX() const {
        return this->x;
    }

    float getY() const {
        return this->y;
    }

    // Setters
    void setX(float cx) {
        this->x = cx;
    }

    void setY(float cy) {
        this->y = cy;
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

int getLightNr(int lightNr) {
    int lightConstant;
    switch (lightNr) {
    case 0:
        lightConstant = GL_LIGHT0;
        //printf("CONSTANT 0 = %f\n", lightConstant);
        break;
    case 1:
        lightConstant = GL_LIGHT1;
        //printf("CONSTANT 1 = %f\n", lightConstant);
        break;
    case 2:
        lightConstant = GL_LIGHT2;
        break;
    case 3:
        lightConstant = GL_LIGHT3;
        break;
    case 4:
        lightConstant = GL_LIGHT4;
        break;
    case 5:
        lightConstant = GL_LIGHT5;
        break;
    case 6:
        lightConstant = GL_LIGHT6;
        break;
    case 7:
        lightConstant = GL_LIGHT7;
        break;
    }
    return lightConstant;
}

class Light {
public:
    int index;
    virtual void apply() = 0;

};

class LightPoint : public Light {
    float pos[4];

public:

    LightPoint(int i, float a, float b, float c) {
        this->index = getLightNr(i);
        this->pos[0] = a;
        this->pos[1] = b;
        this->pos[2] = c;
        this->pos[3] = 1.0f;
    }

    void apply() {
        glLightfv(this->index, GL_POSITION, this->pos);
        cout << "PointLight: Indice" << this->index << "Pos: (" << pos[0] << ", " << pos[1] << ", " << pos[2] << "1.0f)" << endl;

    }
};

class DirectionalLight : public Light {
    float dir[4];

public:

    DirectionalLight(int i, float a, float b, float c) {
        this->index = getLightNr(i);
        this->dir[0] = a;
        this->dir[1] = b;
        this->dir[2] = c;
        this->dir[3] = 0.0f;
    }

    void apply() {
        glLightfv(this->index, GL_POSITION, this->dir);
        cout << "DirectionalLight: Indice" << this->index << "Pos: (" << dir[0] << ", " << dir[1] << ", " << dir[2] << "1.0f)" << endl;

    }
};

class SpotLight : public Light {
    float pos[4], dir[4], cutoff;

public:

    SpotLight(int i, float a, float b, float c, float da, float db, float dc, float cut) {
        this->index = getLightNr(i);
        this->pos[0] = a; this->pos[1] = b;
        this->pos[2] = c; this->pos[3] = 1.0f;
        this->dir[0] = da; this->dir[1] = db;
        this->dir[2] = dc; this->dir[3] = 0.0f;
        this->cutoff = cut;
    }

    void apply() {
        glLightfv(this->index, GL_POSITION, this->pos);
        glLightfv(this->index, GL_SPOT_DIRECTION, this->dir);
        glLightfv(this->index, GL_SPOT_CUTOFF, &(this->cutoff));
        cout << "SpotLight: Indice" << this->index << "Pos: (" << pos[0] << ", " << pos[1] << ", " << pos[2] << "1.0f)" << endl;
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
        glColor3f(0.6f, 0.3f, 0.4f);
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

    GLuint vertexBuffer, normalBuffer, texBuffer;
    vector<Point> points;
    vector<Point> normal;
    vector<Textu> textures;
    vector<Transformation*> transformations;

public:
    bool hasTexture = false;
    GLfloat diffuse[4] = { 200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f };
    GLfloat ambient[4] = { 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1 };
    GLfloat specular[4] = { 0, 0, 0, 1 };
    GLfloat emissive[4] = { 0, 0, 0, 1 };
    GLfloat shininess = 0;
    GLuint textureID;

public:
    Model() {
        this->hasTexture = false;
        this->points = vector<Point>();
        this->transformations = vector<Transformation*>();
        this->normal = vector<Point>();
        this->textures = vector<Textu>();
    }

    Model(GLuint vBuffer, GLuint nBuffer, GLuint tBuffer, vector<Point> p, vector<Point> n, vector<Textu> t) {
        this->hasTexture = false;
        this->vertexBuffer = vBuffer;
        this->normalBuffer = nBuffer;
        this->texBuffer = tBuffer;
        this->points = p;
        this->normal = n;
        this->textures = t;
    }

    const GLuint getVertexBuffer() const {
        return this->vertexBuffer;
    }

    const GLuint getNormalBuffer() const {
        return this->normalBuffer;
    }

    const GLuint getTexBuffer() const {
        return this->texBuffer;
    }

    const vector<Point>& getPoints() const {
        return this->points;
    }

    const vector<Point>& getNornal() const {
        return this->normal;
    }

    const vector<Textu>& getTex() const {
        return this->textures;
    }

    const vector<Transformation*>& getTransformations() const {
        return this->transformations;
    }

    void setTransformations(const vector<Transformation*>& tr) {
        this->transformations = tr;
    }

    void setTextureID(GLuint texID) {
        this->textureID = texID;
    }
};

vector<Model> models;
vector<Light*> lights;

int loadTexture(string s) {

    unsigned int t, tw, th;
    unsigned char* texData;
    unsigned int texID;

    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilGenImages(1, &t);
    ilBindImage(t);
    ilLoadImage((ILstring)(string("../textures/") + s).c_str());
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    printf("tw , th = %d %d\n", th, tw);
    texData = ilGetData();

    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;

}

void sphericalToCartesian() {

    camX = radius * cos(beta) * sin(alpha);
    camY = radius * sin(beta);
    camZ = radius * cos(beta) * cos(alpha);
}

Model read3dFile(string figureName) {

    printf("READ3DFILE - BEGIN\n");
    string line;
    int trianglesNumber;
    vector<Point> modelPoints, modelNormal;
    vector<Textu> modelTex;
    GLuint vertexBuffer, normalBuffer, texBuffer;

    stringstream filePath;
    filePath << "../file3d/" << figureName;

    ifstream file(filePath.str());

    if (file.is_open()) {
        if (getline(file, line)) {
            trianglesNumber = stoi(line);
            // printf("Vertices= %f\n", verticesNumber);
        }

        while (getline(file, line)) {
            stringstream ss(line);
            string type;
            float x, y, z;
            char comma;

            if (ss >> type) {
                if (type == "v") {  // Vertex data
                    if (ss >> x >> comma >> y >> comma >> z) {
                        // printf("x1= %f, y1=%f, z1=%f\n", x, y, z);
                        Point p1(x, y, z);
                        modelPoints.push_back(p1);
                    }
                }
                else if (type == "n") {  // Normal data
                    if (ss >> x >> comma >> y >> comma >> z) {
                        // printf("x2= %f, y2=%f, z2=%f\n", x, y, z);
                        Point p2(x, y, z);
                        modelNormal.push_back(p2);
                    }
                }
                else if (type == "t") {  // Texture coordinate data
                    if (ss >> x >> comma >> y) {
                        // printf("x3=%f, y3=%f\n", x, y);
                        Textu p3(x, y);
                        modelTex.push_back(p3);
                    }
                }
            }
        }
        printf("READ3DFILE - END\n");
    }

    file.close();

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, modelPoints.size() * sizeof(Point), modelPoints.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, modelNormal.size() * sizeof(Point), modelNormal.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &texBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
    glBufferData(GL_ARRAY_BUFFER, modelTex.size() * sizeof(Textu), modelTex.data(), GL_STATIC_DRAW);

    Model m(vertexBuffer, normalBuffer, texBuffer, modelPoints, modelNormal, modelTex);

    return m;
}

void readXMLFileGroup(XMLElement* group, vector<Transformation*> tr) {

    printf("READXMLFILEGROUP - BEGIN\n");

    vector<Transformation*> originalTransformations = tr;

    while (group) {

        tr = originalTransformations;

        XMLElement* transformElement = group->FirstChildElement("transform");

        XMLElement* modelsElement = group->FirstChildElement("models");

        XMLElement* childGroup = group->FirstChildElement("group");

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

                /*for (int k = 0; k < tr.size(); k++) {
                    tr[k]->print();
                }*/

                ts = ts->NextSiblingElement();
            }
        }

        if (modelsElement) {

            for (XMLElement* m = modelsElement->FirstChildElement("model"); m; m = m->NextSiblingElement()) {
                Model mo = read3dFile(m->Attribute("file"));

                XMLElement* colorsElement = m->FirstChildElement("color");
                if (colorsElement) {

                    for (XMLElement* c = colorsElement->FirstChildElement(); c; c = c->NextSiblingElement()) {
                        string colorName = string(c->Name());

                        if (!strcmp(colorName.c_str(), "diffuse")) {
                            GLfloat r, g, b;
                            r = atof(c->FindAttribute("R")->Value());
                            g = atof(c->FindAttribute("G")->Value());
                            b = atof(c->FindAttribute("B")->Value());

                            mo.diffuse[0] = r; mo.diffuse[1] = g;
                            mo.diffuse[2] = b; mo.diffuse[3] = 1.0f;

                        }

                        if (!strcmp(colorName.c_str(), "ambient")) {
                            GLfloat r, g, b;
                            r = atof(c->FindAttribute("R")->Value());
                            g = atof(c->FindAttribute("G")->Value());
                            b = atof(c->FindAttribute("B")->Value());

                            mo.ambient[0] = r; mo.ambient[1] = g;
                            mo.ambient[2] = b; mo.ambient[3] = 1.0f;
                        }

                        if (!strcmp(colorName.c_str(), "specular")) {
                            GLfloat r, g, b;
                            r = atof(c->FindAttribute("R")->Value());
                            g = atof(c->FindAttribute("G")->Value());
                            b = atof(c->FindAttribute("B")->Value());

                            mo.specular[0] = r; mo.specular[1] = g;
                            mo.specular[2] = b; mo.specular[3] = 1.0f;
                        }

                        if (!strcmp(colorName.c_str(), "emissive")) {
                            GLfloat r, g, b;
                            r = atof(c->FindAttribute("R")->Value());
                            g = atof(c->FindAttribute("G")->Value());
                            b = atof(c->FindAttribute("B")->Value());

                            mo.emissive[0] = r; mo.emissive[1] = g;
                            mo.emissive[2] = b; mo.emissive[3] = 1.0f;
                        }

                        if (!strcmp(colorName.c_str(), "shininess")) {
                            GLfloat shi;
                            shi = atof(c->FindAttribute("value")->Value());

                            mo.shininess = shi;
                        }
                    }
                }

                XMLElement* textureElement = m->FirstChildElement("texture"); // -> meter mo.setTexBool = true;

                if (textureElement) {
                    mo.hasTexture = true;
                    const char* file = textureElement->Attribute("file");
                    auto it = textureMap.find(file);
                    if (it == textureMap.end()) {
                        // File not found in map, load the texture and add to map
                        GLuint textureID = loadTexture(file);
                        textureMap[file] = textureID;
                        mo.textureID = textureID;
                    }
                    else {
                        // File found in map, use existing texture ID
                        // printf("ENCONTROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOUUUUUUUUUUUUUUU\n");
                        mo.textureID = it->second;
                    }
                }

                mo.setTransformations(tr);
                models.push_back(mo);
            }
        }

        readXMLFileGroup(childGroup, tr);
        group = group->NextSiblingElement("group");
    }
    printf("READXMLFILEGROUP - END\n");

}

void readXMLFile() {

    printf("READXMLFILE - BEGIN\n");

    XMLError eResult = doc.LoadFile(filename);
    if (eResult != XML_SUCCESS) {
        cerr << "Nao foi possivel abrir o ficheiro XML: " << filename << endl;
        return;
    }

    XMLElement* worldElement = doc.FirstChildElement("world");

    if (worldElement) {

        vector<Transformation*> transf;

        int lightIndex = 0;
        printf("INDEX NUMBER BEGGINING = %d\n", lightIndex);

        XMLElement* windowElement = worldElement->FirstChildElement("window");
        XMLElement* cameraElement = worldElement->FirstChildElement("camera");
        XMLElement* lightsElement = worldElement->FirstChildElement("lights");
        XMLElement* groupElement = worldElement->FirstChildElement("group");

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


        if (lightsElement) {
            for (XMLElement* l = lightsElement->FirstChildElement("light"); l; l = l->NextSiblingElement()) {
                string type = l->Attribute("type");

                if (!strcmp(type.c_str(), "point")) {
                    float x, y, z;
                    x = atof(l->FindAttribute("posx")->Value());
                    y = atof(l->FindAttribute("posy")->Value());
                    z = atof(l->FindAttribute("posz")->Value());

                    printf("LIGHT: %s X: %f, Y: %f, Z: %f\n", type.c_str(), x, y, z);
                    lights.push_back(new LightPoint(lightIndex, x, y, z));
                }
                else if (!strcmp(type.c_str(), "directional")) {
                    float x, y, z;
                    x = atof(l->FindAttribute("dirx")->Value());
                    y = atof(l->FindAttribute("diry")->Value());
                    z = atof(l->FindAttribute("dirz")->Value());

                    //printf("LIGHT: %s X: %f, Y: %f, Z: %f\n", type.c_str(), x, y, z);
                    lights.push_back(new DirectionalLight(lightIndex, x, y, z));
                }
                else if (!strcmp(type.c_str(), "spot")) {
                    float x1, y1, z1, x2, y2, z2, cutoff;
                    x1 = atof(l->FindAttribute("posx")->Value());
                    y1 = atof(l->FindAttribute("posy")->Value());
                    z1 = atof(l->FindAttribute("posz")->Value());
                    x2 = atof(l->FindAttribute("dirx")->Value());
                    y2 = atof(l->FindAttribute("diry")->Value());
                    z2 = atof(l->FindAttribute("dirz")->Value());
                    cutoff = atof(l->FindAttribute("cutoff")->Value());

                    //printf("LIGHT: %s X1: %f, Y1: %f, Z1: %f, X2: %f, Y2: %f, Z2: %f, CUT: %f\n", type.c_str(), x1, y1, z1, x2, y2, z2, cutoff);
                    lights.push_back(new SpotLight(lightIndex, x1, y1, z1, x2, y2, z2, cutoff));
                }
                printf("INDEX NUMBER BEFORE LIGNTINDE++ = %d\n", lightIndex);
                lightIndex++;

            }
        }

        readXMLFileGroup(groupElement, transf);
    }

    printf("READXMLFILE - END\n");

}

void setLights() {

    printf("SETLIGHTS - BEGIN\n");

    for (int j = 0; j < lights.size(); j++) {
        lights[j]->apply();
    }

    printf("SETLIGHTS - END\n");

}

void drawAxis() {

    printf("DRAWAXIS - BEGIN\n");

    glDisable(GL_LIGHTING); // Disable lighting
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
    glEnable(GL_LIGHTING);

    printf("DRAWAXIS - END\n");


}

void drawModel() {

    printf("DRAWMODEL - BEGIN\n");

    for (Model model : models) {
        printf("TRANSFORMATION - BEFORE\n");

        glPushMatrix();
        for (Transformation* t : model.getTransformations()) {
            t->apply();
        }

        printf("TRANSFORMATION - AFTER\n");
        printf("COLOR - BEFORE\n");

        if (!model.hasTexture) {
            for (int i = 0; i < 4; i++) {
                model.ambient[i] = model.ambient[i] / 255;
                model.diffuse[i] = model.diffuse[i] / 255;
                model.specular[i] = model.specular[i] / 255;
                model.emissive[i] = model.emissive[i] / 255;
            }

            glMaterialfv(GL_FRONT, GL_AMBIENT, model.ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, model.diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, model.specular);
            glMaterialfv(GL_FRONT, GL_EMISSION, model.emissive);
            glMaterialf(GL_FRONT, GL_SHININESS, model.shininess);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, model.textureID);
        }

        printf("COLOR - AFTER\n");
        printf("VERTEX - BEFORE\n");

        GLuint vertexVBO = model.getVertexBuffer();
        if (vertexVBO) {
            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
            glVertexPointer(3, GL_FLOAT, 0, nullptr);
            glEnableClientState(GL_VERTEX_ARRAY);
            // printf("Entrou neste if -> VERTEX\n");
        } else {
            printf("Invalid vertex VBO\n");
            continue;
        }

        printf("VERTEX - AFTER\n");
        printf("NORMAL - BEFORE\n");

        GLuint normalVBO = model.getNormalBuffer();
        if (normalVBO) {
            glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
            glNormalPointer(GL_FLOAT, 0, nullptr);
            glEnableClientState(GL_NORMAL_ARRAY);
            //printf("Entrou neste if -> NORMAL\n");
        } else {
            printf("Invalid normal VBO\n");
            continue;
        }

        printf("NORMAL - AFTER\n");
        printf("TEXTURE -BEFORE\n");

        GLuint textVBO = model.getTexBuffer();
        if (textVBO) {
            glBindTexture(GL_ARRAY_BUFFER, textVBO);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
            // alterar para isto depois 
            // glVertexPointer(2, GL_FLOAT, 0, nullptr);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            //printf("Entrou neste if -> TEXTURE\n");
        }
        else {
            printf("Invalid tex VBO\n");
            continue;
        }


        printf("DRAW - BEFORE\n");

        size_t pointsSize = model.getPoints().size();
        printf("POINTS = %zu\n", pointsSize);
        printf("POINTSSS = %zu\n", model.getNornal().size());
        printf("POINTSSSSSS = %zu\n", model.getTex().size());

        if (pointsSize > 0) {
            glDrawArrays(GL_TRIANGLES, 0, pointsSize);
        }
        else {
            printf("Invalid points size\n");
        }

        printf("DRAW - AFTER\n");

        if (drawNormals) {
            glDisable(GL_LIGHTING); // Disable lighting

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);

            glColor3f(1.0f, 0.0f, 0.0f);

            glBegin(GL_LINES);
            for (size_t i = 0; i < model.getPoints().size(); ++i) {
                Point p = model.getPoints()[i];
                Point n = model.getNornal()[i];
                glVertex3f(p.getX(), p.getY(), p.getZ());
                glVertex3f(p.getX() + n.getX() * 5, p.getY() + n.getY() * 5, p.getZ() + n.getZ() * 5);
            }
            glEnd();

            glEnable(GL_LIGHTING);
        }

        glPopMatrix();
    }

    printf("DRAWMODEL - END\n");
}

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

void renderScene(void) {

    int fps;
    char s[64];

    // Clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the camera
    glLoadIdentity();

    gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);

    if (axisBoolean) drawAxis();

    // set lights
    setLights();

    float white[4] = { 1,1,1,1 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);

    drawModel();
    //  glutSolidCube(2); // -> serve para ver se as luzes funcioam 


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
    case '6':
        drawNormals = !drawNormals;
        break;
    }
    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy)
{
    switch (key) {

    case GLUT_KEY_F1:
        // diminuir velocidade rotacao
        for (Model& model : models) {
            for (Transformation* t : model.getTransformations()) {
                Curve* cur = dynamic_cast<Curve*>(t);
                if (cur && cur->getTime() < 50000) {
                    cur->setTime(cur->getTime() * 2);
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
                    cur->setTime(cur->getTime() * 0.5);
                    printf("time= %f\n", cur->getTime());
                }
            }
        }
        break;

    case GLUT_KEY_RIGHT:
        alpha -= 0.1;
        break;

    case GLUT_KEY_LEFT:
        alpha += 0.1;
        break;

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

    case GLUT_KEY_PAGE_DOWN:
        radius -= 0.5f;
        if (radius < 0.1f)
            radius = 0.1f;
        break;

    case GLUT_KEY_PAGE_UP:
        radius += 1.0f;
        break;

    }
    sphericalToCartesian();

}

int main(int argc, char** argv) {

    // Initialize GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@DI-UM-GRUPO14");

    // Register callbacks
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    glewInit();
    ilInit();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    readXMLFile();

    // put here the registration of the keyboard callbacks
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_TEXTURE_2D);

    float amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

    for (int i = 0; i < lights.size(); i++) {
        glEnable(GL_LIGHT0 + i);
        float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        // light colors
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, white);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, white);
    }

    sphericalToCartesian();

    // Enter GLUT's main loop
    glutMainLoop();

    return 0;
}