#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <cstring>
#include <tuple>
#include <vector>
#include <iostream>
#include <algorithm>


#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>
#include "tinyxml2/tinyxml2.h"


namespace fs = std::filesystem;

using namespace std;
using namespace tinyxml2;
using namespace fs;

XMLDocument doc;
const char* filename = "creator.xml";
int nPatches, nCPontos;

// Classe Ponto e construtores
class Ponto {

    float x;
    float y;
    float z;

public:

    Ponto() {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Ponto(float cx, float cy, float cz) {
        x = cx;
        y = cy;
        z = cz;
    }

    string PontoToString() const {
        return to_string(x) + ", " + to_string(y) + ", " + to_string(z);
    }

    string PontoTexToString() const {
        return to_string(x) + ", " + to_string(y);
    }

    float getX() const {
        return this->x;
    }

    float getY() const {
        return this->y;
    }
    
    float getZ() const {
        return this->x;
    }

    Ponto operator-(const Ponto& other) const {
        return Ponto(x - other.getX(), y - other.getY(), z - other.z);
    }

    Ponto cross(const Ponto& other) const {
        return Ponto(
            y * other.z - z * other.getY(),
            z * other.getX() - x * other.z,
            x * other.getY() - y * other.getX()
        );
    }

    Ponto normalize() {
    float length = sqrt(x * x + y * y + z * z);
    if (length != 0) {
        return Ponto(x / length, y / length, z / length);
    } else {
        return *this;
    }
}


};

// Classe Triangulo e construtores
class Triangulo {

    tuple <Ponto, Ponto, Ponto> triangulo;

public:

    Triangulo() {
        Ponto p;
        triangulo = make_tuple(p, p, p);
    }

    Triangulo(Ponto p1, Ponto p2, Ponto p3) {
        triangulo = make_tuple(p1, p2, p3);
    }

    void printTriangulo(ofstream& file, string type) const {
        file << type << " " << get<0>(triangulo).PontoToString() << endl;   // Primeiro Vertice
        file << type << " " << get<1>(triangulo).PontoToString() << endl;   // Segundo Vertice
        file << type << " " << get<2>(triangulo).PontoToString() << endl;   // Terceiro Vertice
    }

    void printTrianguloTex(ofstream& file, string type) const {
        file << type << " " << get<0>(triangulo).PontoTexToString() << endl;   // Primeiro Vertice
        file << type << " " << get<1>(triangulo).PontoTexToString() << endl;   // Segundo Vertice
        file << type << " " << get<2>(triangulo).PontoTexToString() << endl;   // Terceiro Vertice
    }

};

void normalize(float* a) {

    float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0] / l;
    a[1] = a[1] / l;
    a[2] = a[2] / l;
}

