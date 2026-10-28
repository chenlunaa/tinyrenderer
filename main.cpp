#include <cmath>
#include <filesystem>
#include "geometry.h"
#include "tgaimage.h"

constexpr int width  = 128;
constexpr int height = 128;

constexpr TGAColor white   = {{255, 255, 255, 255}}; // attention, BGRA order
constexpr TGAColor green   = {{  0, 255,   0, 255}};
constexpr TGAColor red     = {{  0,   0, 255, 255}};
constexpr TGAColor blue    = {{255, 128,  64, 255}};
constexpr TGAColor yellow  = {{  0, 200, 255, 255}};

double sign_triangleArea(const Triangle& t) {
    return 0.5 * ((t.v1.y-t.v0.y)*(t.v1.x+t.v0.x) + (t.v2.y-t.v1.y)*(t.v2.x+t.v1.x) + (t.v0.y-t.v2.y)*(t.v0.x+t.v2.x));
}
std::tuple<double, double, double> barycentric2D(const Triangle& t, int x, int y) {
    double totalArea = sign_triangleArea(t);
    double alpha = sign_triangleArea({t.v1, t.v2, {x, y, 0}}) / totalArea;
    double beta  = sign_triangleArea({t.v2, t.v0, {x, y, 0}}) / totalArea;
    double gamma = sign_triangleArea({t.v0, t.v1, {x, y, 0}}) / totalArea;
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

void triangle(const Triangle& t, TGAImage& framebuffer) {
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
                TGAColor pixelColor = t.color[0] * aplha + t.color[1] * beta + t.color[2] * gamma;
                framebuffer.set(x, y, pixelColor);
            }
        }
    }
}

int main(int argc, char** argv) {
    
    TGAImage framebuffer(width, height, TGAImage::RGB);
    Triangle t1 = {{10, 70, 0}, {50, 120, 0}, {70, 80, 0}, {red, green, blue}};
    triangle(t1, framebuffer);

    const std::filesystem::path outputPath = std::filesystem::path(__FILE__).parent_path() / "framebuffer.tga";
    framebuffer.write_tga_file(outputPath.string());
    return 0;
}

