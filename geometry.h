#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct Vec3f {
    // int会被截断
    float x, y, z;
};

class Model {
public:
    std::vector<Vec3f> vertices;
    std::vector<std::vector<int>> faces;
    Model() = default;
    ~Model() = default;
    void parseObjFile(const std::string& filename);
};
