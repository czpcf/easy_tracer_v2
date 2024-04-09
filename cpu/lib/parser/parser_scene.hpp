#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include <cassert>

#include "../accel/accel_bvh.hpp"
#include "../accel/accel_naive.hpp"
#include "../accel/accel.hpp"

#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"

#include "../camera/camera_perspective.hpp"
#include "../camera/camera.hpp"

#include "../bxdf/bxdf_specular.hpp"
#include "../bxdf/bxdf_disney.hpp"
#include "../bxdf/bxdf_ggx.hpp"
#include "../bxdf/bxdf_gtr1.hpp"
#include "../bxdf/bxdf_lambertian.hpp"
#include "../bxdf/bxdf_phong.hpp"
#include "../bxdf/bxdf.hpp"

#include "../geometry/direction.hpp"
#include "../geometry/geometry.hpp"
#include "../geometry/plane.hpp"
#include "../geometry/point.hpp"
#include "../geometry/triangle.hpp"
#include "../geometry/sphere.hpp"

#include "../light/light_point.hpp"
#include "../light/light.hpp"

#include "../resource/emittor.hpp"
#include "../resource/resource_light_direction.hpp"
#include "../resource/resource_light_global.hpp"
#include "../resource/resource_light_point.hpp"
#include "../resource/resource_light_triangle.hpp"
#include "../resource/resource_mesh.hpp"
#include "../resource/resource_plane.hpp"
#include "../resource/resource_sphere.hpp"
#include "../resource/resource.hpp"

#include "../sampler/sampler_disney.hpp"
#include "../sampler/sampler_ggx.hpp"
#include "../sampler/sampler_gtr1.hpp"
#include "../sampler/sampler_lambertian.hpp"
#include "../sampler/sampler_reflection.hpp"
#include "../sampler/sampler_refraction.hpp"
#include "../sampler/sampler.hpp"

#include "../texture/texture_simple.hpp"
#include "../texture/texture.hpp"

#define MAX_PARSER_TOKEN_LENGTH 1024

class SceneParser {
public:

    SceneParser();
    SceneParser(const char *filename);

    void init(const char *filename);

    ~SceneParser();

    Camera *get_camera() const {
        return camera;
    }

    Vec3 get_background_color() const {
        return background_color;
    }

    int n_group_mesh();
    int tot_in_group_mesh();
    ResourceGroupMesh *get_group_mesh(int n);

    int n_group_sphere();
    int tot_in_group_sphere();
    ResourceGroupSphere *get_group_sphere(int n);

    int n_group_light_point();
    int tot_in_group_light_point();
    ResourceGroupLightPoint *get_group_light_point(int n);

    int n_group_plane();
    int tot_in_group_plane();
    ResourceGroupPlane *get_group_plane(int n);

    int n_group_light_direction();
    int tot_in_group_light_direction();
    ResourceGroupLightDirection *get_group_light_direction(int n);

    int n_group_light_triangle();
    int tot_in_group_light_triangle();
    ResourceGroupLightTriangle *get_group_light_triangle(int n);

    int n_group_light_global();
    int tot_in_group_light_global();
    ResourceGroupLightGlobal *get_group_light_global(int n);

    Accel *build_accel();

    /// @brief if the hit indicates a light
    bool is_light(const RayHit &hit);

    /// @brief if hit is LIGHT, return light info
    ResourceLight *get_light_info(const RayHit &hit);

    /// @brief if hit is NOT LIGHT, return light info
    Resource *get_info(const RayHit &hit);

    /// @brief return a random light and set pdf
    void get_random_light(RNG *rng, ResourceLight *&light, float &pdf);

    /// @brief return pdf of choosing this light
    float get_pdf_random_light(const RayHit &hit);

private:
    int parse_lines;

    float degrees_to_radians(float);
    void parse_file();
    void parse_perspective_camera();
    void parse_background();
    void parse_lights();
    void parse_point_light();
    void parse_directional_light();
    void parse_global_light();

    void parse_materials();
    void parse_material_phong();
    void parse_bxdf_lambertian();
    void parse_bxdf_ggx();
    void parse_bxdf_gtr1();
    void parse_bxdf_disney();
    void parse_bxdf_reflection();
    void parse_bxdf_refraction();

    void parse_group(int current_index, Mat3 T);
    void parse_object(int current_index, Mat3 T, char token[MAX_PARSER_TOKEN_LENGTH]);
    void parse_sphere(int current_index, Mat3 T);
    void parse_plane(int current_index, Mat3 T);
    void parse_triangle(int current_index, Mat3 T);
    void parse_triangle_mesh(int current_index, Mat3 T);
    void parse_triangle_light();
    void parse_transform(int current_index, Mat3 T);

    int get_num_materials();

    int get_token(char token[MAX_PARSER_TOKEN_LENGTH]);
    void get_token_expect(char token[MAX_PARSER_TOKEN_LENGTH], const char *expect);

    Vec3 read_vec3();
    float read_float();
    int read_int();

    FILE *file;
    Camera *camera;
    Vec3 background_color;
    std::vector<Bxdf*> bxdfs;
    std::vector<Sampler*> samplers;
    std::vector<Texture*> textures;

    std::vector<ResourceGroupLightPoint*> group_light_point;
    std::vector<ResourceGroupLightDirection*> group_light_direction;
    std::vector<ResourceGroupLightTriangle*> group_light_triangle;
    std::vector<ResourceGroupLightGlobal*> group_light_global;
    std::vector<ResourceGroupSphere*> group_sphere;
    std::vector<ResourceGroupPlane*> group_plane;
    std::vector<ResourceGroupMesh*> group_mesh;

    // arranged
    int n_lights;
    int n_tot;
    std::vector<ResourceLight*> accel_resource_light;
    std::vector<Resource*> accel_resource;

    template<typename T>void get_n_light(Accel *accel, std::vector<T *> &);
    template<typename T>void get_n(Accel *accel, std::vector<T *> &);
    template<typename T>void add_resource_light(Accel *accel, std::vector<T *> &);
    template<typename T>void add_resource(Accel *accel, std::vector<T *> &);
};

#endif // SCENE_PARSER_H
