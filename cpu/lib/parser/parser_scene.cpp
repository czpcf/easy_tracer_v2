#include <exception>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "parser_scene.hpp"

SceneParser::SceneParser(const char *filename) {
    camera = nullptr;
    background_color = Vec3(0.0f, 0.0f, 0.0f);

    // parse the file
    assert(filename != nullptr);
    const char *ext = &filename[strlen(filename) - 4];

    if (strcmp(ext, ".txt") != 0) {
        printf("wrong file name extension\n");
        exit(0);
    }
    file = fopen(filename, "r");

    if (file == nullptr) {
        printf("cannot open scene file\n");
        exit(0);
    }
    parse_file();
    fclose(file);
    file = nullptr;
}

SceneParser::~SceneParser() {
    for(int i = 0; i < bxdfs.size(); ++i) {
        delete bxdfs[i];
    }
    bxdfs.clear();

    for(int i = 0; i < textures.size(); ++i) {
        delete textures[i];
    }
    textures.clear();

    for(int i = 0; i < samplers.size(); ++i) {
        delete samplers[i];
    }
    samplers.clear();
}

void SceneParser::parse_file() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    while (get_token(token)) {
        if (!strcmp(token, "PerspectiveCamera")) {
            parse_perspective_camera();
        } else if (!strcmp(token, "Background")) {
            parse_background();
        } else if (!strcmp(token, "Lights")) {
            parse_lights();
        } else if (!strcmp(token, "Materials")) {
            parse_materials();
        } else if (!strcmp(token, "Group")) {
            parse_group(-1, Mat3::id());
        } else {
            printf("Unknown token in parseFile: '%s'\n", token);
            exit(0);
        }
    }
}

int SceneParser::get_token(char token[MAX_PARSER_TOKEN_LENGTH]) {
    // for simplicity, tokens must be separated by whitespace
    assert (file != nullptr);
    int success = fscanf(file, "%s ", token);
    if (success == EOF) {
        token[0] = '\0';
        return 0;
    }
    return 1;
}

float SceneParser::degrees_to_radians(float x) {
    return x * PI / 180.0f;
}

Vec3 SceneParser::read_vec3() {
    float x, y, z;
    int count = fscanf(file, "%f %f %f", &x, &y, &z);
    if (count != 3) {
        printf("Error trying to read 3 floats to make a Vector3f\n");
        assert (0);
    }
    return Vec3(x, y, z);
}

float SceneParser::read_float() {
    float answer;
    int count = fscanf(file, "%f", &answer);
    if (count != 1) {
        printf("Error trying to read 1 float\n");
        assert (0);
    }
    return answer;
}

int SceneParser::read_int() {
    int answer;
    int count = fscanf(file, "%d", &answer);
    if (count != 1) {
        printf("Error trying to read 1 int\n");
        assert (0);
    }
    return answer;
}

void SceneParser::parse_perspective_camera() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the camera parameters
    get_token(token);
    assert (!strcmp(token, "{"));
    get_token(token);
    assert (!strcmp(token, "center"));
    Vec3 center = read_vec3();
    get_token(token);
    assert (!strcmp(token, "direction"));
    Vec3 direction = read_vec3();
    get_token(token);
    assert (!strcmp(token, "up"));
    Vec3 up = read_vec3();
    get_token(token);
    assert (!strcmp(token, "angle"));
    float angle_degrees = read_float();
    float angle_radians = degrees_to_radians(angle_degrees);
    get_token(token);
    assert (!strcmp(token, "width"));
    int width = read_int();
    get_token(token);
    assert (!strcmp(token, "height"));
    int height = read_int();
    get_token(token);
    assert (!strcmp(token, "}"));
    camera = new CameraPerspective(center, direction, up, width, height, angle_radians);
}

void SceneParser::parse_background() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the background color
    get_token(token);
    assert (!strcmp(token, "{"));
    while (true) {
        get_token(token);
        if (!strcmp(token, "}")) {
            break;
        } else if (!strcmp(token, "color")) {
            background_color = read_vec3();
        } else {
            printf("Unknown token in parseBackground: '%s'\n", token);
            assert(0);
        }
    }
}

