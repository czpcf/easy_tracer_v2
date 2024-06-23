/*
texture color
*/
#ifndef TEXTURE_COLOR_H
#define TEXTURE_COLOR_H

#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION

class TextureColor: public Texture {
public:
    TextureColor();
    TextureColor(const char *path);
    ~TextureColor() override = default;

    /// @brief get color using uv coordinate
    Vec3 get(const UV &uv) override;

private:
    int width;
    int height;
    Vec3 *data;
};
#endif