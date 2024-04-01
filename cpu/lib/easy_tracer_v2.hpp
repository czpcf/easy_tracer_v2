#ifndef EASY_TRACER_V2_H
#define EASY_TRACER_V2_H

#include "const.hpp"

#include "accel/box.hpp"

#include "bxdf/bxdf.hpp"
#include "bxdf/bxdf_lambertian.hpp"

#include "geometry/geometry.hpp"
#include "geometry/sphere.hpp"
#include "geometry/triangle.hpp"

#include "random/random.hpp"
#include "random/random_mt19937.hpp"

#include "ray/ray.hpp"
#include "ray/ray_hit.hpp"

#include "resource/resource_group.hpp"

#include "sampler/onb.hpp"
#include "sampler/sampler_lambertian.hpp"
#include "sampler/sampler_reflection.hpp"
#include "sampler/sampler_refraction.hpp"
#include "sampler/sampler.hpp"

#include "tensor/mat3.hpp"
#include "tensor/vec2.hpp"
#include "tensor/vec3.hpp"
#include "tensor/vec4.hpp"


#endif