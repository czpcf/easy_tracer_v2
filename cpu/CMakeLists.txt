cmake_minimum_required(VERSION 3.10)

project(easy_tracer_v2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp")

set(SOURCE_FILES
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