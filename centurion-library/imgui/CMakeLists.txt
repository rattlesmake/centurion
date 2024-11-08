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

set(CENTURION_LIBRARY_IMGUI_NAME "centurion-library-imgui")

set(CENTURION_LIBRARY_IMGUI_SOURCES
  "imconfig.h"
  "imgui.cpp"
  "imgui.h"
  "imgui_demo.cpp"
  "imgui_draw.cpp"
  "imgui_impl_glfw.cpp"
  "imgui_impl_glfw.h"
  "imgui_impl_opengl3.cpp"
  "imgui_impl_opengl3.h"
  "imgui_internal.h"
  "imgui_tables.cpp"
  "imgui_widgets.cpp"
  "imstb_rectpack.h"
  "imstb_textedit.h"
  "imstb_truetype.h"
  "ImFileDialog.h"
  "ImFileDialog.cpp"
)

set(CENTURION_LIBRARY_IMGUI_INC_PATH "")

add_library(${CENTURION_LIBRARY_IMGUI_NAME}
    STATIC
        ${CENTURION_LIBRARY_IMGUI_SOURCES}
 )

target_include_directories(${CENTURION_LIBRARY_IMGUI_NAME}
    PUBLIC
        "${CENTURION_LIBRARY_IMGUI_INC_PATH}"
)

target_compile_features(${CENTURION_LIBRARY_IMGUI_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${CENTURION_LIBRARY_IMGUI_NAME} PUBLIC "/Zc:__cplusplus")
endif()

