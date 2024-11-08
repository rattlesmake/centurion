# centurion library

cmake_minimum_required (VERSION 3.8)
find_package(OpenGL REQUIRED)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

if(WIN32)
	add_definitions(/bigobj)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /bigobj")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
endif(WIN32)

set(CENTURION_LIBRARY_UTILS_NAME "centurion-library-utils")

set(CENTURION_LIBRARY_UTILS_SOURCES
  "custom_exceptions.hpp"
  "duration_logger.cpp"
  "duration_logger.h"
  "icons.h"
  "imgui_utils.cpp"
  "imgui_utils.h"
  "memory_logger.cpp"
  "memory_logger.h"
  "binary_file_reader.h"  
  "binary_file_reader.cpp"
  "binary_file_writer.h"  
  "binary_file_writer.cpp"
  "bin_data_interpreter.cpp"
  "bin_data_interpreter.h"
  "stl_extention/fibonacci_heap.h"
  "thread_guard.cpp"
  "thread_guard.h"
  "tinyxml2_utils.h"
  "tinyxml2_utils.cpp"
)

set(CENTURION_LIBRARY_UTILS_INC_PATH "")

add_library(${CENTURION_LIBRARY_UTILS_NAME}
    STATIC
        ${CENTURION_LIBRARY_UTILS_SOURCES}
 )

target_include_directories(${CENTURION_LIBRARY_UTILS_NAME}
    PUBLIC
        "${CENTURION_LIBRARY_UTILS_INC_PATH}"
)

target_compile_features(${CENTURION_LIBRARY_UTILS_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${CENTURION_LIBRARY_UTILS_NAME} PUBLIC "/Zc:__cplusplus")
endif()

