#include "resource_mesh.hpp"

ResourceTriangle::ResourceTriangle() {
}

ResourceTriangle::ResourceTriangle(
        const Vec3 &n1, const Vec3 &n2, const Vec3 &n3,
        const Vec2 &u1, const Vec2 &u2, const Vec2 &u3,
        Sampler *s,
        Bxdf *b,
        Texture *t):
        norm1(n1), norm2(n2), norm3(n3),
        uv1(u1), uv2(u2), uv3(u3),
        sampler(s),
        bxdf(b),
        texture(t) {}

UV ResourceTriangle::local_to_uv(const Vec2 &local) {
    float t1 = local.x;
    float t3 = local.y;
    float t2 = 1.0f - t1 - t3;
    return uv1 * t1 + uv2 * t2 + uv3 * t3;
}

Vec3 ResourceTriangle::get_color(const Vec2 &local, const UV &uv) {
    return texture->get(uv);
}

Vec3 ResourceTriangle::get_normal(const Vec2 &local, const UV &uv) {
    float t1 = local.x;
    float t3 = local.y;
    float t2 = 1.0f - t1 - t3;
    return (norm1 * t1 + norm2 * t2 + norm3).norm();
}

Bxdf *ResourceTriangle::get_bxdf(const Vec2 &local, const UV &uv) {
    return bxdf;
}

Sampler *ResourceTriangle::get_sampler(const Vec2 &local, const UV &uv) {
    return sampler;
}



ResourceGroupMesh::ResourceGroupMesh() {
}

void ResourceGroupMesh::set_size(int n) {
    triangles.reserve(n);
    infos.reserve(n);
}

int ResourceGroupMesh::n_objects() {
    return triangles.size();
}

Geometry* ResourceGroupMesh::get_shape(int n) {
    return &triangles[n];
}

Resource* ResourceGroupMesh::get_info(int n) {
    return &infos[n];
}

void ResourceGroupMesh::add(const Triangle& t, const ResourceTriangle &info) {
    triangles.push_back(t);
    infos.push_back(info);
}