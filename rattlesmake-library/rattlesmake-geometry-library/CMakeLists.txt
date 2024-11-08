# rattlesmake library

cmake_minimum_required (VERSION 3.8)
find_package(OpenGL REQUIRED)

## rattlesmake geometry library

set(RATTLESMAKE_GEOMETRY_LIBRARY_NAME "rattlesmake-geometry-library")

set(RATTLESMAKE_GEOMETRY_LIBRARY_SOURCES
    "rectangle_shader.cpp"
    "circle_shader.cpp"
)

set(RATTLESMAKE_GEOMETRY_LIBRARY_HEADERS
    "rectangle_shader.h"
    "circle_shader.h"
)

set(RATTLESMAKE_GEOMETRY_LIBRARY_INC_PATH "")

add_library(${RATTLESMAKE_GEOMETRY_LIBRARY_NAME}
    STATIC
        ${RATTLESMAKE_GEOMETRY_LIBRARY_SOURCES}
        ${RATTLESMAKE_GEOMETRY_LIBRARY_HEADERS}
 )

target_include_directories(${RATTLESMAKE_GEOMETRY_LIBRARY_NAME}
    PUBLIC
        "${RATTLESMAKE_GEOMETRY_LIBRARY_INC_PATH}"
)

target_compile_features(${RATTLESMAKE_GEOMETRY_LIBRARY_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${RATTLESMAKE_GEOMETRY_LIBRARY_NAME} PUBLIC "/Zc:__cplusplus")
endif()
