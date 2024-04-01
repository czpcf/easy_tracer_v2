#include "texture_simple.hpp"

TextureSimple::TextureSimple() {
}

TextureSimple::TextureSimple(const Vec3 &c) {
    color = c;
}

Vec3 TextureSimple::get(const UV &uv) {
    return color;
}