void cross(float* a, float* b, float* res) {

    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

void createXML() {
  
    if(!doc.LoadFile(filename) == XML_SUCCESS) {

    XMLElement* world = doc.NewElement("world");
    doc.InsertEndChild(world);

    XMLElement* window = doc.NewElement("window");
    window->SetAttribute("width", "512");
    window->SetAttribute("height", "512");
    world->InsertEndChild(window);

    XMLElement* camera = doc.NewElement("camera");
    world->InsertEndChild(camera);

    XMLElement* position = doc.NewElement("position");
    position->SetAttribute("x", "3");
    position->SetAttribute("y", "2");
    position->SetAttribute("z", "1");
    camera->InsertEndChild(position);

    XMLElement* lookAt = doc.NewElement("lookAt");
    lookAt->SetAttribute("x", "0");
    lookAt->SetAttribute("y", "0");
    lookAt->SetAttribute("z", "0");
    camera->InsertEndChild(lookAt);

    XMLElement* up = doc.NewElement("up");
    up->SetAttribute("x", "0");
    up->SetAttribute("y", "1");
    up->SetAttribute("z", "0");
    camera->InsertEndChild(up);

    XMLElement* projection = doc.NewElement("projection");
    projection->SetAttribute("fov", "60");
    projection->SetAttribute("near", "1");
    projection->SetAttribute("far", "1000");
    camera->InsertEndChild(projection);

    XMLElement* group = doc.NewElement("group");
    world->InsertEndChild(group);

    XMLElement* models = doc.NewElement("models");
    group->InsertEndChild(models);

    XMLError eResult = doc.SaveFile(filename);
    cout << "Ficheiro XML " << filename << " criado com sucesso." << endl;

    }
}

void updateXMLFile(const char* file3d) {

    XMLError eResult = doc.LoadFile(filename);

    if (eResult == XML_SUCCESS) {

        XMLElement* worldElement = doc.FirstChildElement("world");

        if (worldElement) {
            XMLElement* groupElement = worldElement->FirstChildElement("group");

            if (groupElement) {
                XMLElement* modelsElement = groupElement->FirstChildElement("models");

                if (modelsElement) {

                    XMLElement* existingModel = modelsElement->FirstChildElement("model");
                    bool modelExists = false;

                    while (existingModel) {
                        const char* existingFile = existingModel->Attribute("file");
                        if (existingFile && strcmp(existingFile, file3d) == 0) {
                            modelExists = true;
                            break;
                        }
                        existingModel = existingModel->NextSiblingElement("model");
                    }

                    if (!modelExists) {
                        XMLElement* modelElement = doc.NewElement("model");
                        modelElement->SetAttribute("file", file3d);
                        modelsElement->InsertEndChild(modelElement);
                    }
                }
            }
        }    
        doc.SaveFile(filename);
        cout <<  filename << " atualizado com sucesso." << endl;
    } else {
        cerr << "Não foi possivel abrir o ficheiro XML: " << filename << endl;
    }
}

void newPrintToFile(const vector<Triangulo>& tri, const vector<Triangulo>& nor, const vector<Triangulo> tex, const string& f) {
    ofstream file(f, ios::out);
    if (file.is_open()) {
        if (tri.size() == nor.size() && nor.size() == tex.size()) {
            file << tri.size() << endl;
        } else  {
            file << tri.size() << " " << nor.size() << " " << tex.size() << endl;
        }
        for (const auto& triangulo : tri) {
            triangulo.printTriangulo(file, "v");
        }
        for (const auto& triangulo : nor) {
            triangulo.printTriangulo(file, "n");
        }
        for (const auto& triangulo : tex) {
            triangulo.printTrianguloTex(file, "t");
        }
        file.close();
    } else {
        cout << "Não foi possível abrir o ficheiro: " << f << endl;
    }
}

void printToFile(const vector<Triangulo>& v, const string& f) {
    ofstream file(f, ios::out);
    if (file.is_open()) {
        file << v.size() << endl;
        for (const auto& triangulo : v) {
            triangulo.printTriangulo(file, "v");
        }
        file.close();
    } else {
        cout << "Não foi possível abrir o ficheiro: " << f << endl;
    }
}

// generate Plane              1          4
void generatePlane(float dimension, int divisions, const string& plane3d) {
    vector<Triangulo> triangulos;
    vector<Triangulo> normals;
    vector<Triangulo> texturas;

    float squareHeight = dimension / divisions; // 0.25
    float halfDimension = dimension / 2.0f; // 0.5
    float tex_step = 1.0f / divisions;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {

            float x1 = i * squareHeight - halfDimension;  // -0.5
            float z1 = j * squareHeight - halfDimension;
            float x2 = (i + 1) * squareHeight - halfDimension;  //-0.25
            float z2 = (j + 1) * squareHeight - halfDimension;

            Ponto p1(x1, 0.0f, z1);
            Ponto p2(x1, 0.0f, z2);
            Ponto p3(x2, 0.0f, z1);
            Ponto p4(x2, 0.0f, z2);

            Ponto normal(0.0f, 1.0f, 0.0f);
            Triangulo TrianguloNormal(normal, normal, normal);

            Ponto t1(i * tex_step, j * tex_step, 0);
            Ponto t2(i * tex_step, (j + 1) * tex_step, 0);
            Ponto t3((i + 1) * tex_step, j * tex_step, 0);
            Ponto t4((i + 1) * tex_step, (j + 1) * tex_step, 0);

            Triangulo t_tri1(p1, p2, p3);
            Triangulo t_tri2(p3, p2, p4);

            triangulos.push_back(t_tri1);
            triangulos.push_back(t_tri2);

            normals.push_back(TrianguloNormal);
            normals.push_back(TrianguloNormal);

            texturas.push_back(Triangulo(t1, t2, t3));
            texturas.push_back(Triangulo(t3, t2, t4));
        }
    }

    newPrintToFile(triangulos, normals, texturas, "file3d/" + plane3d);

    triangulos.clear();
    normals.clear();
    texturas.clear();
}

