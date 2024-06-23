#include "resource_mesh.hpp"
#include "tiny_obj_loader.h"


#include <map>

ResourceTriangle::ResourceTriangle() {
}

ResourceTriangle::ResourceTriangle(
    Triangle *_triangle,
    const Vec3 &n1, const Vec3 &n2, const Vec3 &n3,
    const Vec2 &u1, const Vec2 &u2, const Vec2 &u3,
    Sampler *s,
    Bxdf *b,
    Texture *t):
    triangle(_triangle),
    norm1(n1), norm2(n2), norm3(n3),
    uv1(u1), uv2(u2), uv3(u3),
    sampler(s),
    bxdf(b),
    texture(t) {}

UV ResourceTriangle::local_to_uv(const Vec2 &local) {
    UV uv = Triangle::interpolate_uv(uv1, uv2, uv3, local);
    return Triangle::interpolate_uv(uv1, uv2, uv3, local);
}

Surface ResourceTriangle::get_surface(const Vec2 &local) {
    Vec3 norm = triangle->get_norm();
    Vec3 x = (triangle->get_p2() - triangle->get_p1()).norm();
    Vec3 y = norm.cross(x);
    UV uv = local_to_uv(local);
    
    return Surface(
        norm,
        Triangle::interpolate_norm(norm1, norm2, norm3, local).norm(),
        x,
        y,
        texture->get(uv),
        bxdf,
        sampler
    );
}

Geometry *ResourceTriangle::get_shape() {
    return triangle;
}



ResourceGroupMesh::ResourceGroupMesh() {
}

ResourceGroupMesh::ResourceGroupMesh(
    const char *filename, 
    Sampler *sampler, 
    Bxdf *bxdf, 
    Texture *texture
) {
    fprintf(stderr, "parse file: %s\n", filename);
    tinyobj::ObjReaderConfig obj_reader_config;
    obj_reader_config.triangulate = true;
    obj_reader_config.vertex_color = false;
    tinyobj::ObjReader obj_reader;
    if(!obj_reader.ParseFromFile(filename)) {
        auto e = obj_reader.Error();
        fprintf(stderr, "Cannot open %s\n ! Message: %s\n", filename, e.c_str());
        exit(1);
    }
    auto &&p = obj_reader.GetAttrib().vertices;
    std::vector<Vec3> vertices;
    for(uint i = 0; i < p.size(); i += 3) {
        vertices.push_back(Vec3(p[i + 0], p[i + 1], p[i + 2]));
    }

    auto &&q = obj_reader.GetAttrib().normals;
    std::vector<Vec3> normals;
    for(uint i = 0; i < q.size(); i += 3) {
        normals.push_back(Vec3(q[i + 0], q[i + 1], q[i + 2]));
    }

    auto &&r = obj_reader.GetAttrib().texcoords;
    std::vector<Vec2> uvs;
    for(uint i = 0; i < r.size(); i += 2) {
        uvs.push_back(Vec2(r[i + 0], r[i + 1]));
    }

    // does NOT support uv weights

    // does NOT support different materials in a single mesh
    // use materials defined in the sccene file

    bool have_normals = normals.size() > 0;
    bool have_uvs = uvs.size() > 0;

    int tot = 0;
    for(auto &&shape: obj_reader.GetShapes()) {
        std::vector<tinyobj::index_t> const &t = shape.mesh.indices;
        assert(t.size() % 3 == 0);
        tot += t.size() / 3;
    }

    set_size(tot);
    
    for(auto &&shape: obj_reader.GetShapes()) {
        std::vector<tinyobj::index_t> const &t = shape.mesh.indices;
        Vec3 p0, p1; // vertices
        Vec3 n0, n1; // normals
        Vec2 u0, u1; // uvs
        bool first = false;
        bool second = false;
        for(tinyobj::index_t i: t) {
            Vec3 p2 = vertices[i.vertex_index];
            Vec3 n2;
            Vec2 u2;
            if(have_normals) {
                assert(0 <= i.normal_index && i.normal_index < vertices.size());
                n2 = normals[i.normal_index];
            }
            if(have_uvs) {
                if(0 <= i.texcoord_index && i.texcoord_index < uvs.size()) {
                    u2 = uvs[i.texcoord_index];
                }
            }
            if(first == false) {
                p0 = p2;
                n0 = n2;
                u0 = u2;
                first = true;
            } else if(second == false) {
                p1 = p2;
                n1 = n2;
                u1 = u2;
                second = true;
            } else {
                Triangle *triangle = new Triangle(p0, p1, p2);
                if(have_normals == false) {
                    n0 = n1 = n2 = triangle->get_norm();
                }
                if(have_uvs == false) {
                    u0 = u1 = u2 = Vec2(0.0f, 0.0f);
                }
                ResourceTriangle info(
                    triangle,
                    n0, n1, n2,
                    u0, u1, u2,
                    sampler,
                    bxdf,
                    texture
                );
                add(info);
                first = second = false;
            }
        }
    }
    fprintf(stderr, "%d triangles parsed in file: %s\n", tot, filename);
}

