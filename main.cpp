#include <cmath>
#include "geometry.h"
#include "tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color) {
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

int main(int argc, char** argv) {
    constexpr int width  = 1200;
    constexpr int height = 1200;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    // int ax =  7, ay =  3;
    // int bx = 12, by = 37;
    // int cx = 62, cy = 53;

    // line(ax, ay, bx, by, framebuffer, red);
    // line(cx, cy, bx, by, framebuffer, green);
    // line(cx, cy, ax, ay, framebuffer, blue);

    // framebuffer.set(ax, ay, white);
    // framebuffer.set(bx, by, white);
    // framebuffer.set(cx, cy, white);

    Model* model = new Model(); 
    model->parseObjFile("D:\\Games101\\tinyrenderer\\obj\\diablo3_pose\\diablo3_pose.obj");

    for (auto face : model->faces) {
        int idx0 = face[0];
        int idx1 = face[1];
        int idx2 = face[2];

        Vec3f v0 = model->vertices[idx0];
        Vec3f v1 = model->vertices[idx1];
        Vec3f v2 = model->vertices[idx2];

        int screen_x0 = (v0.x + 1.0) * width / 2.0;
        int screen_y0 = (v0.y + 1.0) * height / 2.0;

        int screen_x1 = (v1.x + 1.0) * width / 2.0;
        int screen_y1 = (v1.y + 1.0) * height / 2.0;

        int screen_x2 = (v2.x + 1.0) * width / 2.0;
        int screen_y2 = (v2.y + 1.0) * height / 2.0;
        
        line(screen_x0, screen_y0, screen_x1, screen_y1, framebuffer, red);
        line(screen_x1, screen_y1, screen_x2, screen_y2, framebuffer, red);
        line(screen_x2, screen_y2, screen_x0, screen_y0, framebuffer, red);

        framebuffer.set(screen_x0, screen_y0, white);
        framebuffer.set(screen_x1, screen_y1, white);
        framebuffer.set(screen_x2, screen_y2, white);

    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

