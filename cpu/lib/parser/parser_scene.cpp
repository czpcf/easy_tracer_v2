#include <exception>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include "parser_scene.hpp"

SceneParser::SceneParser() {
}

SceneParser::SceneParser(const char *filename) {
    init(filename);
}

void SceneParser::init(const char *filename) {
    parse_lines = 0;
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

void SceneParser::get_token_expect(char token[MAX_PARSER_TOKEN_LENGTH], const char *expect) {
    // for simplicity, tokens must be separated by whitespace
    assert (file != nullptr);
    int success = fscanf(file, "%s ", token);
    if (success == EOF) {
        token[0] = '\0';
    }
    if(strcmp(token, expect) != 0) {
        fprintf(stderr, "unexpected token found: %s, expect: %s\n", token, expect);
        assert(0);
    }
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
    get_token_expect(token, "{");
    get_token_expect(token, "center");
    Vec3 center = read_vec3();
    get_token_expect(token, "direction");
    Vec3 direction = read_vec3();
    get_token_expect(token, "up");
    Vec3 up = read_vec3();
    get_token_expect(token, "angle");
    float angle_degrees = read_float();
    float angle_radians = degrees_to_radians(angle_degrees);
    get_token_expect(token, "width");
    int width = read_int();
    get_token_expect(token, "height");
    int height = read_int();
    get_token_expect(token, "}");
    camera = new CameraPerspective(center, direction, up, width, height, angle_radians);
}

void SceneParser::parse_background() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the background color
    get_token_expect(token, "{");
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
    get_token_expect(token, "{");
    // read in the number of objects
    get_token_expect(token, "numMaterials");
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
            parse_material_phong();
        } else if (strcmp(token, "bxdfLambertian") == 0) {
            parse_bxdf_lambertian();
        } else if (strcmp(token, "bxdfGGX") == 0) {
            parse_bxdf_ggx();
        } else if (strcmp(token, "bxdfGTR1") == 0) {
            parse_bxdf_gtr1();
        } else if (strcmp(token, "bxdfDisney") == 0) {
            parse_bxdf_disney();
        } else if (strcmp(token, "bxdfReflection") == 0) {
            parse_bxdf_reflection();
        } else if (strcmp(token, "bxdfRefraction") == 0) {
            parse_bxdf_refraction();
        } else {
            printf("Unknown token in parseMaterial: '%s'\n", token);
            exit(0);
        }
        count++;
    }
    get_token_expect(token, "}");
}

void SceneParser::parse_material_phong() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    Vec3 diffuseColor(1, 1, 1), specularColor(0, 0, 0);
    float shininess = 0;
    get_token_expect(token, "{");
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

void SceneParser::parse_bxdf_lambertian() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    get_token_expect(token, "{");
    Vec3 color = read_vec3();
    get_token_expect(token, "}");

    auto *bxdf = new BxdfLambertian();
    auto *sampler = new SamplerLambertian();
    auto *texture = new TextureSimple(color);
    bxdfs.push_back(bxdf);
    samplers.push_back(sampler);
    textures.push_back(texture);
}

void SceneParser::parse_bxdf_ggx() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    get_token_expect(token, "{");
    get_token_expect(token, "alphaX");
    float alpha_x = read_float();
    get_token_expect(token, "alphaY");
    float alpha_y = read_float();
    get_token_expect(token, "roughness");
    float roughness = read_float();
    get_token_expect(token, "color");
    Vec3 color = read_vec3();
    get_token_expect(token, "}");

    auto *bxdf = new BxdfGGX(roughness, alpha_x, alpha_y);
    auto *sampler = new SamplerGGX(roughness, alpha_x, alpha_y);
    auto *texture = new TextureSimple(color);
    bxdfs.push_back(bxdf);
    samplers.push_back(sampler);
    textures.push_back(texture);
}

void SceneParser::parse_bxdf_gtr1() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    get_token_expect(token, "{");
    get_token_expect(token, "alpha");
    float alpha = read_float();
    get_token_expect(token, "color");
    Vec3 color = read_vec3();
    get_token_expect(token, "}");

    if(alpha >= 1.0) {
        fprintf(stderr, "WARNING : alpha in bxdfGTR1 is >= 1.0 !\n");
    }

    auto *bxdf = new BxdfGTR1(alpha);
    auto *sampler = new SamplerGTR1(alpha);
    auto *texture = new TextureSimple(color);
    bxdfs.push_back(bxdf);
    samplers.push_back(sampler);
    textures.push_back(texture);
}