// generate Box 
void generateBox(float dimension, int divisions, string box3d) {

    vector<Triangulo> triangulos;
    vector<Triangulo> normals;
    vector<Triangulo> texturas; 

    float squareHeight = dimension / divisions;
    float halfDimension = dimension / 2.0f;


    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {

            float x = i * squareHeight - halfDimension;
            float z = j * squareHeight - halfDimension;

            // Plano inferior
            // Primeiro Triangulo
            Ponto p1(x, -halfDimension, z);
            Ponto p2(x + squareHeight, -halfDimension, z);
            Ponto p3(x, -halfDimension, z + squareHeight);
            // Segundo Triangulo
            Ponto p4(x + squareHeight, -halfDimension, z);
            Ponto p5(x + squareHeight, -halfDimension, z + squareHeight);
            Ponto p6(x, -halfDimension, z + squareHeight);
            // Plano superior
            // Primeiro Triangulo
            Ponto p7(x, halfDimension, z);
            Ponto p8(x, halfDimension, z + squareHeight);
            Ponto p9(x + squareHeight, halfDimension, z);
            // Segundo Triangulo
            Ponto p10(x + squareHeight, halfDimension, z);
            Ponto p11(x, halfDimension, z + squareHeight);
            Ponto p12(x + squareHeight, halfDimension, z + squareHeight);

            Triangulo t1(p1,p2,p3);
            Triangulo t2(p4,p5,p6);
            Triangulo t3(p7,p8,p9);
            Triangulo t4(p10,p11,p12);

            Ponto normal_up(0, 1, 0);
            Triangulo TrianguloNormal_up(normal_up, normal_up, normal_up);

            Ponto normal_down(0, -1, 0);
            Triangulo TrianguloNormal_down(normal_down, normal_down, normal_down);

            Ponto tt1(0, 0, 0);
            Ponto tt2(0, 1, 0);
            Ponto tt3(1, 0, 0);
            Ponto tt4(1, 1, 0);

            triangulos.push_back(t1);
            triangulos.push_back(t2);
            
            normals.push_back(TrianguloNormal_down);
            normals.push_back(TrianguloNormal_down);
            texturas.push_back(Triangulo(tt1, tt2, tt3));
            texturas.push_back(Triangulo(tt2, tt4, tt3));

            triangulos.push_back(t3);
            triangulos.push_back(t4);

            normals.push_back(TrianguloNormal_up);
            normals.push_back(TrianguloNormal_up);
            texturas.push_back(Triangulo(tt3, tt2, tt1)); 
            texturas.push_back(Triangulo(tt3, tt4, tt2));
        }
    }

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float x = i * squareHeight - halfDimension;
            float y = j * squareHeight - halfDimension;

            // Plano frontal
            // Primeiro Triangulo
            Ponto p1(x, y, -halfDimension);
            Ponto p2(x, y + squareHeight, -halfDimension);
            Ponto p3(x + squareHeight, y, -halfDimension);
            // Segundo Triangulo
            Ponto p4(x + squareHeight, y, -halfDimension);
            Ponto p5(x, y + squareHeight, -halfDimension);
            Ponto p6(x + squareHeight, y + squareHeight, -halfDimension);
            // Plano traseiro
            // Primeiro Triangulo
            Ponto p7(x, y, halfDimension);
            Ponto p8(x + squareHeight, y, halfDimension);
            Ponto p9(x, y + squareHeight, halfDimension);
            // Segundo Triangulo
            Ponto p10(x + squareHeight, y, halfDimension);
            Ponto p11(x + squareHeight, y + squareHeight, halfDimension);
            Ponto p12(x, y + squareHeight, halfDimension);

            Triangulo t1(p1,p2,p3);
            Triangulo t2(p4,p5,p6);
            Triangulo t3(p7,p8,p9);
            Triangulo t4(p10,p11,p12);

            Ponto normal_front(0, 0, -1);
            Triangulo TrianguloNormal_front(normal_front, normal_front, normal_front);

            Ponto normal_back(0, 0, 1);
            Triangulo TrianguloNormal_back(normal_back, normal_back, normal_back);

            Ponto tt1(0, 0, 0);
            Ponto tt2(0, 1, 0);
            Ponto tt3(1, 0, 0);
            Ponto tt4(1, 1, 0);

            triangulos.push_back(t1);
            triangulos.push_back(t2);
            normals.push_back(TrianguloNormal_front);
            normals.push_back(TrianguloNormal_front);
            texturas.push_back(Triangulo(tt1, tt2, tt3));
            texturas.push_back(Triangulo(tt2, tt4, tt3));

            triangulos.push_back(t3);
            triangulos.push_back(t4);          
            normals.push_back(TrianguloNormal_back);
            normals.push_back(TrianguloNormal_back);
            texturas.push_back(Triangulo(tt3, tt2, tt1)); 
            texturas.push_back(Triangulo(tt3, tt4, tt2));
        }
    }

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float y = i * squareHeight - halfDimension;
            float z = j * squareHeight - halfDimension;
            // Plano esquerdo
            // Primeiro Triangulo
            Ponto p1(-halfDimension, y, z);
            Ponto p2(-halfDimension, y, z + squareHeight);
            Ponto p3(-halfDimension, y + squareHeight, z);
            // Segundo Triangulo
            Ponto p4(-halfDimension, y, z + squareHeight);
            Ponto p5(-halfDimension, y + squareHeight, z + squareHeight);
            Ponto p6(-halfDimension, y + squareHeight, z);
            // Plano direito
            // Primeiro Triangulo
            Ponto p7(halfDimension, y, z);
            Ponto p8(halfDimension, y + squareHeight, z);
            Ponto p9(halfDimension, y, z + squareHeight);
            // Segundo Triangulo
            Ponto p10(halfDimension, y + squareHeight, z);
            Ponto p11(halfDimension, y + squareHeight, z + squareHeight);
            Ponto p12(halfDimension, y, z + squareHeight);

            Triangulo t1(p1,p2,p3);
            Triangulo t2(p4,p5,p6);
            Triangulo t3(p7,p8,p9);
            Triangulo t4(p10,p11,p12);

            Ponto normal_left(-1, 0, 0);
            Triangulo TrianguloNormal_left(normal_left, normal_left, normal_left);

            Ponto normal_right(1, 0, 0);
            Triangulo TrianguloNormal_right(normal_right, normal_right, normal_right);

            Ponto tt1(0, 0, 0);
            Ponto tt2(0, 1, 0);
            Ponto tt3(1, 0, 0);
            Ponto tt4(1, 1, 0);

            triangulos.push_back(t1);
            triangulos.push_back(t2);
            normals.push_back(TrianguloNormal_left);
            normals.push_back(TrianguloNormal_left);
            texturas.push_back(Triangulo(tt1, tt2, tt3));
            texturas.push_back(Triangulo(tt2, tt4, tt3));

            triangulos.push_back(t3);
            triangulos.push_back(t4);
            normals.push_back(TrianguloNormal_right);
            normals.push_back(TrianguloNormal_right);
            texturas.push_back(Triangulo(tt1, tt3, tt2)); 
            texturas.push_back(Triangulo(tt2, tt3, tt4));
        }
    }


    newPrintToFile(triangulos, normals, texturas, "file3d/" + box3d);

    triangulos.clear();
    normals.clear();
    texturas.clear();
}

