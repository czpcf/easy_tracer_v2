/*
texture
*/
#ifndef TEXTURE_H
#define TEXTURE_H

#include "../tensor/vec4.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/vec2.hpp"

class Texture {
public:
    Texture();
    virtual ~Texture() = default;

    /// @brief get color using uv coordinate
    virtual Vec3 get(const UV &uv);
};
#endif