#ifndef IMAGE_H
#define IMAGE_H

#include <cmath>
#include <cassert>
#include "../tensor/vec3.hpp"


// Simple image class
class Image {

public:

    Image(int w, int h) {
        _width = w;
        _height = h;
        data = new Vec3[_width * _height];
    }

    ~Image() {
        delete[] data;
    }

    int width() const {
        return _width;
    }

    int height() const {
        return _height;
    }

    const Vec3 &get_pixel(int x, int y) const {
        assert(x >= 0 && x < _width);
        assert(y >= 0 && y < _height);
        return data[y * _width + x];
    }

    void set_all_pixels(const Vec3 &color) {
        for (int i = 0; i < _width * _height; ++i) {
            data[i] = color;
        }
    }

    void set_pixel(int x, int y, const Vec3 &color) {
        assert(x >= 0 && x < _width);
        assert(y >= 0 && y < _height);
        data[y * _width + x] = color;
    }

    void add_pixel(int x, int y, const Vec3 &color) {
        assert(x >= 0 && x < _width);
        assert(y >= 0 && y < _height);
        data[y * _width + x] += color;
    }

    void divide_by(float x) {
        for(int i = 0; i < _width * _height; ++i) {
            data[i] /= x;
        }
    }

    void linear_to_srgb() {
        auto f = [](float x) {
            if(x <= 0.0031308f) {
                return 12.92f * x;
            }
            return 1.055f * std::pow(x, 1.0f / 2.4f) - 0.055f;
        };
        for(int i = 0; i < _width * _height; ++i) {
            data[i] = Vec3(f(data[i].x), f(data[i].y), f(data[i].z));
        }
    }

    static Image *load_ppm(const char *filename);

    void save_ppm(const char *filename) const;

    static Image *load_tga(const char *filename);

    void save_tga(const char *filename) const;

    int save_bmp(const char *filename);

    void save_image(const char *filename);

private:

    int _width;
    int _height;
    Vec3 *data;

};

#endif // IMAGE_H
