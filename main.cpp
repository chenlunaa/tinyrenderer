#include <cmath>
#include <filesystem>
#include "geometry.h"
#include "tgaimage.h"

constexpr int width  = 800;
constexpr int height = 800;

// constexpr TGAColor white   = {{255, 255, 255, 255}}; // attention, BGRA order
// constexpr TGAColor green   = {{  0, 255,   0, 255}};
// constexpr TGAColor red     = {{  0,   0, 255, 255}};
// constexpr TGAColor blue    = {{255, 128,  64, 255}};
// constexpr TGAColor yellow  = {{  0, 200, 255, 255}};

double signedArea(const Vec3i& a, const Vec3i& b, const Vec3i& c) {
    double ax = a.x, ay = a.y;
    double bx = b.x, by = b.y;
    double cx = c.x, cy = c.y;
    return 0.5 * ((by - ay) * (bx + ax)
                + (cy - by) * (cx + bx)
                + (ay - cy) * (ax + cx));
}

double signedArea(const Triangle& t) {
    return signedArea(t.v0, t.v1, t.v2);
}

std::tuple<double, double, double> barycentric2D(const Triangle& t, int x, int y) {
    double totalArea = signedArea(t);
    Vec3i p{x, y, 0};
    if (std::abs(totalArea) < 1e-12) return {-1.0, -1.0, -1.0};
    double alpha = signedArea(t.v1, t.v2, p) / totalArea;
    double beta  = signedArea(t.v2, t.v0, p) / totalArea;
    double gamma = signedArea(t.v0, t.v1, p) / totalArea;
    return {alpha, beta, gamma};
}

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color) {
    bool bIsRange = (ax >= 0 && ax < width && ay >= 0 && ay < height) && (bx >= 0 && bx < width && by >= 0 && by < height);
    if (!bIsRange) {
        return;
    }
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    // 保证输入的顺序
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    // 按照x的线粒度来填充
    for (int x = ax; x < bx; x++) {
        float t = (x - ax) / static_cast<float>(bx - ax);
        int y = std::round(ay + (by - ay)*t);
        if (steep) {
            framebuffer.set(y, x, color);
        }else {
            framebuffer.set(x, y, color);
        }
    }
}

void triangle(const Triangle& t, TGAImage& framebuffer, ZBuffer& zbuffer) {
   int bbminx = std::min(std::min(t.v0.x, t.v1.x), t.v2.x);
   int bbminy = std::min(std::min(t.v0.y, t.v1.y), t.v2.y);
   int bbmaxx = std::max(std::max(t.v0.x, t.v1.x), t.v2.x);
   int bbmaxy = std::max(std::max(t.v0.y, t.v1.y), t.v2.y);

   // 遍历包围盒内的每个像素点，判断是否在三角形内,使用叉乘
#pragma omp parallel for
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            auto[aplha, beta, gamma] = barycentric2D(t, x, y);
            if (aplha < 0 || beta < 0 || gamma < 0) {
                continue;
            }else {
                float z = aplha * t.v0.z + beta * t.v1.z + gamma * t.v2.z;
                if (zbuffer.at(x, y) < z) {
                    TGAColor pixelColor = t.color[0] * aplha + t.color[1] * beta + t.color[2] * gamma;
                    framebuffer.set(x, y, pixelColor);
                    zbuffer.at(x, y) = z;
                }
            }
        }
    }
}

// [-1, 1] -> [0, width], [0, height]
std::tuple<int, int, int> projectToScreen(Vec3f v) {
    int x = static_cast<int>((v.x + 1.0f) * width / 2.0f);
    int y = static_cast<int>((v.y + 1.0f) * height / 2.0f);
    int z = static_cast<int>((v.z + 1.0f) * 255.0f / 2.0f);
    return {x, y, z};
}

int main(int argc, char** argv) {
    
    TGAImage framebuffer(width, height, TGAImage::RGB);
    ZBuffer zbuffer(width, height);
    Model model;
    model.parseObjFile("D:\\Games101\\tinyrenderer\\obj\\diablo3_pose\\diablo3_pose.obj");
    for (auto face : model.faces) {
        Vec3f a = model.vertices[face[0]];
        Vec3f b = model.vertices[face[1]];
        Vec3f c = model.vertices[face[2]];
        auto [ax, ay, az] = projectToScreen(a);
        auto [bx, by, bz] = projectToScreen(b);
        auto [cx, cy, cz] = projectToScreen(c);
        TGAColor rnd[3];
        for (int i = 0; i < 3; i++) {
            for (int c = 0; c < 3; c++) {
                rnd[i][c] = static_cast<uint8_t>(std::rand() % 255);
            }
        }
        Triangle t({ax, ay, az}, {bx, by, bz}, {cx, cy, cz}, rnd[0], rnd[1], rnd[2]);
        triangle(t, framebuffer, zbuffer);
    }

    const std::filesystem::path outputPath = std::filesystem::path(__FILE__).parent_path() / "framebuffer.tga";
    framebuffer.write_tga_file(outputPath.string());
    return 0;
}

