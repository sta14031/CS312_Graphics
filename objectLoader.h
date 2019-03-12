//#include "definitions.h"
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cstring>

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

Object& ReadFile(char* filename)
{
    std::ifstream fin;
    Object obj;

    fin.open(filename);
    if (fin.fail())
        return obj;

    Vertex tmp;
    char* line;
    double d;
    while (fin.is_open()) {
        fin.getline(line, 128);
        if(fin.eof()){
            fin.close();
        }
        else {

            //line = strtok(line, " ");
            if (line == NULL)
                break;
            if (line[0] == 'v') {
                if (line[1] == ' '){
                    line = strtok(line, " ");
                    line = strtok(NULL, " ");
                    tmp.x = atof(line);
                    line = strtok(NULL, " ");
                    tmp.y = atof(line);
                    line = strtok(NULL, " ");
                    tmp.z = atof(line);
                    obj.v.push_back(tmp);
                }
            }
        }
    }

    obj.t.push_back({1,2,3});
    return obj;
}

void ObjectLoader(Buffer2D<PIXEL> & target)
{
    
}

#endif // OBJ_LOADER_H