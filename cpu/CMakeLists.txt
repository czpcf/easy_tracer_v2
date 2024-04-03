cmake_minimum_required(VERSION 3.10)

project(easy_tracer_v2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp -Wall")

set(SOURCE_FILES
    lib/accel/accel_naive.cpp
    lib/accel/accel.cpp
    lib/accel/box.cpp
    lib/bxdf/bxdf.cpp
    lib/bxdf/bxdf_lambertian.cpp
    lib/bxdf/bxdf_phong.cpp
    lib/camera/camera_perspective.cpp
    lib/camera/camera.cpp
    lib/geometry/direction.cpp
    lib/geometry/geometry.cpp
    lib/geometry/plane.cpp
    lib/geometry/point.cpp
    lib/geometry/sphere.cpp
    lib/geometry/triangle.cpp
    lib/image/image.cpp
    lib/light/light_direction.cpp
    lib/light/light_point.cpp
    lib/light/light.cpp
    lib/parser/parser_scene.cpp
    lib/random/random_mt19937.cpp
    lib/random/random.cpp
    lib/ray/ray_hit.cpp
    lib/ray/ray.cpp
    lib/resource/emittor.cpp
    lib/resource/resource_light_direction.cpp
    lib/resource/resource_light_point.cpp
    lib/resource/resource_mesh.cpp
    lib/resource/resource_plane.cpp
    lib/resource/resource_sphere.cpp
    lib/resource/resource.cpp
    lib/sampler/onb
    lib/sampler/sampler_lambertian.cpp
    lib/sampler/sampler_reflection.cpp
    lib/sampler/sampler_refraction.cpp
    lib/sampler/sampler.cpp
    lib/tensor/mat3.cpp
    lib/tensor/vec2.cpp
    lib/tensor/vec3.cpp
    lib/tensor/vec4.cpp
    lib/texture/texture_simple.cpp
    lib/texture/texture.cpp
)

# Create a library target
add_library(easy_tracer_v2_lib STATIC ${SOURCE_FILES})

# Set compile options for the library
target_compile_options(easy_tracer_v2_lib PRIVATE -O3 -fopenmp)

# Create executable targets and link with the library
add_executable(test_box test/test_box.cpp)
target_link_libraries(test_box PRIVATE easy_tracer_v2_lib)

add_executable(test_intersection test/test_intersection.cpp)
target_link_libraries(test_intersection PRIVATE easy_tracer_v2_lib)

add_executable(test_rand test/test_rand.cpp)
target_link_libraries(test_rand PRIVATE easy_tracer_v2_lib)

add_executable(test_sampler test/test_sampler.cpp)
target_link_libraries(test_sampler PRIVATE easy_tracer_v2_lib)

add_executable(test_vector test/test_vector.cpp)
target_link_libraries(test_vector PRIVATE easy_tracer_v2_lib)


# shaders
add_executable(shader_phong shader/shader_phong.cpp)
target_link_libraries(shader_phong PRIVATE easy_tracer_v2_lib)
