#include "geometry.h"
#include <iostream>
#include <fstream>
#include <sstream>

void Model::parseObjFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件:" << filename << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        // 按照空格分割 f 100/101/102分割成空格后面的字符串
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix; // 每行的第一个字符

        if (prefix == "v") {
            Vec3f v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }else if (prefix == "f") {
            std::vector<int> faceIndices;
            std::string vertexStr;

            for (int i = 0; i < 3; i++) {
                iss >> vertexStr;
                size_t slashPos = vertexStr.find('/');
                std::string indexStr = vertexStr.substr(0, slashPos);

                int vertexIndex = std::stoi(indexStr) - 1;
                faceIndices.push_back(vertexIndex);
            }
            faces.push_back(faceIndices);
        }
    }
    file.close();
}
