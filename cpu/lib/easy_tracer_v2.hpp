#ifndef EASY_TRACER_V2_H
#define EASY_TRACER_V2_H

#include "const.hpp"

#include "accel/accel_naive.cpp"
#include "accel/accel.cpp"
#include "accel/box.hpp"

#include "bxdf/bxdf_lambertian.hpp"
#include "bxdf/bxdf_phong.hpp"
#include "bxdf/bxdf.hpp"

#include "camera/camera_perspective.hpp"
#include "camera/camera.hpp"

#include "geometry/direction.hpp"
#include "geometry/geometry.hpp"
#include "geometry/plane.hpp"
#include "geometry/point.hpp"
#include "geometry/sphere.hpp"
#include "geometry/triangle.hpp"

#include "image/image.hpp"

#include "light/light_point.hpp"
#include "light/light.hpp"

#include "parser/parser_scene.hpp"

#include "random/random_mt19937.hpp"
#include "random/random.hpp"

#include "ray/ray_hit.hpp"
#include "ray/ray.hpp"

#include "resource/emittor.hpp"
#include "resource/resource_light_direction.hpp"
#include "resource/resource_light_point.hpp"
#include "resource/resource_mesh.hpp"
#include "resource/resource_plane.hpp"
#include "resource/resource_sphere.hpp"
#include "resource/resource.hpp"

#include "sampler/onb.hpp"
#include "sampler/sampler_lambertian.hpp"
#include "sampler/sampler_reflection.hpp"
#include "sampler/sampler_refraction.hpp"
#include "sampler/sampler.hpp"

#include "tensor/mat3.hpp"
#include "tensor/vec2.hpp"
#include "tensor/vec3.hpp"
#include "tensor/vec4.hpp"

#include "texture/texture_simple.hpp"
#include "texture/texture.hpp"

#endif