void SceneParser::parse_materials() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token(token);
    assert (!strcmp(token, "{"));
    // read in the number of objects
    get_token(token);
    assert (!strcmp(token, "numMaterials"));
    int n_materials = read_int();
    
    bxdfs.reserve(n_materials);
    textures.reserve(n_materials);
    samplers.reserve(n_materials);
    
    // read in the objects
    int count = 0;
    while (n_materials > count) {
        get_token(token);
        if (strcmp(token, "Material") == 0 ||
            strcmp(token, "PhongMaterial") == 0) {
            parse_material_lambertian();
        } else {
            printf("Unknown token in parseMaterial: '%s'\n", token);
            exit(0);
        }
        count++;
    }
    get_token(token);
    assert (!strcmp(token, "}"));
}

void SceneParser::parse_material_lambertian() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    Vec3 diffuseColor(1, 1, 1), specularColor(0, 0, 0);
    float shininess = 0;
    get_token(token);
    assert (!strcmp(token, "{"));
    while (true) {
        get_token(token);
        if (strcmp(token, "diffuseColor") == 0) {
            diffuseColor = read_vec3();
        } else if (strcmp(token, "specularColor") == 0) {
            specularColor = read_vec3();
        } else if (strcmp(token, "shininess") == 0) {
            shininess = read_float();
        } else if (strcmp(token, "texture") == 0) {
            // Optional: read in texture and draw it.
            get_token(filename);
        } else {
            assert (!strcmp(token, "}"));
            break;
        }
    }
    auto *bxdf = new BxdfPhong(diffuseColor, specularColor, shininess);
    auto *sampler = new SamplerLambertian();
    auto *texture = new TextureSimple(Vec3(1.0f, 1.0f, 1.0f));
    bxdfs.push_back(bxdf);
    samplers.push_back(sampler);
    textures.push_back(texture);
}

int SceneParser::get_num_materials() {
    return bxdfs.size();
}

void SceneParser::parse_lights() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token(token);
    assert (!strcmp(token, "{"));
    // read in the number of objects
    get_token(token);
    assert (!strcmp(token, "numLights"));
    int num_lights = read_int();
    // read in the objects
    int count = 0;
    while (num_lights > count) {
        get_token(token);
        if (strcmp(token, "DirectionalLight") == 0) {
            parse_directional_light();
        } else if (strcmp(token, "PointLight") == 0) {
            parse_point_light();
        } else {
            printf("Unknown token in parseLight: '%s'\n", token);
            exit(0);
        }
        count++;
    }
    get_token(token);
    assert (!strcmp(token, "}"));
}

void SceneParser::parse_point_light() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token(token);
    assert (!strcmp(token, "{"));
    get_token(token);
    assert (!strcmp(token, "position"));
    Vec3 position = read_vec3();
    get_token(token);
    assert (!strcmp(token, "color"));
    Vec3 color = read_vec3();
    get_token(token);
    assert (!strcmp(token, "}"));

    LightPoint *light_point = new LightPoint(Point(position));
    ResourceGroupLightPoint *group = new ResourceGroupLightPoint;
    ResourceLightPoint info(light_point, nullptr, nullptr, nullptr, color);
    group->add(info);
    group_light_point.push_back(group);
}

void SceneParser::parse_directional_light() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token(token);
    assert (!strcmp(token, "{"));
    get_token(token);
    assert (!strcmp(token, "direction"));
    Vec3 direction = read_vec3();
    get_token(token);
    assert (!strcmp(token, "color"));
    Vec3 color = read_vec3();
    get_token(token);
    assert (!strcmp(token, "}"));

    LightDirection *light_direction = new LightDirection(Direction(direction));
    ResourceGroupLightDirection *group = new ResourceGroupLightDirection;
    ResourceLightDirection info(light_direction, nullptr, nullptr, nullptr, color);
    group->add(info);
    group_light_direction.push_back(group);
}