void SceneParser::parse_bxdf_disney() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    get_token_expect(token, "{");
    get_token_expect(token, "color");
    Vec3 color = read_vec3();
    get_token_expect(token, "metallic");
    float metallic = read_float();
    get_token_expect(token, "subsurface");
    float subsurface = read_float();
    get_token_expect(token, "specular");
    float specular = read_float();
    get_token_expect(token, "specularTint");
    float specularTint = read_float();
    get_token_expect(token, "roughness");
    float roughness = read_float();
    get_token_expect(token, "anisotropic");
    float anisotropic = read_float();
    get_token_expect(token, "sheen");
    float sheen = read_float();
    get_token_expect(token, "sheenTint");
    float sheenTint = read_float();
    get_token_expect(token, "clearcoat");
    Vec3 clearcoat = read_vec3();
    get_token_expect(token, "clearcoatGloss");
    float clearcoatGloss = read_float();
    get_token_expect(token, "}");

    auto *bxdf = new BxdfDisney(
        metallic,
        subsurface,
        specular,
        specularTint,
        roughness,
        anisotropic,
        sheen,
        sheenTint,
        clearcoat,
        clearcoatGloss
    );
    // auto *sampler = new SamplerLambertian();
    // auto *sampler = new SamplerGGX(0.1, 0.1, 0.1);
    // auto *sampler = new SamplerGTR1(0.1);
    auto *sampler = new SamplerDisney(
        metallic,
        subsurface,
        specular,
        specularTint,
        roughness,
        anisotropic,
        sheen,
        sheenTint,
        clearcoat,
        clearcoatGloss
    );
    auto *texture = new TextureSimple(color);
    bxdfs.push_back(bxdf);
    samplers.push_back(sampler);
    textures.push_back(texture);
}

void SceneParser::parse_bxdf_reflection() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    get_token_expect(token, "{");
    get_token_expect(token, "color");
    Vec3 color = read_vec3();
    get_token_expect(token, "}");

    auto *bxdf = new BxdfSpecular();
    auto *sampler = new SamplerReflection();
    auto *texture = new TextureSimple(color);
    bxdfs.push_back(bxdf);
    samplers.push_back(sampler);
    textures.push_back(texture);
}

void SceneParser::parse_bxdf_refraction() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    get_token_expect(token, "{");
    get_token_expect(token, "refract");
    float alpha = read_float();
    get_token_expect(token, "color");
    Vec3 color = read_vec3();
    get_token_expect(token, "}");

    auto *bxdf = new BxdfSpecular();
    auto *sampler = new SamplerRefraction(alpha);
    auto *texture = new TextureSimple(color);
    bxdfs.push_back(bxdf);
    samplers.push_back(sampler);
    textures.push_back(texture);
}

int SceneParser::get_num_materials() {
    return bxdfs.size();
}

void SceneParser::parse_lights() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token_expect(token, "{");
    // read in the number of objects
    get_token_expect(token, "numLights");
    int num_lights = read_int();
    // read in the objects
    int count = 0;
    while (num_lights > count) {
        get_token(token);
        if (strcmp(token, "DirectionalLight") == 0) {
            parse_directional_light();
        } else if (strcmp(token, "PointLight") == 0) {
            parse_point_light();
        } else if (strcmp(token, "GlobalLight") == 0) {
            parse_global_light();
        } else if (strcmp(token, "TriangularLight") == 0) {
            parse_triangle_light();
        } else {
            printf("Unknown token in parseLight: '%s'\n", token);
            exit(0);
        }
        count++;
    }
    get_token_expect(token, "}");
}

void SceneParser::parse_point_light() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token_expect(token, "{");
    get_token_expect(token, "position");
    Vec3 position = read_vec3();
    get_token_expect(token, "color");
    Vec3 color = read_vec3();
    get_token_expect(token, "}");

    LightPoint *light_point = new LightPoint(Point(position));
    ResourceGroupLightPoint *group = new ResourceGroupLightPoint;
    ResourceLightPoint info(light_point, nullptr, nullptr, nullptr, color);
    group->add(info);
    group_light_point.push_back(group);
}

void SceneParser::parse_directional_light() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token_expect(token, "{");
    get_token_expect(token, "direction");
    Vec3 direction = read_vec3();
    get_token_expect(token, "color");
    Vec3 color = read_vec3();
    get_token_expect(token, "}");

    LightDirection *light_direction = new LightDirection(Direction(direction));
    ResourceGroupLightDirection *group = new ResourceGroupLightDirection;
    ResourceLightDirection info(light_direction, nullptr, nullptr, nullptr, color);
    group->add(info);
    group_light_direction.push_back(group);
}

