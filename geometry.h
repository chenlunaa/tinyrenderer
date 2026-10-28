#include <string>
#include <vector>
#include "tgaimage.h"

struct Vec3f {
    // int会被截断
    float x, y, z;
};

struct Vec3i {
    int x, y, z;
};

struct Triangle {
    Vec3i v0, v1, v2;
    TGAColor color[3];
};

class Model {
public:
    std::vector<Vec3f> vertices;
    std::vector<std::vector<int>> faces;
    Model() = default;
    ~Model() = default;
    void parseObjFile(const std::string& filename);
};