// generate Sphere
void generateSphere(float baseRadius, int slices, int stacks, string sphere3d) {
    vector<Triangulo> triangulos;
    vector<Triangulo> normals;
    vector<Triangulo> texturas;

    double stackSkew = M_PI / stacks, sliceSkew = (2 * M_PI) / slices;

    for (int i = 0; i < stacks; i++) {
        double phi = (M_PI / 2) - (i * stackSkew);
        double nextPhi = (M_PI / 2) - ((i + 1) * stackSkew);

        for (int j = 0; j < slices; j++) {
            double teta = j * sliceSkew;
            double nextTeta = (j + 1) * sliceSkew;

            Ponto p1(baseRadius * cos(nextPhi) * cos(teta), baseRadius * sin(nextPhi), baseRadius * cos(nextPhi) * sin(teta));
            Ponto p2(baseRadius * cos(phi) * cos(teta), baseRadius * sin(phi), baseRadius * cos(phi) * sin(teta));
            Ponto p3(baseRadius * cos(phi) * cos(nextTeta), baseRadius * sin(phi), baseRadius * cos(phi) * sin(nextTeta));
            Ponto p4(baseRadius * cos(nextPhi) * cos(nextTeta), baseRadius * sin(nextPhi), baseRadius * cos(nextPhi) * sin(nextTeta));

            // Triangulo 1
            Triangulo t1(p1, p2, p3);
            triangulos.push_back(t1);

            Ponto normalP1 = p1.normalize();
            Ponto normalP2 = p2.normalize();
            Ponto normalP3 = p3.normalize();
            Triangulo normal1(normalP1, normalP2, normalP3);
            normals.push_back(normal1);

            Ponto textP1((float)j / slices, 1.0f - (float)(i + 1) / stacks, 0.0f);
            Ponto textP2((float)j / slices, 1.0f - (float)i / stacks, 0.0f);
            Ponto textP3((float)(j + 1) / slices, 1.0f - (float)i / stacks, 0.0f);
            Triangulo textureT1(textP1, textP2, textP3);
            texturas.push_back(textureT1);

            // Triangulo 2
            Triangulo t2(p1, p3, p4);
            triangulos.push_back(t2);

            Ponto normalP4 = p4.normalize();
            Triangulo normal2(normalP1, normalP3, normalP4);
            normals.push_back(normal2);

            Ponto textP4((float)j / slices, 1.0f - (float)(i + 1) / stacks, 0.0f);
            Ponto textP5((float)(j + 1) / slices, 1.0f - (float)i / stacks, 0.0f);
            Ponto textP6((float)(j + 1) / slices, 1.0f - (float)(i + 1) / stacks, 0.0f);
            Triangulo textureT2(textP4, textP5, textP6);
            texturas.push_back(textureT2);
        }
    }

    newPrintToFile(triangulos, normals, texturas, "file3d/" + sphere3d);

    triangulos.clear();
    normals.clear();
    texturas.clear();
}