void SceneParser::parse_triangle_light() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token_expect(token, "{");
    get_token_expect(token, "MaterialIndex");
    int index = read_int();
    get_token_expect(token, "emittor");
    Vec3 emittor = read_vec3();
    get_token_expect(token, "p0");
    Vec3 p0 = read_vec3();
    get_token_expect(token, "p1");
    Vec3 p1 = read_vec3();
    get_token_expect(token, "p2");
    Vec3 p2 = read_vec3();
    get_token_expect(token, "}");

    LightTriangle *light_triangle = new LightTriangle(Triangle(p0, p1, p2));
    ResourceGroupLightTriangle *group = new ResourceGroupLightTriangle;
    ResourceLightTriangle info(
        light_triangle,
        samplers[index],
        bxdfs[index],
        textures[index],
        emittor
    );
    group->add(info);
    group_light_triangle.push_back(group);
}

void SceneParser::parse_global_light() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token_expect(token, "{");
    get_token_expect(token, "emittor");
    Vec3 emittor = read_vec3();
    get_token_expect(token, "}");

    LightGlobal *light_global = new LightGlobal();
    ResourceGroupLightGlobal *group = new ResourceGroupLightGlobal;
    TextureSimple *texture = new TextureSimple(emittor);
    ResourceLightGlobal info(
        light_global,
        nullptr,
        nullptr,
        texture,
        emittor
    );
    group->add(info);
    group_light_global.push_back(group);
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
    get_token_expect(token, "{");

    // read in the number of objects
    get_token_expect(token, "numObjects");
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
    get_token_expect(token, "}");
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
    } else if (!strcmp(token, "BezierCurve")) {
        parse_bezier_curve(current_index, T);
    } else if (!strcmp(token, "BsplineCurve")) {
        parse_bspline_curve(current_index, T);
    } else if (!strcmp(token, "Transform")) {
        parse_transform(current_index, T);
    } else {
        printf("Unknown token in parseObject: '%s'\n", token);
        exit(0);
    }
}

