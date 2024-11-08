cmake_minimum_required (VERSION 3.8)

project ("Opengl_Framework")

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CURRENT_PROJECT_NAME "centurion")
set(OUTPUT_PATH_DIRECTORY "${CMAKE_BINARY_DIR}/${CURRENT_PROJECT_NAME}")
message("-- Output main root: ${OUTPUT_PATH_DIRECTORY}")

include_directories(
	third-party/glfw/include/
	third-party/glew/
	third-party/glm/glm/
	third-party/tinyxml2/
	third-party/sqlite3/
	third-party/pybind11/include/	
	#third-party/SFML/include/
	third-party/py38/include/

	rattlesmake-library/rattlesmake-geometry-library/
	rattlesmake-library/rattlesmake-image-library/
	rattlesmake-library/rattlesmake-peripherals-library/
	rattlesmake-library/rattlesmake-services-library/
	rattlesmake-library/rattlesmake-utils-library/
)

if(${CURRENT_PROJECT_NAME} STREQUAL "centurion")
	include_directories(
		centurion-library/assets/
		centurion-library/engine/
		centurion-library/gui/
		centurion-library/imgui/
		centurion-library/opengl/
		centurion-library/util/
	)
endif()


link_directories(
	third-party/py38/libs/
)

add_subdirectory(third-party)
add_subdirectory(rattlesmake-library)

if(${CURRENT_PROJECT_NAME} STREQUAL "centurion")
	add_subdirectory(centurion-library)
endif()

add_subdirectory(${CURRENT_PROJECT_NAME}) 
