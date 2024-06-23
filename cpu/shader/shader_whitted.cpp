#include <iostream>
#include "../lib/easy_tracer_v2.hpp"

using namespace std;

const int MAX_DEPTH = 20;

static Ray offset(const Ray &ray) {
    return Ray(ray.get_origin() + ray.get_direction() * 0.00006103515625f, ray.get_direction());
}

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        cout << "Usage: <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    SceneParser parser(inputFile.c_str());
    Camera *camera = parser.get_camera();
    int width = camera->get_width();
    int height = camera->get_height();

    Accel *accel = parser.build_accel();

    RNGMT19937 rng(123);

    Image image(width, height);

    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            Ray ray = camera->generate_ray(&rng, Vec2(x, y));
            RayHit hit;
            bool hit_on_diffuse = false;
            int depth = 0;
            Vec3 g(1.0f, 1.0f, 1.0f);
            while(accel->inter(ray, hit) && depth < MAX_DEPTH) {
                auto info = parser.get_info(hit);
                Surface surface = info->get_surface(hit.get_local());
                if(surface.get_sampler()->is_specular()) {
                    ++depth;
                    Vec3 dir_out;
                    float pdf;
                    surface.get_sampler()->sample_out(surface, &rng, -ray.get_direction(), dir_out, pdf);
                    ray = offset(Ray(hit.get_inter(ray), dir_out));
                    g *= surface.get_color();
                } else {
                    hit_on_diffuse = true;
                    break;
                }
            }
            if(hit_on_diffuse) {
                // only support point light and directional light
                if(parser.is_light(hit)) {
                    continue;
                }
                auto info = parser.get_info(hit);

                // get intersection on the surface
                Vec3 inter = hit.get_inter(ray);

                // change local coordinate into uv coordinate
                UV uv = info->local_to_uv(hit.get_local());

                // get surface information
                Surface surface = info->get_surface(hit.get_local());
                Vec3 norm = surface.get_normal();

                // radiance to be calculated
                Vec3 radiance = Vec3(0.0f, 0.0f, 0.0f);

                // enumerate the point light groups
                for(int i = 0; i < parser.n_group_light_point(); ++i) {

                    // enumerate point lights in this group
                    auto group_light_point = parser.get_group_light_point(i);
                    for(int j = 0; j < group_light_point->n_objects(); ++j) {
                        // get the point light
                        auto light = group_light_point->get_light(j);

                        // get the resource of the point light
                        auto info_light = group_light_point->get_info(j);

                        // sample ray from light to intersection
                        float pdf;
                        Ray ray_in;
                        Vec2 local; // local is useless here
                        light->sample_in_ray(&rng, inter, ray_in, pdf, local);
                        Vec3 p_light = ray_in.get_origin();

                        // direction from intersection to light
                        Vec3 suf_to_light = (p_light - inter).norm();

                        // direction from intersection to eye
                        Vec3 suf_to_eye = -ray.get_direction();
                        
                        float rate = 1.0;
                        if(accel->if_inter_dis(offset(Ray(inter, suf_to_light)), (suf_to_light - inter).len()*0.9999f)) {
                            rate = 0.5;
                        }

                        // get emittor
                        Vec3 emittor = info_light->get_emittor();

                        // calculate color
                        radiance += emittor * surface.get_bxdf()->phase(surface, suf_to_light, suf_to_eye, surface.get_inter_normal()) * rate;
                    }
                }
                
                // enumerate the directional light groups
                for(int i = 0; i < parser.n_group_light_direction(); ++i) {
                    auto group_light_direction = parser.get_group_light_direction(i);
                    for(int j = 0; j < group_light_direction->n_objects(); ++j) {
                        auto light = group_light_direction->get_light(j);
                        auto info_light = group_light_direction->get_info(j);
                        float pdf;
                        Ray ray_in;
                        Vec2 local;
                        light->sample_in_ray(&rng, inter, ray_in, pdf, local);
                        Vec3 p_light = ray_in.get_origin();
                        Vec3 suf_to_light = (p_light - inter).norm();
                        Vec3 suf_to_eye = -ray.get_direction();
                        
                        float rate = 1.0;
                        if(accel->if_inter_dis(offset(Ray(inter, suf_to_light)), 1000)) {
                            rate = 0.5;
                        }

                        Vec3 emittor = info_light->get_emittor();
                        radiance += emittor * surface.get_bxdf()->phase(surface, suf_to_light, suf_to_eye, norm) * rate;
                    }
                }

                // convert radiance to final color
                image.set_pixel(x, y, radiance * g);
            } else {
                image.set_pixel(x, y, parser.get_background_color());
            }
        }
    }
    image.save_bmp(outputFile.c_str());
    return 0;
}
