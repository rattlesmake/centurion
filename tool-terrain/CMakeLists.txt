cmake_minimum_required (VERSION 3.8)

project (terrain)

find_package(OpenGL REQUIRED)

include_directories(
	library/engine
	library/imgui
	library/util
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Add source to this project's executable.
add_executable(terrain 
	main.cpp
	library/engine/engine.cpp
	library/engine/engine.h
	library/util/tinyxml2utils.hpp
	library/util/filemanager.h
	library/util/filemanager.cpp
	library/engine/terrain.h
	library/engine/terrain.cpp
	library/engine/user_interface.h
	library/engine/user_interface.cpp
	library/engine/stb_image.h
	
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
target_compile_features(terrain PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(terrain PUBLIC "/Zc:__cplusplus")
endif()


target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${OPENGL_gl_LIBRARY}
        third-party/glew #non capisco perche se tolgo external-libs, non trova la .lib
        glfw
        tinyxml2
		
		rattlesmake-library/rattlesmake-geometry-library/rattlesmake-geometry-library
		rattlesmake-library/rattlesmake-image-library/rattlesmake-image-library
		rattlesmake-library/rattlesmake-peripherals-library/rattlesmake-peripherals-library
		rattlesmake-library/rattlesmake-services-library/rattlesmake-services-library
		rattlesmake-library/rattlesmake-utils-library/rattlesmake-utils-library
)

set_target_properties(terrain PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/terrain/")

add_custom_command(TARGET terrain PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/exe-root-content $<TARGET_FILE_DIR:terrain>)

