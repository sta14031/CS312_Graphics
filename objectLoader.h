#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h> // atoi, atof
#include <sstream> // stringstream (tokenize)

#include "definitions.h"
#include "matrix.h"

#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

struct TriDat
{
    int i1;
    int i2;
    int i3;
};

class Object
{
    public:
    std::vector<Vertex> v;
    std::vector<TriDat> t;
};

// This must be returned by copy, returning by reference causes dangling pointers in the vectors in obj.
Object ReadFile(char* filename)
{
    std::ifstream fin;
    Object obj = Object();

    fin.open(filename);
    if (fin.fail())
        return obj;

    Vertex tmpV;
    TriDat tmpT;
    std::string line;
    double d[3];
    int ints[4];
    while (fin.is_open()) {
        getline(fin, line);
        if(fin.eof()){
            fin.close();
        }
        else {
            if (line.empty())
                break;
            // Get vertices first
            if (line.at(0) == 'v') {
                if (line.at(1) == ' '){
                    std::stringstream ss(line);
                    std::string word;
                    int i = -1;
                    while (getline(ss, word, ' '))
                    {
                        if (i ==-1) {
                            i++;
                            continue;
                        }
                        // Get the token
                        d[i++] = atof(word.c_str());
                    }

                    tmpV.x = d[0];
                    tmpV.y = d[1];
                    tmpV.z = d[2];
                    tmpV.w = 1;
                    obj.v.push_back(tmpV);
                }
                else if (line.at(1) == 't') {
                    // Texture mapping (we can ignore this)
                }
                else if (line.at(1) == 'n') {
                    // Normals (we can also ignore this)
                }
            }
            else if (line.at(0) == 'f') {
                // Faces (triangles)
                /*
                    Faces in our format are given as quads; that is, groups of 4
                    vertices. They are given in the format v/vt/vn where v is the
                    index of the vertex and the other two numbers we throw away.

                    Since each face is defined by four vertices, we must draw two
                    triangles: one for 0,1,2 and one for 2,3,0.
                */
               if (line.at(1) == ' '){
                    std::stringstream ss(line);
                    std::string word;
                    int i = -1;
                    while (getline(ss, word, ' '))
                    {
                        if (i ==-1) {
                            i++;
                            continue;
                        }
                        // Get the token
                        ints[i++] = atoi(word.c_str());
                    }

                    tmpT.i1 = ints[2];
                    tmpT.i2 = ints[1];
                    tmpT.i3 = ints[0];
                    obj.t.push_back(tmpT);
                    tmpT.i1 = ints[0];
                    tmpT.i2 = ints[3];
                    tmpT.i3 = ints[2];
                    obj.t.push_back(tmpT);
                }
            }
        }
    }

    return obj;
}

void ObjectLoader(Buffer2D<PIXEL> & target, Buffer2D<double> & zBuf, Object obj)
{
    Vertex triangle[3];
    Attributes attrs[3];

    attrs[0].insertDbl(1.0); // r
    attrs[0].insertDbl(0.0); // g
    attrs[0].insertDbl(0.0); // b
    attrs[1].insertDbl(0.0); // r
    attrs[1].insertDbl(1.0); // g
    attrs[1].insertDbl(0.0); // b
    attrs[2].insertDbl(0.0); // r
    attrs[2].insertDbl(0.0); // g
    attrs[2].insertDbl(1.0); // b

    Attributes uniforms;

    Matrix model = scaleMatrix(0.5);
    Matrix view  = viewTransform(myCam.x, myCam.y, myCam.z,
                                    myCam.yaw, myCam.pitch, myCam.roll);
    Matrix proj  = perspectiveTransform(80.0, 1.0, 1, 200); // FOV, Aspect ratio, Near, Far
    
    uniforms.insertPtr(NULL); // image data (not used at the moment)
    uniforms.insertPtr(&model);
    uniforms.insertPtr(&view);
    uniforms.insertPtr(&proj);

    FragmentShader myFragShader;
    myFragShader.FragShader = &ColorFragShader;
    VertexShader myVertShader;
    myVertShader.VertShader = &MVPVertexShader;

    std::vector<TriDat>::iterator it;

    for (it = obj.t.begin(); it != obj.t.end(); it++) {
        // Render every triangle
        triangle[0] = obj.v[it->i1-1];
        triangle[1] = obj.v[it->i2-1];
        triangle[2] = obj.v[it->i3-1];

        DrawPrimitive(
            TRIANGLE,
            target,
            triangle,
            attrs,
            &uniforms,
            &myFragShader,
            &myVertShader,
            &zBuf);
    }
}

#endif // OBJ_LOADER_H