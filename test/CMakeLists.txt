cmake_minimum_required (VERSION 3.8)

project (TestingProject)

find_package(OpenGL REQUIRED)

#include_directories()

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Add source to this project's executable.
add_executable(TestingProject 
	main.cpp
   )

# Use C++20
target_compile_features(TestingProject PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(TestingProject PUBLIC "/Zc:__cplusplus")
endif()


target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${OPENGL_gl_LIBRARY}
        third-party/glew #non capisco perche se tolgo external-libs, non trova la .lib
        glfw
        tinyxml2
		stb_image
		sqlite3
)
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::embed)

set_target_properties(TestingProject PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TestingProject/")

add_custom_command(TARGET TestingProject PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/exe-root-content $<TARGET_FILE_DIR:TestingProject>)