// generate cone
void generateCone(double baseRadius, double height, double slices, double stacks, string cone3d) {
    vector<Triangulo> triangulos;
    vector<Triangulo> normals;
    vector<Triangulo> texturas;

    double alpha = (2 * M_PI) / slices;
    double beta = height / stacks;
    double i, j;
    double angle, raio1, raio2, frstPonto, scndPonto;
    float coneAngle = atan(baseRadius / height);

    for (i = 0; i < slices; i++) {
        angle = alpha * i;
        Ponto p1(0.0f, 0.0f, 0.0f);
        Ponto p2(baseRadius * sin(angle + alpha), 0.0f, baseRadius * cos(angle + alpha));
        Ponto p3(baseRadius * sin(angle), 0.0f, baseRadius * cos(angle));

        Ponto normal_down(0, -1, 0);
        Triangulo TrianguloNormal_down(normal_down, normal_down, normal_down);

        Triangulo t1(p1, p2, p3);
        triangulos.push_back(t1);
        normals.push_back(TrianguloNormal_down);
    }

    for (i = 0; i < stacks; i++) {
        scndPonto = i * beta;
        frstPonto = (i + 1) * beta;

        raio2 = baseRadius - ((baseRadius / stacks) * i);
        raio1 = baseRadius - ((baseRadius / stacks) * (i + 1));

        for (j = 0; j < slices; j++) {
            double circleAngle = alpha * j;
            double nextCircleAngle = alpha * (j + 1);

            Ponto p1(raio2 * sin(circleAngle), scndPonto, raio2 * cos(circleAngle));
            Ponto p2(raio1 * sin(nextCircleAngle), frstPonto, raio1 * cos(nextCircleAngle));
            Ponto p3(raio1 * sin(circleAngle), frstPonto, raio1 * cos(circleAngle));

            Ponto p4(raio2 * sin(circleAngle), scndPonto, raio2 * cos(circleAngle));
            Ponto p5(raio2 * sin(nextCircleAngle), scndPonto, raio2 * cos(nextCircleAngle));
            Ponto p6(raio1 * sin(nextCircleAngle), frstPonto, raio1 * cos(nextCircleAngle));

            Ponto normal1(
                cos(coneAngle) * cos(circleAngle),
                sin(coneAngle),
                cos(coneAngle) * sin(circleAngle)
            );

            Ponto normal2(
                cos(coneAngle) * cos(nextCircleAngle),
                sin(coneAngle),
                cos(coneAngle) * sin(nextCircleAngle)
            );

            double texCoord1X = j / slices;
            double texCoord1Y = i / stacks;
            double texCoord2X = (j + 1) / slices;
            double texCoord2Y = (i + 1) / stacks;

            Ponto texCoordP1(texCoord1X, texCoord1Y, 0.0);
            Ponto texCoordP2(texCoord2X, texCoord1Y, 0.0);
            Ponto texCoordP3(texCoord1X, texCoord2Y, 0.0);
            Ponto texCoordP4(texCoord1X, texCoord1Y, 0.0);
            Ponto texCoordP5(texCoord2X, texCoord1Y, 0.0);
            Ponto texCoordP6(texCoord2X, texCoord2Y, 0.0);

            Triangulo t1(p1, p2, p3);
            Triangulo t2(p4, p5, p6);

            Triangulo normalTriangle1(normal1, normal1, normal1);
            Triangulo normalTriangle2(normal2, normal2, normal2);

            Triangulo texCoordTriangle1(texCoordP1, texCoordP2, texCoordP3);
            Triangulo texCoordTriangle2(texCoordP4, texCoordP5, texCoordP6);

            triangulos.push_back(t1);
            triangulos.push_back(t2);

            normals.push_back(normalTriangle1);
            normals.push_back(normalTriangle2);

            texturas.push_back(texCoordTriangle1);
            texturas.push_back(texCoordTriangle2);
        }
    }

    newPrintToFile(triangulos, normals, texturas, "file3d/" + cone3d);
    triangulos.clear();
    normals.clear();
    texturas.clear();
}


