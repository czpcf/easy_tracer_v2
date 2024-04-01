/*
texture
*/
#ifndef TEXTURE_SIMPLE_H
#define TEXTURE_SIMPLE_H

#include "texture.hpp"

class TextureSimple: Texture {
public:
    TextureSimple();
    TextureSimple(const Vec3 &);
    ~TextureSimple() override = default;

    /// @brief get color using uv coordinate
    Vec3 get(const UV &uv) override;

private:
    Vec3 color;
};
#endif