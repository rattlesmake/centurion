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

set(CENTURION_LIBRARY_GUI_NAME "centurion-library-gui")

set(CENTURION_LIBRARY_GUI_SOURCES
	# GUI
  "dialogWindows.cpp"
  "dialogWindows.h"
  "iframe/iframe.cpp"
  "iframe/iframe.h"
  "iframe/imgui_elements_include.h"
  "iframe/imguiBufferingBar.cpp"
  "iframe/imguiBufferingBar.h"
  "iframe/imguiCheckBox.cpp"
  "iframe/imguiCheckBox.h"
  "iframe/imguiColorPicker.cpp"
  "iframe/imguiColorPicker.h"
  "iframe/imguiComboBox.cpp"
  "iframe/imguiComboBox.h"
  "iframe/imguiEditorTree.cpp"
  "iframe/imguiEditorTree.h"
  "iframe/imguiElement.cpp"
  "iframe/imguiElement.h"
  "iframe/imguiElementWithOptions.cpp"
  "iframe/imguiElementWithOptions.h"
  "iframe/imguiImage.cpp"
  "iframe/imguiImage.h"
  "iframe/imguiImageButton.cpp"
  "iframe/imguiImageButton.h"
  "iframe/imguiInputInt.cpp"
  "iframe/imguiInputInt.h"
  "iframe/imguiMultipleChoice.cpp"
  "iframe/imguiMultipleChoice.h"
  "iframe/imguiProgressBar.cpp"
  "iframe/imguiProgressBar.h"
  "iframe/imguiSlider.cpp"
  "iframe/imguiSlider.h"
  "iframe/imguiSpinner.cpp"
  "iframe/imguiSpinner.h"
  "iframe/imguiText.cpp"
  "iframe/imguiText.h"
  "iframe/imguiTextInput.cpp"
  "iframe/imguiTextInput.h"
  "iframe/imguiTextInputMultiline.cpp"
  "iframe/imguiTextInputMultiline.h"
  "iframe/imguiTextList.cpp"
  "iframe/imguiTextList.h"
  "iframe/imguiTextWrapped.cpp"
  "iframe/imguiTextWrapped.h"
  "iframe/imgui_tooltip.cpp"
  "iframe/imgui_tooltip.h"
  "iframeSkinCollection.cpp"
  "iframeSkinCollection.h"
  "shortcuts.h" 
  "shortcuts.cpp"
  "ui.h" 
  "ui.cpp"
  "menubar.h" 
  "menubar.cpp"
)

set(CENTURION_LIBRARY_GUI_INC_PATH "")

add_library(${CENTURION_LIBRARY_GUI_NAME}
    STATIC
        ${CENTURION_LIBRARY_GUI_SOURCES}
 )

target_include_directories(${CENTURION_LIBRARY_GUI_NAME}
    PUBLIC
        "${CENTURION_LIBRARY_GUI_INC_PATH}"
)

target_compile_features(${CENTURION_LIBRARY_GUI_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${CENTURION_LIBRARY_GUI_NAME} PUBLIC "/Zc:__cplusplus")
endif()

