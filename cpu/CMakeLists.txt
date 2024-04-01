cmake_minimum_required(VERSION 3.10)

project(easy_tracer_v2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp -Wall")

set(SOURCE_FILES
    lib/accel/box.cpp
    lib/geometry/geometry.cpp
    lib/geometry/triangle.cpp
    lib/random/random.cpp
    lib/random/random_mt19937.cpp
    lib/ray/ray.cpp
    lib/ray/ray_hit.cpp
    lib/tensor/mat3.cpp
    lib/tensor/vec2.cpp
    lib/tensor/vec3.cpp
    lib/tensor/vec4.cpp
)

# Create a library target
add_library(easy_tracer_v2_lib STATIC ${SOURCE_FILES})

# Set compile options for the library
target_compile_options(easy_tracer_v2_lib PRIVATE -O3 -fopenmp)

# Create executable targets and link with the library
add_executable(test_vector test/test_vector.cpp)
target_link_libraries(test_vector PRIVATE easy_tracer_v2_lib)

add_executable(test_rand test/test_rand.cpp)
target_link_libraries(test_rand PRIVATE easy_tracer_v2_lib)