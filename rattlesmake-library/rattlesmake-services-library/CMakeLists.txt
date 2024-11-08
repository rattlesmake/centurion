# rattlesmake library

cmake_minimum_required (VERSION 3.8)
find_package(OpenGL REQUIRED)

### third-party ###

include_directories(
	third-party/ziplib/
	third-party/bzip2/
	third-party/lzma/
	third-party/zlib/
)
include(third-party/CMakeLists.txt)

## rattlesmake services library

set(RATTLESMAKE_SERVICES_LIBRARY_NAME "rattlesmake-services-library")

set(RATTLESMAKE_SERVICES_LIBRARY_SOURCES
    "fileservice.cpp"
    "zipservice.cpp"
    ${BZIP2_SOURCES}
    ${LZMA_SOURCES}
    ${ZIPLIB_SOURCES}
    ${ZLIB_SOURCES}
)

set(RATTLESMAKE_SERVICES_LIBRARY_HEADERS
    "fileservice.h"
    "zipservice.h"
    ${BZIP2_HEADERS}
    ${LZMA_HEADERS}
    ${ZIPLIB_HEADERS}        
    ${ZLIB_HEADERS}
)

set(RATTLESMAKE_SERVICES_LIBRARY_INC_PATH "")

add_library(${RATTLESMAKE_SERVICES_LIBRARY_NAME}
    STATIC
        ${RATTLESMAKE_SERVICES_LIBRARY_SOURCES}
        ${RATTLESMAKE_SERVICES_LIBRARY_HEADERS}
 )

target_include_directories(${RATTLESMAKE_SERVICES_LIBRARY_NAME}
    PUBLIC
        "${RATTLESMAKE_SERVICES_LIBRARY_INC_PATH}"
)

target_compile_features(${RATTLESMAKE_SERVICES_LIBRARY_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${RATTLESMAKE_SERVICES_LIBRARY_NAME} PUBLIC "/Zc:__cplusplus")
endif()
