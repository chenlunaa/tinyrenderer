#pragma once
#include <cstdint>
#include <fstream>
#include <vector>
#include <algorithm>

#pragma pack(push,1)
struct TGAHeader {
    std::uint8_t  idlength = 0;
    std::uint8_t  colormaptype = 0;
    std::uint8_t  datatypecode = 0;
    std::uint16_t colormaporigin = 0;
    std::uint16_t colormaplength = 0;
    std::uint8_t  colormapdepth = 0;
    std::uint16_t x_origin = 0;
    std::uint16_t y_origin = 0;
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::uint8_t  bitsperpixel = 0;
    std::uint8_t  imagedescriptor = 0;
};
#pragma pack(pop)

// 聚合体|没有用户声明的构造函数|没有私有或受保护的非静态数据成员|没有基类，也没有虚函数
struct TGAColor {
    std::uint8_t bgra[4] = {0,0,0,0};
    std::uint8_t bytespp = 4;
    // 聚合初始化
    std::uint8_t& operator[](const int i) { return bgra[i]; }

    TGAColor operator+(const TGAColor& other) const{
        TGAColor out = *this;
        for (int i=0; i<3; i++) {
            auto value = out.bgra[i] + other.bgra[i];
            auto color = static_cast<uint8_t>(std::clamp(value, 0, 255));
            out.bgra[i] = color;
        }
        return out;
    }

    TGAColor operator*(double intensity) const{
        TGAColor out = *this;
        for (int i=0; i<3; i++) {
            double value = bgra[i] * intensity;
            auto color = static_cast<uint8_t>(std::clamp(value, 0.0, 255.0));
            out.bgra[i] = color;
        }
        return out;
    }
};

struct TGAImage {
    enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };
    TGAImage() = default;
    TGAImage(const int w, const int h, const int bpp);
    bool  read_tga_file(const std::string filename);
    bool write_tga_file(const std::string filename, const bool vflip=true, const bool rle=true) const;
    void flip_horizontally();
    void flip_vertically();
    TGAColor get(const int x, const int y) const;
    void set(const int x, const int y, const TGAColor &c);
    int width()  const;
    int height() const;
private:
    bool   load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;
    int w = 0, h = 0;
    std::uint8_t bpp = 0;
    std::vector<std::uint8_t> data = {};
};

