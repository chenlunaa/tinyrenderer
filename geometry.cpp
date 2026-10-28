#include "geometry.h"
#include <iostream>
#include <fstream>
#include <algorithm>
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

void ZBuffer::SaveZbuffer(int width, int height, const std::string filename)
{
    TGAImage image(width, height, TGAImage::GRAYSCALE);

    // 1. 找有效深度的范围（跳过 -FLT_MAX 这种背景值）
    float zmin =  std::numeric_limits<float>::max();
    float zmax = -std::numeric_limits<float>::max();
    for (float z : data) {
        if (z == -std::numeric_limits<float>::max()) continue;  // 未写入
        zmin = std::min(zmin, z);
        zmax = std::max(zmax, z);
    }

    if (zmin > zmax) return;   // 全空，没东西画

    float range = (zmax - zmin > 1e-8f) ? (zmax - zmin) : 1.0f;

    // 2. 映射到 0~255
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float z = data[x + y * width];
            if (z == -std::numeric_limits<float>::max()) {
                image.set(x, y, TGAColor{{0, 0, 0, 255}});       // 背景黑
            } else {
                // 越近越亮
                unsigned char v = static_cast<unsigned char>(
                    255.0f * (z - zmin) / range);
                image.set(x, y, TGAColor{{v, v, v, 255}});
            }
        }
    }

    image.write_tga_file(filename);
}
