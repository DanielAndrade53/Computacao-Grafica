#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <cstring>
#include <tuple>
#include <vector>

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

// Classe Ponto e construtores
class Ponto {

    double x;
    double y;
    double z;

public:

    Ponto() {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Ponto(double cx, double cy, double cz) {
        x = cx;
        y = cy;
        z = cz;
    }

    string pointToString() const {
        return to_string(x) + ", " + to_string(y) + ", " + to_string(z);
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

    void printTriangulo(ofstream& file) const {
        file << get<0>(triangulo).pointToString() << endl;   // Primeiro Vertice
        file << get<1>(triangulo).pointToString() << endl;   // Segundo Vertice
        file << get<2>(triangulo).pointToString() << endl;   // Terceiro Vertice
    }

};

void createXML() {
  
    if(!doc.LoadFile(filename) == XML_SUCCESS) {

    XMLElement* world = doc.NewElement("World");
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

        XMLElement* worldElement = doc.FirstChildElement("World");

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

void printToFile(const vector<Triangulo>& v, const string& f) {
    ofstream file(f, ios::out);
    if (file.is_open()) {
        file << v.size() << endl;
        for (const auto& triangulo : v) {
            triangulo.printTriangulo(file);
        }
        file.close();
    } else {
        cout << "Não foi possível abrir o ficheiro: " << f << endl;
    }
}

void generatePlane(float dimension, int divisions, string plane3d) {

    vector <Triangulo> triangulos;

    float squareHeight = dimension / divisions;
    float halfDimension = dimension / 2.0f;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {

            float x = i * squareHeight - halfDimension;
            float z = j * squareHeight - halfDimension;

            // Primeiro Triangulo
            Ponto p1(x, 0.0f, z);
            Ponto p2(x, 0.0f, z + squareHeight);
            Ponto p3(x + squareHeight, 0.0f, z);
            // Segundo Triangulo
            Ponto p4(x + squareHeight, 0.0f, z);
            Ponto p5(x, 0.0f, z + squareHeight);
            Ponto p6(x + squareHeight, 0.0f, z + squareHeight);

            // Primeiro Triangulo
            Ponto p7(x, 0.0f, z);
            Ponto p8(x + squareHeight, 0.0f, z);
            Ponto p9(x, 0.0f, z + squareHeight);
            // Segundo Triangulo
            Ponto p10(x + squareHeight, 0.0f, z);
            Ponto p11(x + squareHeight, 0.0f, z + squareHeight);
            Ponto p12(x, 0.0f, z + squareHeight);

            Triangulo t1(p1,p2,p3);
            Triangulo t2(p4,p5,p6);
            Triangulo t3(p7,p8,p9);
            Triangulo t4(p10,p11,p12);

            triangulos.push_back(t1);
            triangulos.push_back(t2);
            triangulos.push_back(t3);
            triangulos.push_back(t4);

        }
    }
    printToFile(triangulos, "file3d/" + plane3d);
    triangulos.clear();
}

// generate Box
void generateBox(float dimension, int divisions, string box3d) {

    vector <Triangulo> triangulos;

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

            triangulos.push_back(t1);
            triangulos.push_back(t2);
            triangulos.push_back(t3);
            triangulos.push_back(t4);
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

            triangulos.push_back(t1);
            triangulos.push_back(t2);
            triangulos.push_back(t3);
            triangulos.push_back(t4);   
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

            triangulos.push_back(t1);
            triangulos.push_back(t2);
            triangulos.push_back(t3);
            triangulos.push_back(t4);            
        }
    }
    printToFile(triangulos, "file3d/" + box3d);
    triangulos.clear();
}

// generate sphere
void generateSphere(float baseRadius, int slices, int stacks, string sphere3d) {
    
    vector<Triangulo> triangulos;

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            float alpha1 = 2 * M_PI / slices * i;
            float alpha2 = 2 * M_PI / slices * (i + 1);
            float beta1 = M_PI / stacks * j;
            float beta2 = M_PI / stacks * (j + 1);
            // Primeiro Triangulo
            Ponto p1(baseRadius * sin(beta1) * cos(alpha1), baseRadius * cos(beta1), baseRadius * sin(beta1) * sin(alpha1));
            Ponto p2(baseRadius * sin(beta1) * cos(alpha2), baseRadius * cos(beta1), baseRadius * sin(beta1) * sin(alpha2));
            Ponto p3(baseRadius * sin(beta2) * cos(alpha1), baseRadius * cos(beta2), baseRadius * sin(beta2) * sin(alpha1));
            // Segundo Triangulo
            Ponto p4(baseRadius * sin(beta2) * cos(alpha1), baseRadius * cos(beta2), baseRadius * sin(beta2) * sin(alpha1));
            Ponto p5(baseRadius * sin(beta1) * cos(alpha2), baseRadius * cos(beta1), baseRadius * sin(beta1) * sin(alpha2));
            Ponto p6(baseRadius * sin(beta2) * cos(alpha2), baseRadius * cos(beta2), baseRadius * sin(beta2) * sin(alpha2));

            Triangulo t1(p1,p2,p3);
            Triangulo t2(p4,p5,p6);

            triangulos.push_back(t1);
            triangulos.push_back(t2); 
        } 
    }
    printToFile(triangulos, "file3d/" + sphere3d);
    triangulos.clear();
}

