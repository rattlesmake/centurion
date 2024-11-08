# rattlesmake library

cmake_minimum_required (VERSION 3.8)
find_package(OpenGL REQUIRED)

## rattlesmake utils library

set(RATTLESMAKE_UTILS_LIBRARY_NAME "rattlesmake-utils-library")

set(RATTLESMAKE_UTILS_LIBRARY_SOURCES
    "encode_utils.cpp"
    "math_utils.cpp"
    "stl_utils.cpp"
    "new_shader.cpp"
)

set(RATTLESMAKE_UTILS_LIBRARY_HEADERS
    "encode_utils.h"
    "math_utils.h"
    "stl_utils.h"
    "new_shader.h"
)

set(RATTLESMAKE_UTILS_LIBRARY_INC_PATH "")

add_library(${RATTLESMAKE_UTILS_LIBRARY_NAME}
    STATIC
        ${RATTLESMAKE_UTILS_LIBRARY_SOURCES}
        ${RATTLESMAKE_UTILS_LIBRARY_HEADERS}
 )

target_include_directories(${RATTLESMAKE_UTILS_LIBRARY_NAME}
    PUBLIC
        "${RATTLESMAKE_UTILS_LIBRARY_INC_PATH}"
)

target_compile_features(${RATTLESMAKE_UTILS_LIBRARY_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${RATTLESMAKE_UTILS_LIBRARY_NAME} PUBLIC "/Zc:__cplusplus")
endif()