void SceneParser::parse_group(int current_index, Mat3 T) {
    //
    // each group starts with an integer that specifies
    // the number of objects in the group
    //
    // the material index sets the material of all objects which follow,
    // until the next material index (scoping for the materials is very
    // simple, and essentially ignores any tree hierarchy)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token(token);
    assert (!strcmp(token, "{"));

    // read in the number of objects
    get_token(token);
    assert (!strcmp(token, "numObjects"));
    int num_objects = read_int();

    // read in the objects
    int count = 0;
    int index = current_index;
    while (num_objects > count) {
        get_token(token);
        if (!strcmp(token, "MaterialIndex")) {
            // change the current material
            index = read_int();
            assert (index >= 0 && index <= get_num_materials());
        } else {
            parse_object(index, T, token);
            count++;
        }
    }
    get_token(token);
    assert (!strcmp(token, "}"));
}

void SceneParser::parse_object(int current_index, Mat3 T, char token[MAX_PARSER_TOKEN_LENGTH]) {
    if (!strcmp(token, "Group")) {
        parse_group(current_index, T);
    } else if (!strcmp(token, "Sphere")) {
        parse_sphere(current_index, T);
    } else if (!strcmp(token, "Plane")) {
        parse_plane(current_index, T);
    } else if (!strcmp(token, "Triangle")) {
        parse_triangle(current_index, T);
    } else if (!strcmp(token, "TriangleMesh")) {
        parse_triangle_mesh(current_index, T);
    } else if (!strcmp(token, "Transform")) {
        parse_transform(current_index, T);
    } else {
        printf("Unknown token in parseObject: '%s'\n", token);
        exit(0);
    }
}

void SceneParser::parse_sphere(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token(token);
    assert (!strcmp(token, "{"));
    get_token(token);
    assert (!strcmp(token, "center"));
    Vec3 center = read_vec3();
    get_token(token);
    assert (!strcmp(token, "radius"));
    float radius = read_float();
    get_token(token);
    assert (!strcmp(token, "}"));

    ResourceGroupSphere *group = new ResourceGroupSphere;
    group->set_size(1);
    Vec3 z(0.0f, 0.0f, 1.0f);
    Vec3 x(1.0f, 0.0f, 0.0f);
    Vec3 y(0.0f, 1.0f, 0.0f);
    Sphere *sphere = new Sphere(
        z,
        x,
        center,
        radius);
    if(current_index == -1) {
        fprintf(stderr, "no material found!\n");
        exit(0);
    }
    ResourceSphere info(
        sphere,
        samplers[current_index],
        bxdfs[current_index],
        textures[current_index]
    );
    group->add(info);
    group->trans(T);
    group_sphere.push_back(group);
}

void SceneParser::parse_plane(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token(token);
    assert (!strcmp(token, "{"));
    get_token(token);
    assert (!strcmp(token, "normal"));
    Vec3 normal = read_vec3();
    get_token(token);
    assert (!strcmp(token, "offset"));
    float offset = read_float();
    get_token(token);
    assert (!strcmp(token, "}"));   
    ResourceGroupPlane *group = new ResourceGroupPlane;
    group->set_size(1);
    Vec3 origin = normal * offset;
    Plane *plane = new Plane(
        origin,
        normal,
        Vec3(0.123f, 0.234f, 0.345f),
        Vec3(-0.666f, 0.987f, 0.233f)
    );
    if(current_index == -1) {
        fprintf(stderr, "no material found!\n");
        exit(0);
    }
    ResourcePlane info(
        plane,
        samplers[current_index],
        bxdfs[current_index],
        textures[current_index]
    );
    group->add(info);
    group->trans(T);
    group_plane.push_back(group);
}