// generate Cylinder
void generateCylinder(float baseRadius, float height, int slices, string cylinder3d) {
    vector<Triangulo> triangulos;
    vector<Triangulo> normals;
    vector<Triangulo> texturas;
    float halfHeight = height / 2.0f;

    for (int i = 0; i < slices; ++i) {
        float alpha1 = 2.0f * M_PI / slices * i;
        float alpha2 = 2.0f * M_PI / slices * (i + 1);

        // Pontos dos triângulos de cima e baixo
        Ponto p1(0.0f, -halfHeight, 0.0f);
        Ponto p2(baseRadius * cos(alpha1), -halfHeight, baseRadius * sin(alpha1));
        Ponto p3(baseRadius * cos(alpha2), -halfHeight, baseRadius * sin(alpha2));
        Ponto p4(0.0f, halfHeight, 0.0f);
        Ponto p5(baseRadius * cos(alpha2), halfHeight, baseRadius * sin(alpha2));
        Ponto p6(baseRadius * cos(alpha1), halfHeight, baseRadius * sin(alpha1));

        // Pontos dos triângulos laterais
        Ponto p7(baseRadius * cos(alpha1), -halfHeight, baseRadius * sin(alpha1));
        Ponto p8(baseRadius * cos(alpha1), halfHeight, baseRadius * sin(alpha1));
        Ponto p9(baseRadius * cos(alpha2), -halfHeight, baseRadius * sin(alpha2));
        Ponto p10(baseRadius * cos(alpha2), halfHeight, baseRadius * sin(alpha2));

        Ponto t1(0.5f, 0.5f, 0.0f); // Centro da base
        Ponto t2(0.5f + 0.5f * cos(alpha1), 0.5f + 0.5f * sin(alpha1), 0.0f);
        Ponto t3(0.5f + 0.5f * cos(alpha2), 0.5f + 0.5f * sin(alpha2), 0.0f);

        Ponto t4(0.5f, 0.5f, 0.0f); // Centro do topo
        Ponto t5(0.5f + 0.5f * cos(alpha2), 0.5f + 0.5f * sin(alpha2), 0.0f);
        Ponto t6(0.5f + 0.5f * cos(alpha1), 0.5f + 0.5f * sin(alpha1), 0.0f);

        Ponto t7(float(i) / slices, 0.0f, 0.0f); // Lateral inferior
        Ponto t8(float(i) / slices, 1.0f, 0.0f); // Lateral superior
        Ponto t9(float(i + 1) / slices, 0.0f, 0.0f); // Lateral inferior
        Ponto t10(float(i + 1) / slices, 1.0f, 0.0f); // Lateral superior

        Triangulo tri1(p1, p2, p3); // Base
        Triangulo tri2(p4, p5, p6); // Topo

        // Triângulos laterais
        Triangulo tri3(p7, p8, p9);
        Triangulo tri4(p9, p8, p10);

        triangulos.push_back(tri1);
        triangulos.push_back(tri2);
        triangulos.push_back(tri3);
        triangulos.push_back(tri4);

        Ponto normal1 = (p2 - p1).cross(p3 - p1).normalize(); // Base
        Ponto normal2 = (p5 - p4).cross(p6 - p4).normalize(); // Topo
        Ponto normal3 = (p8 - p7).cross(p9 - p7).normalize(); // Lateral 1
        Ponto normal4 = (p8 - p9).cross(p10 - p9).normalize(); // Lateral 2

        Triangulo normal_tri1(normal1, normal1, normal1);
        Triangulo normal_tri2(normal2, normal2, normal2);
        Triangulo normal_tri3(normal3, normal3, normal3);
        Triangulo normal_tri4(normal4, normal4, normal4);

        normals.push_back(normal_tri1);
        normals.push_back(normal_tri2);
        normals.push_back(normal_tri3);
        normals.push_back(normal_tri4);

        Triangulo tex_tri1(t1, t2, t3);
        Triangulo tex_tri2(t4, t5, t6);
        Triangulo tex_tri3(t7, t8, t9);
        Triangulo tex_tri4(t9, t8, t10);

        texturas.push_back(tex_tri1);
        texturas.push_back(tex_tri2);
        texturas.push_back(tex_tri3);
        texturas.push_back(tex_tri4);
    }

    newPrintToFile(triangulos, normals, texturas, "file3d/" + cylinder3d);
    triangulos.clear();
    normals.clear();
    texturas.clear();
}

// generate Torus 
void generateTorus(float outterRadius, float innerRadius, float slices, float stacks, string torus3d) {
    vector<Triangulo> triangulos;
    vector<Triangulo> normals;
    vector<Triangulo> texturas;

    double radius = (innerRadius + outterRadius) / 2;
    double dist = radius - innerRadius;

    double sliceStep = (2*M_PI)/slices;
    double stackStep = (2*M_PI)/stacks;

    for(int slice = 0; slice < slices; slice++){

        for(int stack = 0; stack < stacks; stack++){
            
            Ponto p1 = Ponto((radius + dist * cos(slice * sliceStep))       * cos(stack * stackStep)      , dist * sin(slice * sliceStep)      , (radius + dist * cos(slice * sliceStep))       * sin(stack * stackStep));
            Ponto p2 = Ponto((radius + dist * cos((slice + 1) * sliceStep)) * cos(stack * stackStep)      , dist * sin((slice + 1) * sliceStep), (radius + dist * cos((slice + 1) * sliceStep)) * sin(stack * stackStep));
            Ponto p3 = Ponto((radius + dist * cos((slice + 1) * sliceStep)) * cos((stack + 1) * stackStep), dist * sin((slice + 1) * sliceStep), (radius + dist * cos((slice + 1) * sliceStep)) * sin((stack + 1) * stackStep));
            Ponto p4 = Ponto((radius + dist * cos(slice * sliceStep))       * cos((stack + 1) * stackStep), dist * sin(slice * sliceStep)      , (radius + dist * cos(slice * sliceStep))       * sin((stack + 1) * stackStep));

            Ponto n1 = Ponto(cos(slice * sliceStep)       * cos(stack * stackStep)      , sin(slice * sliceStep)      , cos(slice * sliceStep)       * sin(stack * stackStep)).normalize();
            Ponto n2 = Ponto(cos((slice + 1) * sliceStep) * cos(stack * stackStep)      , sin((slice + 1) * sliceStep), cos((slice + 1) * sliceStep) * sin(stack * stackStep)).normalize();
            Ponto n3 = Ponto(cos((slice + 1) * sliceStep) * cos((stack + 1) * stackStep), sin((slice + 1) * sliceStep), cos((slice + 1) * sliceStep) * sin((stack + 1) * stackStep)).normalize();
            Ponto n4 = Ponto(cos(slice * sliceStep)       * cos((stack + 1) * stackStep), sin(slice * sliceStep)      , cos(slice * sliceStep)       * sin((stack + 1) * stackStep)).normalize();

            Ponto t1 = Ponto((double) slice / slices      , (double) stack / stacks      , 0);
            Ponto t2 = Ponto((double) (slice + 1) / slices, (double) stack / stacks      , 0);
            Ponto t3 = Ponto((double) (slice + 1) / slices, (double) (stack + 1) / stacks, 0);
            Ponto t4 = Ponto((double) slice / slices      , (double) (stack + 1) / stacks, 0);

            triangulos.push_back(Triangulo(p1,p2,p4));

            normals.push_back(Triangulo(n1,n2,n4));

            texturas.push_back(Triangulo(t1,t2,t4));


            triangulos.push_back(Triangulo(p2,p3,p4));
            normals.push_back(Triangulo(n2,n3,n4));
            texturas.push_back(Triangulo(t2,t3,t4));
        }
    }
    newPrintToFile(triangulos, normals, texturas, "file3d/" + torus3d);
    triangulos.clear();
    normals.clear();
    texturas.clear();
}

