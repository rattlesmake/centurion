#if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
#	add_definitions( "/MP" )
#endif()

#add_subdirectory(SFML)


add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

###--------------------------------------------------###
###--------------------------------------------------###

### GLFW ### caso particolare perche glfw ha il suo cmakelists

set(GLFW_LIB_NAME "GLFW")
set(GLFW_INC_PATH ${GLFW_LIB_NAME}/include)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

add_subdirectory(${GLFW_LIB_NAME})

### GLEW ###

set(GLEW_LIB_NAME "GLEW")

set(GLEW_SOURCE
	glew/GL/glew.c
)

set(GLEW_HEADERS
    glew/gl/glew.h
)

add_library( GLEW STATIC
	${GLEW_SOURCE}
	${GLEW_HEADERS}
  )

target_link_libraries(GLEW
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

set(GLEW_INC_PATH ${GLEW_LIB_NAME})

target_include_directories(${GLEW_LIB_NAME}
    PUBLIC
        "${GLEW_INC_PATH}"
)

### GLM ###

add_subdirectory(glm) 

### TINYXML2 ###

set(TINYXML2_LIB_NAME "tinyxml2")

set(TINYXML2_SOURCES
    "${TINYXML2_LIB_NAME}/tinyxml2.cpp"
)

set(TINYXML2_HEADERS
    "${TINYXML2_LIB_NAME}/tinyxml2.h"
)

set(TINYXML2_INC_PATH "${TINYXML2_LIB_NAME}/")

add_library(${TINYXML2_LIB_NAME}
    STATIC
        ${TINYXML2_SOURCES}
        ${TINYXML2_HEADERS}
 )

target_include_directories(${TINYXML2_LIB_NAME}
    PUBLIC
        "${TINYXML2_INC_PATH}"
)

### SQLITE3 ###

set(SQLITE3_LIB_NAME "sqlite3")

set(SQLITE3_SOURCES
    "${SQLITE3_LIB_NAME}/sqlite3.c"
)

set(SQLITE3_HEADERS
    "${SQLITE3_LIB_NAME}/sqlite3.h"
)

set(SQLITE3_INC_PATH "${SQLITE3_LIB_NAME}/")

add_library(${SQLITE3_LIB_NAME}
    STATIC
        ${SQLITE3_SOURCES}
        ${SQLITE3_HEADERS}
 )

target_include_directories(${SQLITE3_LIB_NAME}
    PUBLIC
        "${SQLITE3_INC_PATH}"
)

## PYBIND11 ##

set(PYBIND11_NOPYTHON true)
set(PYTHON_LIBRARY_DIR  "${OUTPUT_PATH_DIRECTORY}/Lib")
add_subdirectory(pybind11)


###--------------------------------------------------###
###--------------------------------------------------###