void SceneParser::parse_triangle(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token(token);
    assert (!strcmp(token, "{"));
    get_token(token);
    assert (!strcmp(token, "vertex0"));
    Vec3 v0 = read_vec3();
    get_token(token);
    assert (!strcmp(token, "vertex1"));
    Vec3 v1 = read_vec3();
    get_token(token);
    assert (!strcmp(token, "vertex2"));
    Vec3 v2 = read_vec3();
    get_token(token);
    assert (!strcmp(token, "}"));
    ResourceGroupMesh *group = new ResourceGroupMesh;
    group->set_size(1);
    Triangle *triangle = new Triangle(v0, v1, v2);
    Vec3 norm = triangle->get_norm();
    if(current_index == -1) {
        fprintf(stderr, "no material found!\n");
        exit(0);
    }
    ResourceTriangle info(
        triangle,
        norm, norm, norm,
        Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f),
        samplers[current_index],
        bxdfs[current_index],
        textures[current_index]
    );
    group->add(info);
    group->trans(T);
    group_mesh.push_back(group);
}

void SceneParser::parse_triangle_mesh(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    // get the filename
    get_token(token);
    assert (!strcmp(token, "{"));
    get_token(token);
    assert (!strcmp(token, "obj_file"));
    get_token(filename);
    get_token(token);
    assert (!strcmp(token, "}"));
    const char *ext = &filename[strlen(filename) - 4];
    assert(!strcmp(ext, ".obj"));
    if(current_index == -1) {
        fprintf(stderr, "no material found!\n");
        exit(0);
    }
    ResourceGroupMesh *group = new ResourceGroupMesh(
        filename,
        samplers[current_index],
        bxdfs[current_index],
        textures[current_index]
    );
    group->trans(T);
    group_mesh.push_back(group);
}

void SceneParser::parse_transform(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    Mat3 matrix = Mat3::id();
    get_token(token);
    assert (!strcmp(token, "{"));
    // read in transformations: 
    // apply to the LEFT side of the current matrix (so the first
    // transform in the list is the last applied to the object)
    get_token(token);

    while (true) {
        if (!strcmp(token, "Scale")) {
            Vec3 s = read_vec3();
            matrix = matrix * Mat3::mat_from_scale(s);
        } else if (!strcmp(token, "UniformScale")) {
            float s = read_float();
            matrix = matrix * Mat3::mat_from_scale(Vec3(s, s, s));
        } else if (!strcmp(token, "Translate")) {
            matrix = matrix * Mat3::mat_from_trans(read_vec3());
        } else if (!strcmp(token, "XRotate")) {
            matrix = matrix * Mat3::mat_from_rotate(Vec3(1.0f, 0.0f, 0.0f), degrees_to_radians(read_float()));
        } else if (!strcmp(token, "YRotate")) {
            matrix = matrix * Mat3::mat_from_rotate(Vec3(0.0f, 1.0f, 0.0f), degrees_to_radians(read_float()));
        } else if (!strcmp(token, "ZRotate")) {
            matrix = matrix * Mat3::mat_from_rotate(Vec3(0.0f, 0.0f, 1.0f), degrees_to_radians(read_float()));
        } else if (!strcmp(token, "Rotate")) {
            get_token(token);
            assert (!strcmp(token, "{"));
            Vec3 axis = read_vec3();
            float degrees = read_float();
            float radians = degrees_to_radians(degrees);
            matrix = matrix * Mat3::mat_from_rotate(axis, radians);
            get_token(token);
            assert (!strcmp(token, "}"));
        } else if (!strcmp(token, "Matrix4f")) {
            fprintf(stderr, "does not support transform followed by another transform !\n");
            assert(0);
        } else {
            // otherwise this must be an object,
            // and there are no more transformations
            parse_object(current_index, T * matrix, token);
            break;
        }
        get_token(token);
    }

    get_token(token);
    assert (!strcmp(token, "}"));
}







int SceneParser::n_group_sphere() {
    return group_sphere.size();
}

int SceneParser::tot_in_group_sphere() {
    int n = 0;
    for(int i = 0; i < group_sphere.size(); ++i) {
        n += group_sphere[i]->n_objects();
    }
    return n;
}

ResourceGroupSphere *SceneParser::get_group_sphere(int n) {
    return group_sphere[n];
}


int SceneParser::n_group_light_point() {
    return group_light_point.size();
}

