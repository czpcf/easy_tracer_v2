#include "texture_color.hpp"
#include "stbi_image.h"
#include <stdlib.h>

TextureColor::TextureColor() {
}

TextureColor::TextureColor(const char *path) {
    int channels;
    int img_width, img_height;
    unsigned char *buf = stbi_load(path, &img_width, &img_height, &channels, STBI_rgb_alpha);
    width = img_width;
    height = img_height;
    data = new Vec3[width * height];
    if(channels == 4) {
        // alpha value is discarded
        for(int i = 0; i < width * height; ++i) {
            data[i] = Vec3(
                float(buf[i * 4 + 0]) / 255,
                float(buf[i * 4 + 1]) / 255,
                float(buf[i * 4 + 2]) / 255
            );
        }
    } else if(channels == 3) {
        for(int i = 0; i < width * height; ++i) {
            data[i] = Vec3(
                float(buf[i * 4 + 0]) / 255,
                float(buf[i * 4 + 1]) / 255,
                float(buf[i * 4 + 2]) / 255
            );
        }
    } else if(channels == 1) {
        for(int i = 0; i < width * height; ++i) {
            data[i] = Vec3(
                float(buf[i * 4 + 0]) / 255,
                float(buf[i * 4 + 0]) / 255,
                float(buf[i * 4 + 0]) / 255
            );
        }
    } else {
        fprintf(stderr, "Image %s is weired.\n", path);
        exit(1);
    }
}


static unsigned int min(unsigned int x, unsigned int y) {
    return x < y ? x : y;
}

static unsigned int max(unsigned int x, unsigned int y) {
    return x > y ? x : y;
}

Vec3 TextureColor::get(const UV &uv) {
    unsigned int x = min((unsigned int)width - 1, (unsigned int)(uv.x * width));
    unsigned int y = min((unsigned int)height - 1, (unsigned int)(uv.y * height));
    return data[y * width + x];
}