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
const uint samples = 64;

static Vec3 offset(const Vec3 &p, const Vec3 &d) {
    return p + d * 0.00006103515625f;
}

// offset the ray to prevent intersecting with the last face
static Ray offset(const Ray &ray) {
    return Ray(ray.get_origin() + ray.get_direction() * 0.00006103515625f, ray.get_direction());
}

static float heuristic(float x, float y) {
    return x / (x + y);
}

// https://www.pbr-book.org/4ed/Light_Transport_I_Surface_Reflection/A_Better_Path_Tracer
Vec3 mc(Vec3 ori, Vec3 dir, RNG *rng) {
    Vec3 product_bxdf(1.0f, 1.0f, 1.0f); // product of bxdfs
    float product_pdf = 1.0f; // product of pdf
    float product_beta = 1.0f; // product of cos<norm, light_to_surface>
    Vec3 radiance(0.0f, 0.0f, 0.0f);
    float pdf_last_sampler = 1e20; // last sampling pdf of sampling bxdf
    for(uint depth = 0; depth < max_depth; ++depth) {
        if(product_bxdf.max() < 1e-12) {
            break;
        }
        Ray ray(offset(ori, dir), dir);
        RayHit hit;
        if(!accel->inter(ray, hit)) {
            break;
        }

        Vec3 color, normal_suf;
        Bxdf *bxdf;
        Sampler *sampler;
        Surface surface;

        // intersection on the surface
        Vec3 inter = hit.get_inter(ray);

        if(parser.is_light(hit)) {
            auto info = parser.get_light_info(hit);
            Vec3 emittor = info->get_emittor();

            if(info->get_light()->area_type() == AreaType::INFINITE) {
                // do not support sampling method 1(because the light is infinitely large)
                auto light = info->get_light();
                float pdf_ray = light->pdf_ray(Ray(hit.get_inter(ray), -dir));
                if(pdf_ray > 0) {
                    radiance += emittor * product_bxdf * product_beta / fmax(product_pdf, 1e-12);
                }
            } else {
                surface = info->get_surface(hit.get_local());
                normal_suf = surface.get_normal();
                bxdf = surface.get_bxdf();
                sampler = surface.get_sampler();

                // sampling method 1: sample bxdf
                auto light = info->get_light();

                // reversed ?
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

                    radiance += emittor * product_bxdf * product_beta / fmax(product_pdf, 1e-12) * mis_weight;
                }
                // prevent sampling on THIS light
                // and it wont lose too much accuracy
            }
            break;
        } else {
            auto info = parser.get_info(hit);
            surface = info->get_surface(hit.get_local());
            normal_suf = surface.get_normal();
            bxdf = surface.get_bxdf();
            sampler = surface.get_sampler();
        }
        // if the sampler is not specular
        // then it can sample light on this surface
        if(!sampler->is_specular()) {
            // sampling method 2: sample light
            ResourceLight *resource_light;
            float pdf_choose_light;
            parser.get_random_light(rng, resource_light, pdf_choose_light);
            auto light = resource_light->get_light();

            Ray light_to_suf;
            float pdf_sample_in_ray;
            Vec2 local_light;

            // if successfully sampled a ray...
            if(light->sample_in_ray(rng, inter, light_to_suf, pdf_sample_in_ray, local_light)) {
                /// TODO: may be better?
                // nothing between light and intersection

                
                if(accel->if_inter_dis(offset(light_to_suf), (light_to_suf.get_origin() - inter).len() * 0.999f) == false) {
                    // cosine on surface
                    float cos_suf = fabs(light_to_suf.get_direction().dot(-surface.get_inter_normal()));

                    // NO cosine on light !
                    // because light may be point/directional;
                    if(cos_suf > 1e-9) {
                        Vec3 emittor = resource_light->get_emittor();

                        // if the light is specular
                        if(light->area_type() == AreaType::ZERO ||
                           light->area_type() == AreaType::INFINITE ||
                           light->dir_type() == DirType::SINGULAR) {
                            Vec3 now_bxdf = bxdf->phase(surface, -light_to_suf.get_direction(), -dir, normal_suf);

                            radiance += emittor * product_bxdf * product_beta * now_bxdf *
                                cos_suf * // dont forget about the cosine term !
                                (1.0 / fmax(product_pdf * pdf_choose_light, 1e-6));
                        } else {
                            // pdf of samling on light
                            float pdf_light =
                                pdf_choose_light * // choose THIS light
                                pdf_sample_in_ray; // sample THIS ray

                            float cos_light = fabs(
                                resource_light->get_surface(local_light).get_normal()
                                .dot(light_to_suf.get_direction()));
                            
                            float pdf_bxdf =
                                sampler->pdf(surface, -dir, -light_to_suf.get_direction()) * // sampling bxdf pdf
                                cos_light * // jacobian
                                light->decaying(inter, ori); // decaying of light
                            
                            float mis_weight = heuristic(pdf_light, pdf_bxdf);

                            // geometry term
                            float G = cos_suf * cos_light / (inter - light_to_suf.get_origin()).square();

                            Vec3 now_bxdf = bxdf->phase(surface, -light_to_suf.get_direction(), -dir, normal_suf);

                            radiance += emittor * product_bxdf * product_beta * now_bxdf *
                                (G * mis_weight / fmax(product_pdf * pdf_light, 1e-12));
                        }
                    }
                }
            }
        }

        Vec3 dir_in; // direction from intersection to light
        if(sampler->sample_in(surface, rng, -dir, dir_in, pdf_last_sampler) == false) {
            break;
        }
        if(pdf_last_sampler > 0) {
            product_pdf *= pdf_last_sampler;
        } else {
            product_pdf *= -pdf_last_sampler;
            pdf_last_sampler = 1e30; // convert to infinite continous pdf
        }
        product_bxdf *= bxdf->phase(surface, dir_in, -dir, surface.get_inter_normal());
        product_beta *= fabs(surface.get_inter_normal().dot(dir_in));

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
    if(radiance.have_bad() || radiance.min() < 0) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    return min(radiance, Vec3(20.0f, 20.0f, 20.0f));
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

    parser.init(inputFile.c_str());

    Camera *camera = parser.get_camera();
    Vec3 background = parser.get_background_color();
    int width = camera->get_width();
    int height = camera->get_height();

    accel = parser.build_accel();

    Image image(width, height);

    // parallel
    int max_threads = omp_get_max_threads();
    // max_threads = 1;
    omp_set_num_threads(max_threads);

    RNGMT19937 *rng = new RNGMT19937[max_threads];
    for(int i = 0; i < max_threads; ++i) {
        rng[i].init(i);
    }

    for(int t = 0; t < samples; ++t) {
        fprintf(stderr, "samples: [%d/%d]\r", t, samples);
        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            for(int x = id; x < width; x += max_threads) {
                for(int y = 0; y < height; ++y) {
                    Ray camRay;
                    float pdf;
                    camera->sample_ray(x, y, rng, camRay, pdf);
                    Vec3 res = mc(camRay.get_origin(), camRay.get_direction(), &rng[id]);
                    image.add_pixel(x, y, res);
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
