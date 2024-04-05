/*
MIS sampling
m = 2 and n_1 = n_2 = 1
sampling method:
1. sampling bxdf (on the sphere)
2. sampling on light (according to its manifold)
*/
#include <cmath>
#include <iostream>
#include "../lib/easy_tracer_v2.hpp"
#include "omp.h"

using namespace std;

SceneParser parser;
Accel *accel;

const uint max_depth = 5;
const uint samples = 4096;

static Vec3 offset(const Vec3 &p, const Vec3 &d) {
    return p + d * 0.001953125f;
}

// offset the ray to prevent intersecting with the last face
static Ray offset(const Ray &ray) {
    return Ray(ray.get_origin() + ray.get_direction() * 0.001953125f, ray.get_direction());
}

static float heuristic(float x, float y) {
    return x / (x + y);
}

// https://www.pbr-book.org/4ed/Light_Transport_I_Surface_Reflection/A_Better_Path_Tracer
Vec3 mc(Vec3 ori, Vec3 dir, RNG *rng) {
    Vec3 product_color(1.0f, 1.0f, 1.0f); // product of colors/textures
    Vec3 product_bxdf(1.0f, 1.0f, 1.0f); // product of bxdfs
    float product_pdf = 1.0f; // product of pdf
    float product_beta = 1.0f; // product of cos<norm, light_to_surface>
    Vec3 radiance(0.0f, 0.0f, 0.0f);
    float pdf_last_sampler = 1e20; // last sampling pdf of sampling bxdf
    for(uint depth = 0; depth < max_depth; ++depth) {
        Ray ray(offset(ori, dir), dir);
        RayHit hit;
        if(!accel->inter(ray, hit)) {
            break;
        }

        Vec3 color, normal_suf;
        Bxdf *bxdf;
        Sampler *sampler;

        // intersection on the surface
        Vec3 inter = hit.get_inter(ray);

        if(parser.is_light(hit)) {
            auto info = parser.get_light_info(hit);
            Vec3 emittor = info->get_emittor();
            info->get_all(hit.get_local(), color, normal_suf, bxdf, sampler);

            // sampling method 1: sample bxdf
            auto light = info->get_light();

            float pdf_ray = light->pdf_ray(Ray(hit.get_inter(ray), -dir));
            if(pdf_ray > 0) {

                // pdf of sampling on light
                float pdf_sample_on_light =
                    parser.get_pdf_random_light(hit) * // pdf of choosing THIS light
                    light->pdf_point(inter); // pdf of sampling THIS point
                
                // pdf of sampling on bxdf
                float pdf_A =
                    pdf_last_sampler * // pdf of sampling bxdf
                    fabs(normal_suf.dot(-dir)) * // jacobian of converting from sphere to manifold
                    light->decaying(inter, ori); // decaying of radiance
                
                // for numerical precisions, some terms are cancelled out,
                // check the light transport equation for details !

                // mis weight
                float mis_weight = heuristic(pdf_A, pdf_sample_on_light);

                radiance += emittor * product_color * product_bxdf * product_beta / fmax(product_pdf, 1e-6) * mis_weight;
            }
        } else {
            auto info = parser.get_info(hit);
            info->get_all(hit.get_local(), color, normal_suf, bxdf, sampler);
        }
        // sampling method 2: sample light
        ResourceLight *resource_light;
        float pdf_choose_light;
        parser.get_random_light(rng, resource_light, pdf_choose_light);
        auto light = resource_light->get_light();

        Ray light_to_suf;
        float pdf_sample_in_ray;
        Vec2 local_light;

        // if successfully sample a ray...
        if(light->sample_in_ray(rng, inter, light_to_suf, pdf_sample_in_ray, local_light)) {
            /// TODO: may be better?
            // nothing between light and intersection

            if(accel->if_inter_id(offset(light_to_suf), hit.get_id()) == true) {
                // cosine on surface
                float cos_suf = fabs(light_to_suf.get_direction().dot(-normal_suf));

                // NO cosine on light !
                // because light may be point/directional;
                if(cos_suf > 1e-4) {
                    Vec3 emittor = resource_light->get_emittor();

                    // if the light is specular
                    if(light->is_specular()) {
                        Vec3 now_bxdf = bxdf->phase(-light_to_suf.get_direction(), -dir, normal_suf);

                        radiance += emittor * product_bxdf * product_color * product_beta * now_bxdf *
                            (1.0 / fmax(product_pdf * pdf_choose_light, 1e-6));
                    } else {
                        // pdf of samling on light
                        float pdf_light =
                            pdf_choose_light * // choose THIS light
                            pdf_sample_in_ray; // sample THIS ray

                        float cos_light = fabs(
                            resource_light->get_normal(local_light, resource_light->local_to_uv(local_light))
                            .dot(light_to_suf.get_direction()));
                        
                        float pdf_bxdf =
                            sampler->pdf(-dir, -light_to_suf.get_direction(), normal_suf) * // sampling bxdf pdf
                            cos_light * // jacobian
                            light->decaying(inter, ori); // decaying of light
                        
                        float mis_weight = heuristic(pdf_light, pdf_bxdf);

                        // geometry term
                        float G = cos_suf * cos_light / (inter - light_to_suf.get_origin()).square();

                        Vec3 now_bxdf = bxdf->phase(-light_to_suf.get_direction(), -dir, normal_suf);

                        radiance += emittor * product_bxdf * product_color * product_beta * now_bxdf *
                            (G * mis_weight / fmax(product_pdf * pdf_light, 1e-6));
                    }
                }
            }
        }

        product_color *= color;
        Vec3 dir_in; // direction from intersection to light
        sampler->sample_in(rng, -dir, normal_suf, dir_in, pdf_last_sampler);
        product_pdf *= pdf_last_sampler;
        product_bxdf *= bxdf->phase(dir_in, -dir, normal_suf);
        Vec3 g = bxdf->phase(dir_in, -dir, normal_suf);
        product_beta *= fabs(normal_suf.dot(dir_in));

        ori = inter;
        dir = dir_in;

        // russian roulette
        float q = fmin(0.9f, fmax(0.9f - 0.2f * depth, 0.05f));
        float r = rng->rand_float();
        if(r >= q) {
            break;
        }
        product_pdf = product_pdf * q;
    }
    return radiance;
}

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

    parser.init(inputFile.c_str());

    Camera *camera = parser.get_camera();
    Vec3 background = parser.get_background_color();
    int width = camera->get_width();
    int height = camera->get_height();

    accel = parser.build_accel();

    Image image(width, height);

    // parallel
    int  max_threads = omp_get_max_threads();
    omp_set_num_threads(max_threads);

    RNGMT19937 *rng = new RNGMT19937[max_threads];
    for(int i = 0; i < max_threads; ++i) {
        rng[i].init(i);
    }

    for(int t = 0; t < samples; ++t) {
        cout << "samples: [" << t << "/" << samples << "]\r";
        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            for(int x = id; x < width; x += max_threads) {
                for(int y = 0; y < height; ++y) {
                    // cout<<"DEBUG "<<endl;
                    // x=800;
                    // y=20;;
                    Ray camRay = camera->generate_ray(Vec2(rng[id].rand_float() + x, rng[id].rand_float() + y));
                    Vec3 res = mc(camRay.get_origin(), camRay.get_direction(), &rng[id]);
                    image.add_pixel(x, y, res);
                    // cout<<res.x<<" "<<res.y<<" "<<res.z<<endl;
                    // exit(0);
                }
            }
        }
    }
    cout << "samples: [" << samples << "/" << samples << "]\r";
    cout << endl << "ok done" << endl;
    image.divide_by(samples);

    // optional, better visual results
    image.linear_to_srgb();

    image.save_bmp(outputFile.c_str());
    return 0;
}