ResourceGroupMesh::ResourceGroupMesh(
    const int steps,
    const int resolution,
    const Curve &curve, 
    Sampler *sampler, 
    Bxdf *bxdf,
    Texture *texture
) {
    
    // Definition for drawable surface.
    typedef std::tuple<unsigned, unsigned, unsigned> Tup3u;
    // Surface is just a struct that contains vertices, normals, and
    // faces.  VV[i] is the position of vertex i, and VN[i] is the normal
    // of vertex i.  A face is a triple i,j,k corresponding to a triangle
    // with (vertex i, normal i), (vertex j, normal j), ...

    struct Surface {
        std::vector<Vec3> VV;
        std::vector<Vec3> VN;
        std::vector<Tup3u> VF;
    } surface;

    std::vector<CurvePoint> curvePoints;
    const Vec3 UP(0.0f, 1.0f, 0.0f);
    const Vec3 FORWARD(0.0f, 0.0f, 1.0f);

    curve.discretize(resolution, curvePoints);
    for (unsigned int ci = 0; ci < curvePoints.size(); ++ci) {
        const CurvePoint &cp = curvePoints[ci];
        for (unsigned int i = 0; i < steps; ++i) {
            float t = (float) i / steps;
            Mat3 rot = Mat3::mat_from_rotate(UP, t * 2 * 3.14159);
            Vec3 pnew = rot.map(cp.V);
            Vec3 pNormal = cp.T.cross(FORWARD);
            Vec3 nnew = rot.map(pNormal);
            surface.VV.push_back(pnew);
            surface.VN.push_back(nnew);
            int i1 = (i + 1 == steps) ? 0 : i + 1;
            if (ci != curvePoints.size() - 1) {
                surface.VF.emplace_back((ci + 1) * steps + i, ci * steps + i1, ci * steps + i);
                surface.VF.emplace_back((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1);
            }
        }
    }

    set_size(surface.VF.size());
    Triangle *triangles = new Triangle[surface.VF.size()];
    for (unsigned i = 0; i < surface.VF.size(); i++) {
        triangles[i] = Triangle(
            surface.VV[std::get<0>(surface.VF[i])],
            surface.VV[std::get<1>(surface.VF[i])],
            surface.VV[std::get<2>(surface.VF[i])]
        );
        // only simple color now !
        add(ResourceTriangle(
            &triangles[i],
            surface.VN[std::get<0>(surface.VF[i])],
            surface.VN[std::get<1>(surface.VF[i])],
            surface.VN[std::get<2>(surface.VF[i])],
            Vec2(), Vec2(), Vec2(),
            sampler,
            bxdf,
            texture
        ));
    }
}
#include<iostream>
ResourceGroupMesh::ResourceGroupMesh(
    const char *filename,
    std::map<int, bool> have,
    std::map<int, int> index_map,
    std::map<int, Sampler *>sampler_map,
    std::map<int, Bxdf *>bxdf_map,
    std::map<int, Texture *>texture_map,
    bool smooth
) {
    fprintf(stderr, "parse file: %s\n", filename);
    tinyobj::ObjReaderConfig obj_reader_config;
    obj_reader_config.triangulate = true;
    obj_reader_config.vertex_color = false;
    tinyobj::ObjReader obj_reader;
    if(!obj_reader.ParseFromFile(filename)) {
        auto e = obj_reader.Error();
        fprintf(stderr, "Cannot open %s\n ! Message: %s\n", filename, e.c_str());
        exit(1);
    }
    auto &&p = obj_reader.GetAttrib().vertices;
    std::vector<Vec3> vertices;
    for(uint i = 0; i < p.size(); i += 3) {
        vertices.push_back(Vec3(p[i + 0], p[i + 1], p[i + 2]));
    }

    auto &&q = obj_reader.GetAttrib().normals;
    std::vector<Vec3> normals;
    for(uint i = 0; i < q.size(); i += 3) {
        normals.push_back(Vec3(q[i + 0], q[i + 1], q[i + 2]));
    }

    auto &&r = obj_reader.GetAttrib().texcoords;
    std::vector<Vec2> uvs;
    for(uint i = 0; i < r.size(); i += 2) {
        uvs.push_back(Vec2(r[i + 0], r[i + 1]));
    }

    // does NOT support uv weights

    // does NOT support different materials in a single mesh
    // use materials defined in the sccene file

    bool have_normals = normals.size() > 0;
    bool have_uvs = uvs.size() > 0;

    int tot = 0;
    for(auto &&shape: obj_reader.GetShapes()) {
        std::vector<tinyobj::index_t> const &t = shape.mesh.indices;
        assert(t.size() % 3 == 0);
        tot += t.size() / 3;
    }

    set_size(tot);

    int id = 0;
    for(auto &&shape: obj_reader.GetShapes()) {
        std::vector<tinyobj::index_t> const &t = shape.mesh.indices;
        Vec3 p0, p1; // vertices
        Vec3 n0, n1; // normals
        Vec2 u0, u1; // uvs
        bool first = false;
        bool second = false;
        int material_index = -1;
        if(have[id] == true) {
            material_index = index_map[id];
        }
        fprintf(stderr, "in mesh, id: %d, material index: %d\n", id, material_index);
        std::map<int, Vec3> smooth_norm;
        if(smooth && have_normals == false) {
            int i0, i1, i2;
            for(tinyobj::index_t i: t) {
                Vec3 p2 = vertices[i.vertex_index];
                if(first == false) {
                    p0 = p2;
                    i0 = i.vertex_index;
                    first = true;
                } else if(second == false) {
                    p1 = p2;
                    i1 = i.vertex_index;
                    second = true;
                } else {
                    Vec3 norm = Triangle(p0, p1, p2).get_norm();
                    float size = Triangle(p0, p1, p2).area();
                    smooth_norm[i0] += norm * size;
                    smooth_norm[i1] += norm * size;
                    smooth_norm[i.vertex_index] += norm * size;
                    first = second = false;
                }
            }
        }
        for(tinyobj::index_t i: t) {
            Vec3 p2 = vertices[i.vertex_index];
            Vec3 n2;
            Vec2 u2;
            if(have_normals) {
                // assert(0 <= i.normal_index && i.normal_index < vertices.size());
                n2 = normals[i.normal_index];
            } else if(smooth) {
                n2 = smooth_norm[i.vertex_index].norm();
            }
            if(have_uvs) {
                if(0 <= i.texcoord_index && i.texcoord_index < uvs.size()) {
                    u2 = uvs[i.texcoord_index];
                }
            }
            if(first == false) {
                p0 = p2;
                n0 = n2;
                u0 = u2;
                first = true;
            } else if(second == false) {
                p1 = p2;
                n1 = n2;
                u1 = u2;
                second = true;
            } else {
                Triangle *triangle = new Triangle(p0, p1, p2);
                if(have_normals == false && smooth == false) {
                    n0 = n1 = n2 = triangle->get_norm();
                }
                if(have_uvs == false) {
                    u0 = u1 = u2 = Vec2(0.0f, 0.0f);
                }
                if(texture_map[material_index] == nullptr) {
                    fprintf(stderr, "in resource_mesh, texture_map %d is null !\n", material_index);
                }
                ResourceTriangle info(
                    triangle,
                    n0, n1, n2,
                    u0, u1, u2,
                    sampler_map[material_index],
                    bxdf_map[material_index],
                    texture_map[material_index]
                );
                add(info);
                first = second = false;
            }
        }
        ++id;
    }
    fprintf(stderr, "%d triangles parsed in file: %s\n", tot, filename);
}

void ResourceGroupMesh::set_size(int n) {
    infos.reserve(n);
}

int ResourceGroupMesh::n_objects() {
    return infos.size();
}

Resource* ResourceGroupMesh::get_info(int n) {
    return &infos[n];
}

void ResourceGroupMesh::add(const ResourceTriangle &info) {
    infos.push_back(info);
}

void ResourceGroupMesh::trans(Mat3 &T) {
    for(int i = 0; i < n_objects(); ++i) {
        get_info(i)->get_shape()->trans(T);
    }
}