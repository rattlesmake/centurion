# rattlesmake library

cmake_minimum_required (VERSION 3.8)
find_package(OpenGL REQUIRED)

## rattlesmake image library

include_directories(
	third-party/stb_image/
)

set(RATTLESMAKE_IMAGE_LIBRARY_NAME "rattlesmake-image-library")

set(RATTLESMAKE_IMAGE_LIBRARY_SOURCES
    "stb_wrap.cpp"
    "png.cpp"
    "png_shader.cpp"
)

set(RATTLESMAKE_IMAGE_LIBRARY_HEADERS
    "stb_wrap.h"
    "png.h"
    "png_shader.h"
    "third-party/stb_image/stb_image.h"
    "third-party/stb_image/stb_image_write.h"
)

set(RATTLESMAKE_IMAGE_LIBRARY_INC_PATH "")

add_library(${RATTLESMAKE_IMAGE_LIBRARY_NAME}
    STATIC
        ${RATTLESMAKE_IMAGE_LIBRARY_SOURCES}
        ${RATTLESMAKE_IMAGE_LIBRARY_HEADERS}
  )

target_include_directories(${RATTLESMAKE_IMAGE_LIBRARY_NAME}
    PUBLIC
        "${RATTLESMAKE_IMAGE_LIBRARY_INC_PATH}"
)

target_compile_features(${RATTLESMAKE_IMAGE_LIBRARY_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${RATTLESMAKE_IMAGE_LIBRARY_NAME} PUBLIC "/Zc:__cplusplus")
endif()