// generate cone
void generateCone(double baseRadius , double height, double slices, double stacks, string cone3d) {

    vector <Triangulo> triangulos;
    
    double alpha = (2 * M_PI) / slices;
    double beta = height / stacks;
    double i, j;
    double angle, raio1, raio2, frstPoint, scndPoint;

    for (i = 0; i < slices; i++) {
        angle = alpha * i;
        Ponto p1;  // ou Ponto p1(0.0f, 0.0f, 0.0f);
        Ponto p2(baseRadius * sin(angle + alpha), 0.0f, baseRadius * cos(angle + alpha));
        Ponto p3(baseRadius * sin(angle), 0.0f, baseRadius * cos(angle));

        Triangulo t1(p1,p2,p3);
        triangulos.push_back(t1);
    }


    for (i = 0; i < stacks; i++) {
        scndPoint = i * beta;
        frstPoint = (i + 1) * beta;

        raio2 = baseRadius - ((baseRadius / stacks) * i);
        raio1 = baseRadius - ((baseRadius / stacks) * (i + 1));

        for (j = 0; j < slices; j++) {
            height = alpha * j;

            Ponto p1(raio2 * sin(height), scndPoint, raio2 * cos(height));
            Ponto p2(raio1 * sin(height + alpha), frstPoint, raio1 * cos(height + alpha));
            Ponto p3(raio1 * sin(height), frstPoint, raio1 * cos(height));

            Ponto p4(raio2 * sin(height), scndPoint, raio2 * cos(height));
            Ponto p5(raio2 * sin(height + alpha), scndPoint, raio2 * cos(height + alpha));
            Ponto p6(raio1 * sin(height + alpha), frstPoint, raio1 * cos(height + alpha));

            Triangulo t1(p1,p2,p3);
            Triangulo t2(p4,p5,p6);

            triangulos.push_back(t1);
            triangulos.push_back(t2);

        }
    }
    printToFile(triangulos, "file3d/" + cone3d);
    triangulos.clear();
}

void generateCylinder(float baseRadius, float height, int slices, string cylinder3d) {
    
    vector<Triangulo> triangulos;
    float halfHeight = height / 2;

    for (int i = 0; i < slices; ++i) {
        float alpha1 = 2.0f * M_PI / slices * i;
        float alpha2 = 2.0f * M_PI / slices * (i + 1);

        // Bottom triangle
        Ponto p1(0.0f, -halfHeight, 0.0f);  // Centro do círculo no plano XY
        Ponto p2(baseRadius * cos(alpha1), -halfHeight, baseRadius * sin(alpha1));
        Ponto p3(baseRadius * cos(alpha2), -halfHeight, baseRadius * sin(alpha2));

        // Top triangle
        Ponto p4(0.0f, halfHeight, 0.0f);  // Centro do círculo no plano XY
        Ponto p5(baseRadius * cos(alpha2), halfHeight, baseRadius * sin(alpha2));
        Ponto p6(baseRadius * cos(alpha1), halfHeight, baseRadius * sin(alpha1));

        // Side triangles
        Ponto p7(baseRadius * cos(alpha1), -halfHeight, baseRadius * sin(alpha1));
        Ponto p8(baseRadius * cos(alpha1), halfHeight, baseRadius * sin(alpha1));
        Ponto p9(baseRadius * cos(alpha2), -halfHeight, baseRadius * sin(alpha2));
        
        Ponto p10(baseRadius * cos(alpha2), -halfHeight, baseRadius * sin(alpha2));
        Ponto p11(baseRadius * cos(alpha1), halfHeight, baseRadius * sin(alpha1));
        Ponto p12(baseRadius * cos(alpha2), halfHeight, baseRadius * sin(alpha2));

        Triangulo t1(p1,p2,p3);
        Triangulo t2(p4,p5,p6);
        Triangulo t3(p7,p8,p9);
        Triangulo t4(p10,p11,p12);

        triangulos.push_back(t1);
        triangulos.push_back(t2);
        triangulos.push_back(t3);
        triangulos.push_back(t4);        

    }
    printToFile(triangulos, "file3d/" + cylinder3d);
    triangulos.clear();
}

void generateTorus(float outterRadius, float innerRadius, float slices, float stacks, string torus3d) {

    vector<Triangulo> triangulos;

    float arch_alpha = (2 * M_PI) / stacks, arch_beta = (2 * M_PI) / slices;
    outterRadius = (outterRadius + innerRadius) / 2;
    innerRadius = outterRadius - innerRadius;

    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {

            Ponto p1((outterRadius + innerRadius * cos(arch_alpha * i)) * cos(arch_beta * j),
                    innerRadius * sin(arch_alpha * i),
                    (outterRadius + innerRadius * cos(arch_alpha * i)) * sin(arch_beta * j));

            Ponto p2((outterRadius + innerRadius * cos(arch_alpha * (i+1))) * cos(arch_beta * j),
                    innerRadius * sin(arch_alpha * (i+1)),
                    (outterRadius + innerRadius * cos(arch_alpha * (i+1))) * sin(arch_beta * j));

            Ponto p3((outterRadius + innerRadius * cos(arch_alpha * (i+1))) * cos(arch_beta * (j+1)),
                    innerRadius * sin(arch_alpha * (i+1)),
                    (outterRadius + innerRadius * cos(arch_alpha * (i+1))) * sin(arch_beta * (j+1)));

            Ponto p4((outterRadius + innerRadius * cos(arch_alpha * i)) * cos(arch_beta * (j+1)),
                    innerRadius * sin(arch_alpha * i),
                    (outterRadius + innerRadius * cos(arch_alpha * i)) * sin(arch_beta * (j+1)));

            Triangulo t1(p1, p2, p3);
            Triangulo t2(p1, p3, p4);

            triangulos.push_back(t1);
            triangulos.push_back(t2);        
        }
    }

    printToFile(triangulos, "file3d/" + torus3d);
    triangulos.clear();
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
    else {
        printf("Comando desconhecido: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
