#include <iostream>
#include "../lib/easy_tracer_v2.hpp"

using namespace std;

int main() {
    int width = 1024;
    int height = 1024;
    CameraPerspective camera(
        Vec3(1, 2, -3), 
        Vec3(4, 5, -1), 
        Vec3(9, -3, -2), 
        1024, 1024, 
        30.0f / 180.0f * PI
    );
    int T = 10000000;
    RNGMT19937 rng(123);
    int fails_pdf = 0, fails_sample;
    while(T--) {
        int x = rng.rand_uint() % width;
        int y = rng.rand_uint() % height;
        Ray ray;
        float pdf;
        camera.sample_ray(x, y, &rng, ray, pdf);
        Vec3 d = ray.get_direction();
        // cout << "DIR " << d.x << ' ' << d.y << ' ' << d.z << ' ' << pdf << endl;
        float pdf2 = camera.pdf_ray(ray);
        // cout << pdf << endl;
        fails_pdf += fabs(pdf - fails_pdf) < 1e-6;
        Vec3 inter = ray.get_origin() + d * 1.0f;
        if(camera.sample_in_ray(x, y, &rng, inter, ray, pdf)) {
            // d = ray.get_direction();
            // cout << "YES " << d.x << ' ' << d.y << ' ' << d.z << ' ' << pdf << endl;
    } else {
            // cout << "NO" << endl;
            ++fails_sample;
        }
    }
    cout << fails_pdf << ' ' <<fails_sample << endl;
}