float* bezierFormula(float t, float* p0, float* p1, float* p2, float *p3){
    float* pon = new float[3];

    float vectorT[4] = { (float)(pow((1 - t),3)), 
                         (float)(3 * t * pow((1 - t),2)), 
                         (float)(3 * (1 - t) * pow(t,2)), 
                         (float)(pow(t,3)) };


    pon[0] = vectorT[0] * p0[0] + vectorT[1] * p1[0] + vectorT[2] * p2[0] + vectorT[3] * p3[0];
    pon[1] = vectorT[0] * p0[1] + vectorT[1] * p1[1] + vectorT[2] * p2[1] + vectorT[3] * p3[1];
    pon[2] = vectorT[0] * p0[2] + vectorT[1] * p1[2] + vectorT[2] * p2[2] + vectorT[3] * p3[2];

    return pon;


}

float* bezier(float n, float m, float** Pontos, int* index){
    int i, j = 0, N = 0;
    float* Ponto = new float[3];
    float* result = new float[3];
    float accPontos[4][3];
    float accBezier[4][3];

    for(i=0; i < 16; i++){

        accPontos[j][0] = Pontos[index[i]][0];
        accPontos[j][1] = Pontos[index[i]][1];
        accPontos[j][2] = Pontos[index[i]][2];

        printf("ACCOONTOS[J][0] = %f\n", accPontos[j][0]);
        printf("ACCOONTOS[J][1] = %f\n", accPontos[j][1]);
        printf("ACCOONTOS[J][2] = %f\n", accPontos[j][2]);

        j++;

        if(j % 4 == 0){

            Ponto = bezierFormula(n, accPontos[0], accPontos[1], accPontos[2], accPontos[3]);

            accBezier[N][0] = Ponto[0];
            accBezier[N][1] = Ponto[1];
            accBezier[N][2] = Ponto[2];
            j = 0;
            N += 1;
        }
    }
    result = bezierFormula(m, accBezier[0], accBezier[1], accBezier[2], accBezier[3]);

    return result;
}

