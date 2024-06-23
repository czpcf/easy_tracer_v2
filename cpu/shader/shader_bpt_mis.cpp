/// TODO: this shader is BIASED ! (bug in the code !!!)

/*
bpt sampling with mis
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

const uint max_depth_backward = 3; // from camera to light
const uint max_depth_forward = 3; // from light to camera
const uint samples = 16;

bool debug = 0;

static Vec3 offset(const Vec3 &p, const Vec3 &d) {
    return p + d * 0.00006103515625f;
}

// offset the ray to prevent intersecting with the last face
static Ray offset(const Ray &ray) {
    return Ray(ray.get_origin() + ray.get_direction() * 0.00006103515625f, ray.get_direction());
}

Vec3 mc(
    int pixel_x, int pixel_y, RNG *rng, Camera *camera,
    
    Surface *backward_surfaces, Vec3 *backward_points,
    float *backward_pdfs, Vec3 *backward_values,
    int *backward_ids,
    
    Surface *forward_surfaces, Vec3 *forward_points,
    float *forward_pdfs, Vec3 *forward_values,
    int *forward_ids,
    
    float *stack_pdfs, Vec3 *stack_values,
    int *stack_have, Surface **stack_s, Vec3 *stack_p) {

    Vec3 product_bxdf(1.0f, 1.0f, 1.0f); // product of bxdfs
    float product_pdf = 1.0f; // product of pdf
    float geometry_term = 1.0f;
    Vec3 radiance(0.0f, 0.0f, 0.0f);
    float pdf_last_sampler = 1e20; // last sampling pdf of sampling bxdf
    
    bool backward_hit_light = false;
    bool forward_hit_camera = false;

    int depth_backward = 0;
    int depth_forward = 0;

    Vec3 forward_emittor;
    ResourceLight *forward_resource_light;

    bool last_specular = false;

    float last_convert_geometry;

/////////////////////////////////////////////////////////////////////////////////
// step 1. sampling from camera to light
/////////////////////////////////////////////////////////////////////////////////
    Vec3 ori, dir;
    Vec2 local;
    camera->sample_point(pixel_x, pixel_y, rng, ori, pdf_last_sampler, local);
    product_pdf *= fabs(pdf_last_sampler);
    backward_points[0] = ori;
    backward_pdfs[0] = product_pdf;
    backward_values[0] = Vec3(1.0f, 1.0f, 1.0f);
    backward_ids[0] = -1; // camera id

    camera->sample_direction(rng, local, dir, pdf_last_sampler);
    product_pdf *= fabs(pdf_last_sampler);
    if(camera->area_type() == AreaType::ZERO ||
       camera->dir_type() == DirType::SINGULAR) {
        last_specular = true;
    } else {
        last_specular = false;
        /// TODO: ???????
    }
    for(uint depth = 1; depth <= max_depth_backward; ++depth) {
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
            auto light = info->get_light();
            float pdf_ray = light->pdf_ray(Ray(hit.get_inter(ray), -dir));
            // reversed light ?
            if(pdf_ray > 0) {
                Vec3 emittor = info->get_emittor();
                if(depth == 1) {
                    return emittor / product_pdf;
                }
                normal_suf = info->get_surface(hit.get_local()).get_normal();
                if(!last_specular) {
                    // accounts for manifold conversion
                    geometry_term *= fabs(normal_suf.dot(dir)) / (ori - inter).square();
                    product_pdf *= fabs(normal_suf.dot(dir)) / (ori - inter).square();
                }
                ++depth_backward;
                backward_points[depth_backward] = inter;
                backward_pdfs[depth_backward] = product_pdf;
                // a little bit different from other values
                backward_values[depth_backward] = emittor * product_bxdf * geometry_term / product_pdf;
                backward_ids[depth_backward] = hit.get_id();
                // does not need surface info
                backward_hit_light = true;
            }
            break;
        } else {
            auto info = parser.get_info(hit);
            surface = info->get_surface(hit.get_local());
            normal_suf = surface.get_normal();
            bxdf = surface.get_bxdf();
            sampler = surface.get_sampler();
        }

        if(!last_specular) {
            // accounts for manifold conversion
            geometry_term *= fabs(normal_suf.dot(dir)) / (ori - inter).square();
            product_pdf *= fabs(normal_suf.dot(dir)) / (ori - inter).square();
        }

        ++depth_backward;
        backward_points[depth_backward] = inter;
        backward_pdfs[depth_backward] = product_pdf;
        backward_values[depth_backward] = product_bxdf * geometry_term / product_pdf;
        backward_surfaces[depth_backward] = surface;
        backward_ids[depth_backward] = hit.get_id();

        Vec3 dir_in; // direction from camera to light
        if(sampler->sample_in(surface, rng, -dir, dir_in, pdf_last_sampler) == false) {
            break;
        }
        product_pdf *= fabs(pdf_last_sampler);
        if(!sampler->is_specular()) {
            // accounts for light decaying
            geometry_term *= fabs(normal_suf.dot(dir_in));
        }
        last_specular = sampler->is_specular();
        product_bxdf *= bxdf->phase(surface, dir_in, -dir, normal_suf);

        ori = inter;
        dir = dir_in;
    }
/////////////////////////////////////////////////////////////////////////////////
// step 2. sampling from light to camera
/////////////////////////////////////////////////////////////////////////////////
    product_bxdf = Vec3(1.0f, 1.0f, 1.0f);
    geometry_term = 1.0f;

    float pdf_choose_light;
    parser.get_random_light(rng, forward_resource_light, pdf_choose_light);
    auto light = forward_resource_light->get_light();
    light->sample_point(rng, ori, pdf_last_sampler, local);
    product_pdf = fabs(pdf_last_sampler) * pdf_choose_light;
    forward_pdfs[0] = product_pdf;
    forward_points[0] = ori;
    forward_values[0] = Vec3(1.0f, 1.0f, 1.0f) / product_pdf;
    forward_surfaces[0] = forward_resource_light->get_surface(local);
    forward_ids[0] = -2; // emittor id

    light->sample_direction(rng, ori, dir, pdf_last_sampler);

    product_pdf *= fabs(pdf_last_sampler);
    if(light->area_type() != AreaType::FINITE ||
       light->dir_type() == DirType::SINGULAR) {
        last_convert_geometry = 1.0f;
        last_specular = true;
    } else {
        // accounts for manifold conversion
        last_convert_geometry = fabs(forward_resource_light->get_surface(local).get_normal().dot(dir));
        last_specular = false;
    }

    forward_emittor = forward_resource_light->get_emittor();
    for(uint depth = 1; depth <= max_depth_forward; ++depth) {
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
            // ! cannot be the light
            break;
        } else {
            auto info = parser.get_info(hit);
            surface = info->get_surface(hit.get_local());
            normal_suf = surface.get_normal();
            bxdf = surface.get_bxdf();
            sampler = surface.get_sampler();
        }

        if(!sampler->is_specular()) {
            // accounts for light decaying
            geometry_term *= fabs(normal_suf.dot(dir));
            // manifold conversion
            geometry_term *= last_convert_geometry / (inter - ori).square();
        }
        if(!last_specular) {
            product_pdf *= fabs(normal_suf.dot(dir)) / (inter - ori).square();
        }

        ++depth_forward;
        forward_points[depth_forward] = inter;
        forward_pdfs[depth_forward] = product_pdf;
        forward_values[depth_forward] = product_bxdf * geometry_term / product_pdf;
        forward_surfaces[depth_forward] = surface;
        forward_ids[depth_forward] = hit.get_id();

        Vec3 dir_out; // direction from light to camera
        if(sampler->sample_out(surface, rng, -dir, dir_out, pdf_last_sampler) == false) {
            break;
        }
        last_specular = sampler->is_specular();
        product_pdf *= fabs(pdf_last_sampler);
        last_convert_geometry = fabs(normal_suf.dot(dir_out));
        product_bxdf *= bxdf->phase(surface, -dir, dir_out, normal_suf);
        
        ori = inter;
        dir = dir_out;
    }

/////////////////////////////////////////////////////////////////////////////////
// step 3. calculate the radiance
/////////////////////////////////////////////////////////////////////////////////

    bool good_camera =
        (camera->area_type() == AreaType::FINITE) &
        (camera->dir_type() == DirType::CONTINOUS);
    bool good_light =
        (light->area_type() == AreaType::FINITE) &
        (light->dir_type() == DirType::CONTINOUS);
    int max_len = max_depth_backward + max_depth_forward + 1;
    // max_len = 4;
    for(int sum_len = 3; sum_len <= max_len; ++sum_len) {
        int tot = 0;
        for(int len_backward = 1; len_backward <= sum_len && len_backward <= depth_backward + 1; ++len_backward) {
            int len_forward = sum_len - len_backward;
            if(len_forward < 0 || len_forward > depth_forward + 1) {
                continue;
            }
            if(len_backward == sum_len) { // only backward path
                if(backward_hit_light == false || 
                   len_backward != depth_backward + 1) {
                    break;
                }
                // entire backward path
                ++tot;
                stack_pdfs[tot] = backward_pdfs[depth_backward];
                stack_values[tot] = backward_values[depth_backward];
                stack_have[tot] = len_backward;

            } else if(len_backward == 1) { // a single camera
                if(good_camera == false) {
                    continue;
                }
                // if not light and the sampler is specular
                if(len_forward != 1 &&
                    forward_surfaces[len_forward - 1].get_sampler()->is_specular()) {
                    continue;
                }
                // reversed light ?
                if(len_forward == 1) {
                    auto light = forward_resource_light->get_light();
                    float pdf_ray = light->pdf_ray(Ray(forward_points[0], (backward_points[0] - forward_points[0]).norm()));
                    if(!(pdf_ray > 0)) {
                        continue;
                    }
                }
                Ray ray;
                float pdf;
                Vec3 p_on_film;
                if(camera->sample_in_ray(pixel_x, pixel_y, rng, forward_points[len_forward - 1], ray, pdf) == false) {
                    continue;
                }
                if(accel->if_inter_dis(offset(ray), (p_on_film - ray.get_origin()).len() * 0.999f)) {
                    continue;
                }
                Vec3 dir_in = (forward_points[len_forward - 2] - forward_points[len_forward - 1]).norm();
                Vec3 bxdf(1.0f, 1.0f, 1.0f);
                if(len_forward != 1) { // not light
                    Vec3 dir_out = (backward_points[0] - forward_points[len_forward - 1]).norm();
                    bxdf = forward_surfaces[len_forward - 1].get_bxdf()->
                        phase(forward_surfaces[len_forward - 1], dir_in, dir_out, forward_surfaces[len_forward - 1].get_normal());
                }
                Vec3 dir_out = (backward_points[0] - forward_points[len_forward - 1]).norm();
                float cos = fabs(backward_surfaces[len_backward - 1].get_normal().dot(dir_in));
                cos *= fabs(forward_surfaces[len_forward - 1].get_normal().dot(dir_out));
                cos /= (backward_points[0] - forward_points[len_forward - 1]).square();
                Vec3 res = forward_emittor * backward_values[0] * forward_values[len_forward - 1] * bxdf * cos;
                if(res.abs().max() < 1e-6) {
                    continue;
                }
                ++tot;
                stack_pdfs[tot] = backward_pdfs[0] * forward_pdfs[len_forward - 1];
                stack_values[tot] = res;
                stack_have[tot] = len_backward;
                
            } else { // just a common path
                // if not light and the sampler is specular
                Vec3 p_backward = backward_points[len_backward - 1];
                Vec3 p_forward = forward_points[len_forward - 1];
                // hit on the same surface
                if(backward_ids[len_backward - 1] == forward_ids[len_forward - 1]) {
                    continue;
                }
                if(len_forward != 1 &&
                    forward_surfaces[len_forward - 1].get_sampler()->is_specular()) {
                    continue;
                }
                // reversed light ?
                if(len_forward == 1) {
                    auto light = forward_resource_light->get_light();
                    float pdf_ray = light->pdf_ray(Ray(forward_points[0], (p_backward - forward_points[0]).norm()));
                    if(!(pdf_ray > 0)) {
                        continue;
                    }
                }
                // not specular
                if(backward_surfaces[len_backward - 1].get_sampler()->is_specular()) {
                    continue;
                }
                Ray ray(p_backward, (p_forward - p_backward).norm());
                if(accel->if_inter_dis(offset(ray), (p_backward - p_forward).len() * 0.999f)) {
                    continue;
                }
                Vec3 bxdf(1.0f, 1.0f, 1.0f);
                Vec3 dir_in = (p_forward - p_backward).norm();
                Vec3 dir_out = (backward_points[len_backward - 2] - p_backward).norm();

                float cos = 1.0f;
                // light decaying
                cos *= fabs(backward_surfaces[len_backward - 1].get_normal().dot(dir_in));
                // manifold conversion
                cos *= fabs(forward_surfaces[len_forward - 1].get_normal().dot(-dir_in));
                cos /= (p_backward - p_forward).square();

                bxdf *= backward_surfaces[len_backward - 1].get_bxdf()->
                    phase(backward_surfaces[len_backward - 1], dir_in, dir_out, backward_surfaces[len_backward - 1].get_normal());

                if(len_forward != 1) { // not light
                    Vec3 dir_in = (forward_points[len_forward - 2] - p_forward).norm();
                    Vec3 dir_out = (p_forward - forward_points[len_forward - 1]).norm();
                    bxdf *= forward_surfaces[len_forward - 1].get_bxdf()->
                        phase(forward_surfaces[len_forward - 1], dir_in, dir_out, forward_surfaces[len_forward - 1].get_normal());
                }

                Vec3 res = forward_emittor * backward_values[len_backward - 1] * forward_values[len_forward - 1] * bxdf * cos;
                if(res.abs().max() < 1e-6) {
                    continue;
                }
                ++tot;
                stack_pdfs[tot] = backward_pdfs[len_backward - 1] * forward_pdfs[len_forward - 1];
                stack_values[tot] = res;
                stack_have[tot] = len_backward;
                
            }
        }
        // the very bad heuristic adjustment
        // tot=min(tot,1);
        for(int i = 1; i <= tot; ++i) {
            int len_backward_i = stack_have[i];
            float sum_pdf = stack_pdfs[i];
            for(int j = 1; j <= len_backward_i; ++j) {
                stack_p[j] = backward_points[j - 1];
                stack_s[j] = &backward_surfaces[j - 1];
            }
            for(int j = len_backward_i + 1; j <= sum_len; ++j) {
                stack_p[j] = forward_points[sum_len - j];
                stack_s[j] = &forward_surfaces[sum_len - j];
            }
            for(int len_backward_j = 1; len_backward_j <= sum_len; ++len_backward_j) {
                if(len_backward_i == len_backward_j) {
                    continue;
                }
                float pdf = 1.0f;
                // backward path pdf
                pdf *= fabs(camera->pdf_ray(Ray(stack_p[1], (stack_p[2] - stack_p[1]).norm())));
                bool last_specular = good_camera == false;
                for(int u = 2; u <= len_backward_j; ++u) {
                    Vec3 dir_out = (stack_p[u - 1] - stack_p[u]).norm();
                    if(!last_specular) {
                        pdf *= fabs(stack_s[u]->get_normal().dot(dir_out))
                            / (stack_p[u - 1] - stack_p[u]).square();
                    }
                    Vec3 dir_in = (stack_p[u + 1] - stack_p[u]).norm();
                    if(u == len_backward_j) {
                        break;
                    }
                    pdf *= fabs(stack_s[u]->get_sampler()->pdf(
                        *stack_s[u], dir_out, dir_in));
                    last_specular = stack_s[u]->get_sampler()->is_specular();
                }

                if(len_backward_j < sum_len) {
                    // first CHOOSE a light
                    pdf *= parser.get_pdf_random_light(RayHit(0, 0.0f, Vec2()));
                    // forward path pdf
                    pdf *= fabs(forward_resource_light->get_light()->pdf_ray(
                        Ray(stack_p[sum_len], (stack_p[sum_len - 1] - stack_p[sum_len]).norm()))
                    );
                    last_specular = good_light == false;
                    for(int u = sum_len - 1; u >= len_backward_j + 1; --u) {
                        Vec3 dir_in = (stack_p[u + 1] - stack_p[u]).norm();
                        if(!last_specular) {
                            pdf *= fabs(stack_s[u]->get_normal().dot(dir_in))
                                / (stack_p[u + 1] - stack_p[u]).square();
                        }
                        if(u == len_backward_j + 1) {
                            break;
                        }
                        Vec3 dir_out = (stack_p[u - 1] - stack_p[u]).norm();
                        pdf *= fabs(stack_s[u]->get_sampler()->pdf(
                            *stack_s[u], dir_in, dir_out));
                        last_specular = stack_s[u]->get_sampler()->is_specular();
                    }
                }
                sum_pdf += pdf;
            }
            if(debug) {
                cout<<sum_len<<" "<<len_backward_i<<" "<<stack_pdfs[i]<<' '<<sum_pdf<<" "<<stack_values[i].x* (stack_pdfs[i] / sum_pdf)<<endl;
            }
            radiance += stack_values[i] * (stack_pdfs[i] / sum_pdf);
        }
        // break;
    }
    if(radiance.have_bad() || radiance.min() < 0) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    return radiance;
    // return min(radiance, Vec3(20.0f, 20.0f, 20.0f));
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

    Surface *backward_surfaces = new Surface[max_threads * (max_depth_backward + 1)];
    Vec3 *backward_points = new Vec3[max_threads * (max_depth_backward + 1)];
    float *backward_pdfs = new float[max_threads * (max_depth_backward + 1)];
    Vec3 *backward_values = new Vec3[max_threads * (max_depth_backward + 1)];
    int *backward_ids = new int[max_threads * (max_depth_backward + 1)];

    Surface *forward_surfaces = new Surface[max_threads * (max_depth_forward + 1)];
    Vec3 *forward_points = new Vec3[max_threads * (max_depth_forward + 1)];
    float *forward_pdfs = new float[max_threads * (max_depth_forward + 1)];
    Vec3 *forward_values = new Vec3[max_threads * (max_depth_forward + 1)];
    int *forward_ids = new int[max_threads * (max_depth_forward + 1)];

    float *stack_pdfs = new float[max_threads * (max_depth_backward + max_depth_forward + 4)];
    Vec3 *stack_values = new Vec3[max_threads * (max_depth_backward + max_depth_forward + 4)];
    int *stack_have = new int[max_threads * (max_depth_backward + max_depth_forward + 4)];
    Surface **stack_s = new Surface*[max_threads * (max_depth_backward + max_depth_forward + 4)];
    Vec3 *stack_p = new Vec3[max_threads * (max_depth_backward + max_depth_forward + 4)];

    for(int t = 0; t < samples; ++t) {
        fprintf(stderr, "samples: [%d/%d]\r", t, samples);
        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            auto *d_backward_surfaces = backward_surfaces + id * (max_depth_backward + 1);
            auto *d_backward_points = backward_points + id * (max_depth_backward + 1);
            auto *d_backward_pdfs = backward_pdfs + id * (max_depth_backward + 1);
            auto *d_backward_values = backward_values + id * (max_depth_backward + 1);
            auto *d_backward_ids = backward_ids + id * (max_depth_backward + 1);
            
            auto *d_forward_surfaces = forward_surfaces + id * (max_depth_forward + 1);
            auto *d_forward_points = forward_points + id * (max_depth_forward + 1);
            auto *d_forward_pdfs = forward_pdfs + id * (max_depth_forward + 1);
            auto *d_forward_values = forward_values + id * (max_depth_forward + 1);
            auto *d_forward_ids = forward_ids + id * (max_depth_forward + 1);

            auto d_stack_pdfs = stack_pdfs + id * (max_depth_backward + max_depth_forward + 4);
            auto d_stack_values = stack_values + id * (max_depth_backward + max_depth_forward + 4);
            auto d_stack_have = stack_have + id * (max_depth_backward + max_depth_forward + 4);
            auto d_stack_s = stack_s + id * (max_depth_backward + max_depth_forward + 4);
            auto d_stack_p = stack_p + id * (max_depth_backward + max_depth_forward + 4);

            for(int x = id; x < width; x += max_threads) {
                int gg = 0;
                for(int y = 0; y < height; ++y) {
                //     if(gg==0) {
                //         x = 512;
                //         y = 1024-81;
                //     }
                // ++gg;
                // if(gg==1)debug=1;
                    Vec3 res = mc(x, y, rng, camera,
                        d_backward_surfaces, d_backward_points, d_backward_pdfs, d_backward_values, d_backward_ids,
                        d_forward_surfaces, d_forward_points, d_forward_pdfs, d_forward_values, d_forward_ids,
                        d_stack_pdfs, d_stack_values, d_stack_have, d_stack_s, d_stack_p);
                    image.add_pixel(x, y, res);
                    // cout<<res.x<<endl;
                    // if(gg==1)
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
