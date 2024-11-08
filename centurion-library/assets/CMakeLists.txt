# centurion library

cmake_minimum_required (VERSION 3.8)
find_package(OpenGL REQUIRED)

set(CENTURION_LIBRARY_ASSETS_NAME "centurion-library-assets")

set(CENTURION_LIBRARY_ASSETS_SOURCES
    # assets
  "xml_typedef.h"
  "xml_classes.h"
  "xml_assets.h"
  "xml_assets.cpp"
  "xml_classes.cpp"
  "xml_class.h"
  "xml_class.cpp"
  "xml_class_enums.h"
  "xml_class_method.h"
  "xml_class_method.cpp"
  "xml_class_command.h"
  "xml_class_command.cpp"
  "xml_value.h"
  "xml_value.cpp" 
  "xml_values.h"
  "xml_values.cpp"
  "xml_script.h"
  "xml_script.cpp"
  "xml_scripts.h"
  "xml_scripts.cpp"
  "xml_entity_structs.h"
  "xml_entity.h"
  "xml_entity.cpp"
  "xml_entity_shader.h"
  "xml_entity_shader.cpp"
  "xml_entity_image.h"
  "xml_entity_image.cpp"
  "xml_entity_layer.h"
  "xml_entity_layer.cpp"
)

set(CENTURION_LIBRARY_ASSETS_INC_PATH "")

add_library(${CENTURION_LIBRARY_ASSETS_NAME}
    STATIC
        ${CENTURION_LIBRARY_ASSETS_SOURCES}
 )

target_include_directories(${CENTURION_LIBRARY_ASSETS_NAME}
    PUBLIC
        "${CENTURION_LIBRARY_ASSETS_INC_PATH}"
)

target_compile_features(${CENTURION_LIBRARY_ASSETS_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${CENTURION_LIBRARY_ASSETS_NAME} PUBLIC "/Zc:__cplusplus")
endif()

