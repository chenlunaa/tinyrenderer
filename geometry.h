#include <string>
#include <vector>
#include <limits>
#include "tgaimage.h"

struct Vec3f {
    // int会被截断
    float x, y, z;
};

struct Vec3i {
    int x, y, z;
    Vec3i(int x, int y, int z) : x(x), y(y), z(z) {}
};

struct Triangle {
    Vec3i v0, v1, v2;
    TGAColor color[3];
    Triangle(const Vec3i& v0, const Vec3i& v1, const Vec3i& v2) : v0(v0), v1(v1), v2(v2) {}
    Triangle(const Vec3i& v0, const Vec3i& v1, const Vec3i& v2, const TGAColor& c0, const TGAColor& c1, const TGAColor& c2) 
        : v0(v0), v1(v1), v2(v2) {
        color[0] = c0;
        color[1] = c1;
        color[2] = c2;
    }
};

class Model {
public:
    // 顶点坐标
    std::vector<Vec3f> vertices;
    std::vector<std::vector<int>> faces;
    Model() = default;
    ~Model() = default;
    void parseObjFile(const std::string& filename);
};

struct ZBuffer {
    int w, h;
    std::vector<float> data;

    ZBuffer(int w, int h)
        : w(w), h(h),
          data(static_cast<size_t>(w) * h, -std::numeric_limits<float>::max()) {}

    float& at(int x, int y)       { return data[static_cast<size_t>(y) * w + x]; }
    float  at(int x, int y) const { return data[static_cast<size_t>(y) * w + x]; }

    void clear() { std::fill(data.begin(), data.end(),
                             -std::numeric_limits<float>::max()); }
};