void SceneParser::parse_sphere(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token_expect(token, "{");
    get_token_expect(token, "center");
    Vec3 center = read_vec3();
    get_token_expect(token, "radius");
    float radius = read_float();
    get_token_expect(token, "}");

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
        assert(0);
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
    get_token_expect(token, "{");
    get_token_expect(token, "normal");
    Vec3 normal = read_vec3();
    get_token_expect(token, "offset");
    float offset = read_float();
    get_token_expect(token, "}");
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
        assert(0);
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
    get_token_expect(token, "{");
    get_token_expect(token, "vertex0");
    Vec3 v0 = read_vec3();
    get_token_expect(token, "vertex1");
    Vec3 v1 = read_vec3();
    get_token_expect(token, "vertex2");
    Vec3 v2 = read_vec3();
    get_token_expect(token, "}");
    ResourceGroupMesh *group = new ResourceGroupMesh;
    group->set_size(1);
    Triangle *triangle = new Triangle(v0, v1, v2);
    Vec3 norm = triangle->get_norm();
    if(current_index == -1) {
        fprintf(stderr, "no material found!\n");
        assert(0);
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
    get_token_expect(token, "{");

    int index_defualt = current_index;
    std::map<int, bool> have;
    std::map<int, int> index_map;
    std::map<int, Bxdf*> bxdf_map;
    std::map<int, Sampler*> sampler_map;
    std::map<int, Texture*> texture_map;
    bxdf_map[-1] = bxdfs[index_defualt];
    sampler_map[-1] = samplers[index_defualt];
    texture_map[-1] = textures[index_defualt];
    bool smooth = false;
    while(true) {
        get_token(token);
        if(!strcmp(token, "faceMaterial")) {
            get_token(token);
            if(!strcmp(token, "default")) {
                index_defualt = read_int();
                bxdf_map[-1] = bxdfs[index_defualt];
                sampler_map[-1] = samplers[index_defualt];
                texture_map[-1] = textures[index_defualt];
            } else if(!strcmp(token, "f")) {
                int what = read_int();
                int index = read_int();
                have[what] = true;
                index_map[what] = index;
                bxdf_map[index] = bxdfs[index];
                sampler_map[index] = samplers[index];
                texture_map[index] = textures[index];
            } else {
                fprintf(stderr, "bad token found in parse_triangle_mesh !\n");
                assert(0);
            }
        } else if(!strcmp(token, "smooth")) {
            smooth = true;
        }else {
            break;
        }
    }

    assert (!strcmp(token, "obj_file"));
    get_token(filename);
    get_token_expect(token, "}");
    const char *ext = &filename[strlen(filename) - 4];
    assert(!strcmp(ext, ".obj"));
    ResourceGroupMesh *group = new ResourceGroupMesh(
        filename,
        have,
        index_map,
        sampler_map,
        bxdf_map,
        texture_map,
        smooth
    );
    group->trans(T);
    group_mesh.push_back(group);
}

void SceneParser::parse_transform(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    Mat3 matrix = Mat3::id();
    get_token_expect(token, "{");
    // read in transformations: 
    // apply to the LEFT side of the current matrix (so the first
    // transform in the list is the last applied to the object)
    get_token(token);

    while (true) {
        if (!strcmp(token, "Scale")) {
            Vec3 s = read_vec3();
            matrix = Mat3::mat_from_scale(s) * matrix;
        } else if (!strcmp(token, "UniformScale")) {
            float s = read_float();
            matrix = Mat3::mat_from_scale(Vec3(s, s, s)) * matrix;
        } else if (!strcmp(token, "Translate")) {
            matrix = Mat3::mat_from_trans(read_vec3()) * matrix;
        } else if (!strcmp(token, "XRotate")) {
            matrix = Mat3::mat_from_rotate(Vec3(1.0f, 0.0f, 0.0f), degrees_to_radians(read_float())) * matrix;
        } else if (!strcmp(token, "YRotate")) {
            matrix = Mat3::mat_from_rotate(Vec3(0.0f, 1.0f, 0.0f), degrees_to_radians(read_float())) * matrix;
        } else if (!strcmp(token, "ZRotate")) {
            matrix = Mat3::mat_from_rotate(Vec3(0.0f, 0.0f, 1.0f), degrees_to_radians(read_float())) * matrix;
        } else if (!strcmp(token, "Rotate")) {
            get_token(token);
            assert (!strcmp(token, "{"));
            Vec3 axis = read_vec3();
            float degrees = read_float();
            float radians = degrees_to_radians(degrees);
            matrix = Mat3::mat_from_rotate(axis, radians) * matrix;
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

    get_token_expect(token, "}");
}


void SceneParser::parse_bezier_curve(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token_expect(token, "{");
    std::vector<Vec3> controls;
    int index = current_index;
    int steps = 40;
    int resolution = 30;

    while (true) {
        get_token(token);
        if (!strcmp(token, "controls")) {
            get_token_expect(token, "{");
            while(true) {
                get_token(token);
                if (!strcmp(token, "[")) {
                    controls.push_back(read_vec3());
                    get_token_expect(token, "]");
                } else if(!strcmp(token, "}")) {
                    break;
                } else {
                    fprintf(stderr, "Incorrect format for BezierCurve!\n");
                    exit(0);
                }
            }
        } else if (!strcmp(token, "}")) {
            break;
        } else if (!strcmp(token, "MaterialIndex")) {
            index = read_int();
        } else if (!strcmp(token, "Steps")) {
            steps = read_int();
        } else if (!strcmp(token, "Resolution")) {
            resolution = read_int();
        } else {
            fprintf(stderr, "Incorrect format for BezierCurve!\n");
            exit(0);
        }
    }
    if(index < 0) {
        fprintf(stderr, "No material found for BezierCurve!\n");
        exit(0);
    }
    Sampler *sampler = samplers[index];
    Bxdf *bxdf = bxdfs[index];
    Texture *texture = textures[index];
    Curve *curve = new BezierCurve(controls);
    ResourceGroupMesh *group = new ResourceGroupMesh(
        steps,
        resolution,
        *curve,
        sampler,
        bxdf,
        texture
    );
    group->trans(T);
    group_mesh.push_back(group);
}

void SceneParser::parse_bspline_curve(int current_index, Mat3 T) {
    char token[MAX_PARSER_TOKEN_LENGTH];
    get_token_expect(token, "{");
    std::vector<Vec3> controls;
    int index = current_index;
    int steps = 40;
    int resolution = 30;

    while (true) {
        get_token(token);
        if (!strcmp(token, "controls")) {
            get_token_expect(token, "{");
            while(true) {
                get_token(token);
                if (!strcmp(token, "[")) {
                    controls.push_back(read_vec3());
                    get_token_expect(token, "]");
                } else if(!strcmp(token, "}")) {
                    break;
                } else {
                    fprintf(stderr, "Incorrect format for BsplineCurve!\n");
                    exit(0);
                }
            }
        } else if (!strcmp(token, "}")) {
            break;
        } else if (!strcmp(token, "MaterialIndex")) {
            index = read_int();
        } else if (!strcmp(token, "Steps")) {
            steps = read_int();
        } else if (!strcmp(token, "Resolution")) {
            resolution = read_int();
        } else {
            fprintf(stderr, "Incorrect format for BsplineCurve!\n");
            exit(0);
        }
    }
    if(index < 0) {
        fprintf(stderr, "No material found for BsplineCurve!\n");
        exit(0);
    }
    Sampler *sampler = samplers[index];
    Bxdf *bxdf = bxdfs[index];
    Texture *texture = textures[index];
    Curve *curve = new BsplineCurve(controls);
    ResourceGroupMesh *group = new ResourceGroupMesh(
        steps,
        resolution,
        *curve,
        sampler,
        bxdf,
        texture
    );
    group->trans(T);
    group_mesh.push_back(group);
}






int SceneParser::n_group_sphere() {
    return group_sphere.size();
}

int SceneParser::tot_in_group_sphere() {
    int n = 0;
    for(uint i = 0; i < group_sphere.size(); ++i) {
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
    for(uint i = 0; i < group_light_point.size(); ++i) {
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
    for(uint i = 0; i < group_mesh.size(); ++i) {
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
    for(uint i = 0; i < group_plane.size(); ++i) {
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


int SceneParser::n_group_light_triangle() {
    return group_light_triangle.size();
}

int SceneParser::tot_in_group_light_triangle() {
    int n = 0;
    for(uint i = 0; i < group_light_triangle.size(); ++i) {
        n += group_light_triangle[i]->n_objects();
    }
    return n;
}

ResourceGroupLightTriangle *SceneParser::get_group_light_triangle(int n) {
    return group_light_triangle[n];
}


int SceneParser::n_group_light_global() {
    return group_light_global.size();
}

int SceneParser::tot_in_group_light_global() {
    int n = 0;
    for(uint i = 0; i < group_light_global.size(); ++i) {
        n += group_light_global[i]->n_objects();
    }
    return n;
}

ResourceGroupLightGlobal *SceneParser::get_group_light_global(int n) {
    return group_light_global[n];
}

// accel //////////////////////////////////////////////////////////////////////

template<typename T>
void SceneParser::get_n_light(Accel *accel, std::vector<T *> &group) {
    int m = 0;
    for(uint i = 0; i < group.size(); ++i) {
        m += group[i]->n_objects();
    }
    n_tot += m;
    n_lights += m;
}

template<typename T>
void SceneParser::get_n(Accel *accel, std::vector<T *> &group) {
    int m = 0;
    for(uint i = 0; i < group.size(); ++i) {
        m += group[i]->n_objects();
    }
    n_tot += m;
}

template<typename T>
void SceneParser::add_resource_light(Accel *accel, std::vector<T *> &group) {
    for(uint i = 0; i < group.size(); ++i) {
        accel->add(group[i]);
        for(uint j = 0; j < group[i]->n_objects(); ++j) {
            accel_resource_light.push_back(group[i]->get_info(j));
        }
    }
}

template<typename T>
void SceneParser::add_resource(Accel *accel, std::vector<T *> &group) {
    for(uint i = 0; i < group.size(); ++i) {
        accel->add(group[i]);
        for(uint j = 0; j < group[i]->n_objects(); ++j) {
            accel_resource.push_back(group[i]->get_info(j));
        }
    }
}

Accel *SceneParser::build_accel() {
    // can change data structure here
    // AccelNaive *accel = new AccelNaive();
    AccelBVH *accel = new AccelBVH();

    n_tot = 0;
    n_lights = 0;
    

    get_n_light(accel, group_light_point);
    get_n_light(accel, group_light_direction);
    get_n_light(accel, group_light_triangle);
    get_n_light(accel, group_light_global);
    get_n(accel, group_mesh);
    get_n(accel, group_sphere);
    get_n(accel, group_plane);

    accel->set_size(n_tot);

    // add lights first
    add_resource_light(accel, group_light_point);
    add_resource_light(accel, group_light_direction);
    add_resource_light(accel, group_light_triangle);
    add_resource_light(accel, group_light_global);
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

void SceneParser::get_random_light(RNG *rng, ResourceLight *&light, float &pdf) {
    int x = rng->rand_uint() % n_lights;
    light = accel_resource_light[x];
    pdf = 1.0f / n_lights;
}

float SceneParser::get_pdf_random_light(const RayHit &hit) {
    assert(0 <= hit.get_id() && hit.get_id() < n_lights);
    return 1.0f / n_lights;
}