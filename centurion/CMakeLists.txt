cmake_minimum_required (VERSION 3.8)

project (Centurion)

find_package(OpenGL REQUIRED)

# Add source to this project's executable. 
add_executable(Centurion
  "main.cpp"
)

# Use C++20
target_compile_features(Centurion PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(Centurion PUBLIC "/Zc:__cplusplus")
endif()


target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${OPENGL_gl_LIBRARY}
        third-party/glew #non capisco perche se tolgo external-libs, non trova la .lib
        glfw
        tinyxml2
		sqlite3
		
		#third-party/SFML/lib/sfml-audio-d
		#third-party/SFML/lib/sfml-graphics-d
		#third-party/SFML/lib/sfml-main-d
		#third-party/SFML/lib/sfml-system-d
		#third-party/SFML/lib/sfml-network-d
		#third-party/SFML/lib/sfml-window-d

		rattlesmake-library/rattlesmake-geometry-library/rattlesmake-geometry-library
		rattlesmake-library/rattlesmake-image-library/rattlesmake-image-library
		rattlesmake-library/rattlesmake-peripherals-library/rattlesmake-peripherals-library
		rattlesmake-library/rattlesmake-services-library/rattlesmake-services-library
		rattlesmake-library/rattlesmake-utils-library/rattlesmake-utils-library
		centurion-library/assets/centurion-library-assets
		centurion-library/engine/centurion-library-engine
		centurion-library/gui/centurion-library-gui
		centurion-library/imgui/centurion-library-imgui
		centurion-library/util/centurion-library-utils
)
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::embed)

set_target_properties(Centurion PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Centurion/")

add_custom_command(TARGET Centurion PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/exe-root-content $<TARGET_FILE_DIR:Centurion>)