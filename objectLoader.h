//#include "definitions.h"
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

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
    Object() {}
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
    std::string line;
    double d[3];
    while (fin.is_open()) {
        std::cerr << line << std::endl;
        getline(fin, line);
        if(fin.eof()){
            fin.close();
        }
        else {
            if (line.empty())
                break;
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

                    tmp.x = d[0];
                    tmp.y = d[1];
                    tmp.z = d[2];
                    tmp.w = 1;
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