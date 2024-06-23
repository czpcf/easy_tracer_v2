/*
this shader only supports lights which have non-zero area
*/
#include <cmath>
#include <iostream>
#include "../lib/easy_tracer_v2.hpp"
#include "omp.h"

using namespace std;

SceneParser parser;
Accel *accel;

const uint max_depth = 4;
const uint samples = 16;

// offset the ray to prevent intersecting with the last face
static Vec3 offset(const Vec3 &ori, const Vec3 &d) {
    return ori + d * 0.001953125f;
}

// https://www.pbr-book.org/4ed/Light_Transport_I_Surface_Reflection/The_Light_Transport_Equation
Vec3 mc(Vec3 ori, Vec3 dir, RNG *rng) {
    Vec3 product_bxdf(1.0f, 1.0f, 1.0f); // product of bxdfs
    float product_pdf = 1.0f; // product of pdf
    float product_beta = 1.0f; // product of cos<norm, light_to_surface>
    Vec3 radiance(0.0f, 0.0f, 0.0f);
    for(uint depth = 0; depth < max_depth; ++depth) {
        Ray ray(offset(ori, dir), dir);
        RayHit hit;
        if(!accel->inter(ray, hit)) {
            break;
        }

        Vec3 normal;
        Bxdf *bxdf;
        Sampler *sampler;
        Surface surface;
        if(parser.is_light(hit)) {
            auto info = parser.get_light_info(hit);
            Vec3 emittor = info->get_emittor();
            surface = info->get_surface(hit.get_local());
            normal = surface.get_normal();
            bxdf = surface.get_bxdf();
            sampler = surface.get_sampler();

            // if light CAN generate THIS ray
            if(info->get_light()->pdf_ray(Ray(hit.get_inter(ray), -dir)) > 0) {
                radiance += emittor * product_bxdf * product_beta / fmax(product_pdf, 1e-6);
            }
        } else {
            auto info = parser.get_info(hit);
            surface = info->get_surface(hit.get_local());
            normal = surface.get_normal();
            bxdf = surface.get_bxdf();
            sampler = surface.get_sampler();
        }
        Vec3 dir_in; // direction from intersection to light
        float pdf;
        if(sampler->sample_in(surface, rng, -dir, dir_in, pdf) == false) {
            break;
        }
        if(pdf > 0) {
            product_pdf *= pdf;
        }
        product_bxdf *= bxdf->phase(surface, dir_in, -dir, normal);
        Vec3 g = bxdf->phase(surface, dir_in, -dir, normal);
        product_beta *= fabs(normal.dot(dir_in));

        ori = hit.get_inter(ray);
        dir = dir_in;

        // russian roulette
        // float q = fmin(0.9f, fmax(0.9f - 0.2f * depth, 0.05f));
        float q = 0.5f;
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

    RNGMT19937 rng(123);

    Image image(width, height);

    // parallel
    int  max_threads = omp_get_max_threads();
    omp_set_num_threads(max_threads);

    for(int t = 0; t < samples; ++t) {
        fprintf(stderr, "samples: [%d/%d]\r", t, samples);
        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            for(int x = id; x < width; x += max_threads) {
                for(int y = 0; y < height; ++y) {
                    Ray camRay = camera->generate_ray(&rng, Vec2(rng.rand_float() + x, rng.rand_float() + y));
                    Vec3 res = mc(camRay.get_origin(), camRay.get_direction(), &rng);
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
