# rattlesmake library

cmake_minimum_required (VERSION 3.8)
find_package(OpenGL REQUIRED)

## rattlesmake utils library

set(RATTLESMAKE_PERIPHERALS_LIBRARY_NAME "rattlesmake-peripherals-library")

set(RATTLESMAKE_PERIPHERALS_LIBRARY_SOURCES
    "camera.cpp"
    "keyboard.cpp"
    "mouse.cpp"
    "user_input.cpp"
    "viewport.cpp"
)

set(RATTLESMAKE_PERIPHERALS_LIBRARY_HEADERS
    "camera.h"
    "keyboard.h"
    "mouse.h"
    "user_input.h"
    "viewport.h"
)

set(RATTLESMAKE_PERIPHERALS_LIBRARY_INC_PATH "")

add_library(${RATTLESMAKE_PERIPHERALS_LIBRARY_NAME}
    STATIC
        ${RATTLESMAKE_PERIPHERALS_LIBRARY_SOURCES}
        ${RATTLESMAKE_PERIPHERALS_LIBRARY_HEADERS}
 )

target_include_directories(${RATTLESMAKE_PERIPHERALS_LIBRARY_NAME}
    PUBLIC
        "${RATTLESMAKE_PERIPHERALS_LIBRARY_INC_PATH}"
)

target_compile_features(${RATTLESMAKE_PERIPHERALS_LIBRARY_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${RATTLESMAKE_PERIPHERALS_LIBRARY_NAME} PUBLIC "/Zc:__cplusplus")
endif()