int SceneParser::tot_in_group_light_point() {
    int n = 0;
    for(int i = 0; i < group_light_point.size(); ++i) {
        n += group_light_point[i]->n_objects();
    }
    return n;
}

ResourceGroupLightPoint *SceneParser::get_group_light_point(int n) {
    return group_light_point[n];
}


int SceneParser::n_group_mesh() {
    return group_mesh.size();
}

int SceneParser::tot_in_group_mesh() {
    int n = 0;
    for(int i = 0; i < group_mesh.size(); ++i) {
        n += group_mesh[i]->n_objects();
    }
    return n;
}

ResourceGroupMesh *SceneParser::get_group_mesh(int n) {
    return group_mesh[n];
}

int SceneParser::n_group_plane() {
    return group_plane.size();
}

int SceneParser::tot_in_group_plane() {
    int n = 0;
    for(int i = 0; i < group_plane.size(); ++i) {
        n += group_plane[i]->n_objects();
    }
    return n;
}

ResourceGroupPlane *SceneParser::get_group_plane(int n) {
    return group_plane[n];
}


int SceneParser::n_group_light_direction() {
    return group_light_direction.size();
}

int SceneParser::tot_in_group_light_direction() {
    int n = 0;
    for(int i = 0; i < group_light_direction.size(); ++i) {
        n += group_light_direction[i]->n_objects();
    }
    return n;
}

ResourceGroupLightDirection *SceneParser::get_group_light_direction(int n) {
    return group_light_direction[n];
}

// accel //////////////////////////////////////////////////////////////////////

template<typename T>
void SceneParser::get_n_light(Accel *accel, std::vector<T *> &group) {
    int m = 0;
    for(int i = 0; i < group.size(); ++i) {
        m += group[i]->n_objects();
    }
    n_tot += m;
    n_lights += m;
}

template<typename T>
void SceneParser::get_n(Accel *accel, std::vector<T *> &group) {
    int m = 0;
    for(int i = 0; i < group.size(); ++i) {
        m += group[i]->n_objects();
    }
    n_tot += m;
}

template<typename T>
void SceneParser::add_resource_light(Accel *accel, std::vector<T *> &group) {
    for(int i = 0; i < group.size(); ++i) {
        accel->add(group[i]);
        for(int j = 0; j < group[i]->n_objects(); ++j) {
            accel_resource_light.push_back(group[i]->get_info(j));
        }
    }
}

template<typename T>
void SceneParser::add_resource(Accel *accel, std::vector<T *> &group) {
    for(int i = 0; i < group.size(); ++i) {
        accel->add(group[i]);
        for(int j = 0; j < group[i]->n_objects(); ++j) {
            accel_resource.push_back(group[i]->get_info(j));
        }
    }
}

Accel *SceneParser::build_accel() {
    // can change data structure here
    AccelNaive *accel = new AccelNaive();

    n_tot = 0;
    n_lights = 0;
    

    get_n_light(accel, group_light_point);
    get_n_light(accel, group_light_direction);
    get_n(accel, group_mesh);
    get_n(accel, group_sphere);
    get_n(accel, group_plane);

    accel->set_size(n_tot);

    // add lights first
    add_resource_light(accel, group_light_point);
    add_resource_light(accel, group_light_direction);
    add_resource(accel, group_mesh);
    add_resource(accel, group_sphere);
    add_resource(accel, group_plane);

    accel->build();

    assert(n_tot - n_lights == accel_resource.size());
    assert(n_lights == accel_resource_light.size());

    return accel;
}

bool SceneParser::is_light(const RayHit &hit) {
    return hit.get_id() < n_lights;
}

ResourceLight *SceneParser::get_light_info(const RayHit &hit) {
    assert(0 <= hit.get_id() && hit.get_id() < n_lights);
    return accel_resource_light[hit.get_id()];
}

Resource *SceneParser::get_info(const RayHit &hit) {
    assert(n_lights <= hit.get_id() && hit.get_id() < n_tot);
    return accel_resource[hit.get_id() - n_lights];
}