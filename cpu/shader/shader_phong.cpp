#include <iostream>
#include "../lib/easy_tracer_v2.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
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
            Ray camRay = camera->generate_ray(Vec2(x, y));
            RayHit hit;
            if(accel->inter(camRay, hit)) {
                // only support point light and directional light
                assert(parser.is_light(hit) == false);
                auto info = parser.get_info(hit);

                Vec3 inter, norm, color;

                // get intersection on the surface
                inter = hit.get_inter(camRay);

                UV uv;
                Sampler *sampler = nullptr;
                Bxdf *bxdf = nullptr;


                // change local coordinate into uv coordinate
                uv = info->local_to_uv(hit.get_local());
                
                // get the normal on the surface
                norm = info->get_normal(hit.get_local(), uv);
                
                // get the sampler on the surface, although its useless here
                sampler = info->get_sampler(hit.get_local(), uv);
                
                // get the bxdf on the surface
                bxdf = info->get_bxdf(hit.get_local(), uv);

                // get the texture color
                color = info->get_color(hit.get_local(), uv);

                // // or equivalently:
                // info->get_all(hit.get_local(), color, norm, bxdf, sampler);

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
                        light->sample_in_ray(&rng, inter, ray_in, pdf);
                        Vec3 p_light = ray_in.get_origin();

                        // direction from intersection to light
                        Vec3 suf_to_light = (p_light - inter).norm();

                        // direction from intersection on eye
                        Vec3 suf_to_eye = -camRay.get_direction();
                        
                        // get emittor
                        Vec3 emittor = info_light->get_emittor();

                        // calculate color
                        radiance += emittor * bxdf->phase(suf_to_light, suf_to_eye, norm);
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
                        light->sample_in_ray(&rng, inter, ray_in, pdf);
                        Vec3 p_light = ray_in.get_origin();
                        Vec3 suf_to_light = (p_light - inter).norm();
                        Vec3 suf_to_eye = -camRay.get_direction();
                        Vec3 emittor = info_light->get_emittor();
                        radiance += emittor * bxdf->phase(suf_to_light, suf_to_eye, norm);
                    }
                }
                Vec3 g = radiance * color;
                // convert radiance to final color
                image.SetPixel(width - x - 1, height - y - 1, radiance * color);
            } else {
                image.SetPixel(width - x - 1, height - y - 1, parser.get_background_color());
            }
        }
    }
    image.SaveBMP(outputFile.c_str());
    return 0;
}
