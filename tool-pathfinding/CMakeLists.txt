cmake_minimum_required (VERSION 3.8)

project (pathfinding)

find_package(OpenGL REQUIRED)

include_directories(
	library/engine
	library/imgui
	library/util
	library/jpsplus
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Add source to this project's executable.
add_executable(pathfinding 
	main.cpp
	library/engine/engine.cpp
	library/engine/engine.h
	library/engine/grid_graphic.h
	library/engine/grid_graphic.cpp
	library/util/tinyxml2_utils.h
	library/util/filemanager.h
	library/util/filemanager.cpp
	
	
	library/jpsplus/header.h
	library/jpsplus/cell.h
	library/jpsplus/cell.cpp
	library/jpsplus/pathfinding.h
	library/jpsplus/pathfinding.cpp
	library/jpsplus/precomputed_map.h
	library/jpsplus/precomputed_map.cpp
	library/jpsplus/open_list.h
	library/jpsplus/open_list.cpp
	library/jpsplus/dijkstra_floodfill.h
	library/jpsplus/dijkstra_floodfill.cpp
	library/imgui/imgui.cpp 
	library/imgui/imgui_demo.cpp
	library/imgui/imgui_draw.cpp
	library/imgui/imgui_impl_glfw.cpp
	library/imgui/imgui_impl_opengl3.cpp
	library/imgui/imgui_widgets.cpp
	library/imgui/imconfig.h
	library/imgui/imgui.h
	library/imgui/imgui_impl_glfw.h
	library/imgui/imgui_impl_opengl3.h
	library/imgui/imgui_internal.h
	library/imgui/imstb_rectpack.h
	library/imgui/imstb_textedit.h
	library/imgui/imstb_truetype.h
 )

# Use C++20
target_compile_features(pathfinding PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(pathfinding PUBLIC "/Zc:__cplusplus")
endif()


target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${OPENGL_gl_LIBRARY}
        third-party/glew #non capisco perche se tolgo external-libs, non trova la .lib
        third-party/lua/lua-5.4.0/lua #idem
        glfw
        tinyxml2
		stb_image
)

set_target_properties(pathfinding PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/")

add_custom_command(TARGET pathfinding PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/exe-root-content $<TARGET_FILE_DIR:pathfinding>)