void generateBezier(string patchName, int tessellation, string bezier3d){

    stringstream filePath;
    filePath << "patch/" << patchName;
    ifstream file(filePath.str()); // abrir o ficheiro da patch de bezier que estara em patch/

    ofstream write("file3d/" + bezier3d);

    vector<Triangulo> triangulos;

    string line, value;
    float step = 1.0 / tessellation;
    int patchIndex, PontoIndex, patchPontoIndex, coordinateIndex;


    if (file.is_open()) {

        getline(file, line);
        nPatches = stoi(line); // 32 
        printf("NPATCHES = %d\n", nPatches);

        int** patchIndices = new int*[nPatches];
        /*
        0, 1,  2,  3,  4, 5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15 -> tamnaho = 16
        3, 16, 17, 18, 7, 19, 20, 21, 11, 22, 23, 24, 15, 25, 26, 27 = tamanho = 16
        ...
        */
        
        float*** resPontoos = new float**[nPatches];

        for (patchIndex = 0; patchIndex < nPatches; patchIndex++) {
            getline(file, line);
            patchIndices[patchIndex] = new int[16];  // ver comentario acima

            line.erase(remove(line.begin(), line.end(), ','), line.end()); // Remove virgulas diretamente da linah

            stringstream ss(line);
            for (patchPontoIndex = 0; patchPontoIndex < 16; patchPontoIndex++) {
                ss >> patchIndices[patchIndex][patchPontoIndex];
                /*
                patchIndex = 0
                ss = pathcIndeces[0][16] = { 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 }
                ss = patchIndeces[1][16] = { 3 16 17 18 7 19 20 21 11 22 23 24 15 25 26 27 }
                ...
                */
            }
        }

        getline(file, line);
        int cPontos = stoi(line); // 290
        float** Pontos = new float*[cPontos];



        for (PontoIndex = 0; PontoIndex < cPontos; PontoIndex++) {
            getline(file, line);
            Pontos[PontoIndex] = new float[3];

            line.erase(remove(line.begin(), line.end(), ','), line.end()); // Remove virgulas diretamente da linah

            stringstream ss(line);
            for (coordinateIndex = 0; coordinateIndex < 3; coordinateIndex++) {
                ss >> Pontos[PontoIndex][coordinateIndex];
                // faz igual ao de cima mas agora para o control Points

            }
        }

        for (patchIndex = 0; patchIndex < nPatches; patchIndex++) {
            resPontoos[patchIndex] = new float*[4];

            for(int xx = 0; xx < tessellation; xx++ ) {
                for(int yy = 0; yy < tessellation; yy++) {

                    float x1 = step * xx;
                    float y1 = step * yy;
                    float x2 = step * ( xx+1 );
                    float y2 = step * ( yy+1 );
                    // calcula as coordendas para os cantos da grade atual da tesselação

                    resPontoos[patchIndex][0] = bezier(x1, y1, Pontos, patchIndices[patchIndex]);
                    resPontoos[patchIndex][1] = bezier(x1, y2, Pontos, patchIndices[patchIndex]);
                    resPontoos[patchIndex][2] = bezier(x2, y1, Pontos, patchIndices[patchIndex]);
                    resPontoos[patchIndex][3] = bezier(x2, y2, Pontos, patchIndices[patchIndex]);

                    Ponto p0(resPontoos[patchIndex][0][0], resPontoos[patchIndex][0][1], resPontoos[patchIndex][0][2]);
                    Ponto p1(resPontoos[patchIndex][1][0], resPontoos[patchIndex][1][1], resPontoos[patchIndex][1][2]);
                    Ponto p2(resPontoos[patchIndex][2][0], resPontoos[patchIndex][2][1], resPontoos[patchIndex][2][2]);
                    Ponto p3(resPontoos[patchIndex][3][0], resPontoos[patchIndex][3][1], resPontoos[patchIndex][3][2]);

                    Triangulo t1(p0, p2, p3);
                    Triangulo t2(p0, p3, p1);

                    triangulos.push_back(t1);
                    triangulos.push_back(t2);        
                }
            }
        }
        printToFile(triangulos, "file3d/" + bezier3d);
        triangulos.clear();
    }
}

// Main
int main(int argc, char** argv) {
    
    if (argc < 5) {
        printf("Argumentos Insuficientes\n");
        return 1;  
    }

    createXML();

    if (!strcmp(argv[1], "plane")) { 
        // ./generator plane 1 3 plane.3d
        generatePlane(atof(argv[2]), atof(argv[3]), argv[4]);
        updateXMLFile(argv[4]); // -> para adicionar plane.3d ao xml
    }
    else if (!strcmp(argv[1], "box")) { 
        // ./generator box 2 3 box.3d
        generateBox(atof(argv[2]), atof(argv[3]), argv[4]);
        updateXMLFile(argv[4]); // -> para adicionar box.3d ao xml  
    }
    else if (!strcmp(argv[1], "sphere")) { 
        // ./generator sphere 1 10 10 sphere.3d
        generateSphere(atof(argv[2]), atof(argv[3]), atof(argv[4]), argv[5]);
        updateXMLFile(argv[5]); // -> para adicionar sphere.3d ao xml
    }
    else if (!strcmp(argv[1], "cone")) { 
        // ./generator cone 1 2 4 3 cone.3d
        generateCone(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), argv[6]);
        updateXMLFile(argv[6]); // -> para adicionar cone.3d ao xml
    }
    else if (!strcmp(argv[1], "cylinder")) {
        // ./generator cylinder 1 2 3 cylinder.3d
        generateCylinder(atof(argv[2]), atof(argv[3]), atof(argv[4]), argv[5]);
        updateXMLFile(argv[5]); // -> para adicionar cylinder.3d ao xml
    }
    else if (!strcmp(argv[1], "torus")) {
        // ./generator torus 3 2 5 5 torus.3d
        generateTorus(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), argv[6]);
        updateXMLFile(argv[6]);
    }
    else if (!strcmp(argv[1], "bezier")) {
        // ./generator bezier (name of file) (requires tessellation level) bezier.3d
        generateBezier(argv[2], atoi(argv[3]), argv[4]);
        updateXMLFile(argv[4]);
    }
    else {
        printf("Comando desconhecido: %s\n", argv[1]);
        return 1;
    }

    return 0;
}