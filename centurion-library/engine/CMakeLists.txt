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

set(CENTURION_LIBRARY_ENGINE_NAME "centurion-library-engine")

set(CENTURION_LIBRARY_ENGINE_SOURCES
  "header.h"
  "centurion_typedef.hpp"
  "debugUi.cpp"
  "debugUi.h"
  "engine.cpp"
  "engine.h"
  "settings.cpp"
  "settings.h"
  "environments/game/adventure/adventure.cpp"
  "environments/game/adventure/adventure.h"
  "environments/game/adventure/scenario/minimap.cpp"
  "environments/game/adventure/scenario/minimap.h"
  "environments/game/adventure/scenario/area.cpp"
  "environments/game/adventure/scenario/area.h"
  "environments/game/adventure/scenario/area_array.h"
  "environments/game/adventure/scenario/area_array.cpp"
  "environments/game/adventure/scenario/scenario.cpp"
  "environments/game/adventure/scenario/scenario.h"
  "environments/game/adventure/scenario/surface/surface_grid/surface_grid.cpp"
  "environments/game/adventure/scenario/surface/surface_grid/surface_grid.h"
  "environments/game/adventure/scenario/surface/surface_grid/surface_grid_cluster.cpp"
  "environments/game/adventure/scenario/surface/surface_grid/surface_grid_cluster.h"
  "environments/game/adventure/scenario/surface/surface_grid/surface_grid_cluster_entrance.cpp"
  "environments/game/adventure/scenario/surface/surface_grid/surface_grid_cluster_entrance.h"
  "environments/game/adventure/scenario/surface/surface_grid/surface_grid_cell.cpp"
  "environments/game/adventure/scenario/surface/surface_grid/surface_grid_cell.h"

  "environments/game/adventure/scenario/surface/randommap/randomMapDisplacements.cpp"
  "environments/game/adventure/scenario/surface/randommap/randomMapDisplacements.h"
  "environments/game/adventure/scenario/surface/randommap/randomMapGenerator.cpp"
  "environments/game/adventure/scenario/surface/randommap/randomMapGenerator.h"
  "environments/game/adventure/scenario/surface/sea/sea.cpp"
  "environments/game/adventure/scenario/surface/sea/sea.h"
  "environments/game/adventure/scenario/surface/sea/sea_shader.cpp"
  "environments/game/adventure/scenario/surface/sea/sea_shader.h"
  "environments/game/adventure/scenario/surface/clouds/clouds.cpp"
  "environments/game/adventure/scenario/surface/clouds/clouds.h"
  "environments/game/adventure/scenario/surface/clouds/clouds_shader.cpp"
  "environments/game/adventure/scenario/surface/clouds/clouds_shader.h"
  "environments/game/adventure/scenario/surface/terrain/terrain.cpp"
  "environments/game/adventure/scenario/surface/terrain/terrain.h"
  "environments/game/adventure/scenario/surface/terrain/terrainChunk.cpp"
  "environments/game/adventure/scenario/surface/terrain/terrainChunk.h"
  "environments/game/adventure/scenario/surface/terrain/terrainchunk_shader.cpp"
  "environments/game/adventure/scenario/surface/terrain/terrainchunk_shader.h" 
  "environments/game/adventure/scenario/surface/terrain/terrainBrush.cpp"
  "environments/game/adventure/scenario/surface/terrain/terrainBrush.h"
  "environments/game/adventure/scenario/surface/terrain/terrainConstants.h"
  "environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.cpp"
  "environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.h"
  "environments/game/adventure/scenario/surface/textures/terrainTextures.h"
  "environments/game/adventure/scenario/surface/textures/terrainTextures.cpp"
  "environments/game/adventure/scenario/surface/surface.h"
  "environments/game/adventure/scenario/surface/surface.cpp"
  "environments/game/adventure/scenario/surface/surfaceSettings.h"
  "environments/game/adventure/scenario/surface/surfaceSettings.cpp"

  # Environments
  "game/interface/loading_screen.cpp"
  "game/interface/loading_screen.h"
  "environments/ienvironment.h"
  "environments/ienvironment.cpp"
  "environments/file_save_info.h"
  "environments/file_save_info.cpp"
  "environments/game/editor/editor.h"
  "environments/game/editor/editor.cpp"
  "environments/game/editor/editorUi.h"
  "environments/game/editor/editorUi.cpp"
  "environments/game/editor/addObjectWindow.h"
  "environments/game/editor/addObjectWindow.cpp"
  "environments/game/igame.h"
  "environments/game/igame.cpp"
  "environments/game/game_pipeline.h"
  "environments/game/game_pipeline.cpp"
  "environments/game/editor/editor_pipeline.h"
  "environments/game/editor/editor_pipeline.cpp"
  "environments/game/match/match_pipeline.h"
  "environments/game/match/match_pipeline.cpp"
  "environments/game/igameUi.h"
  "environments/game/igameUi.cpp"
  "environments/game/game_time.h"
  "environments/game/game_time.cpp"
  "environments/game/match/match.h"
  "environments/game/match/match.cpp"
  "environments/game/match/matchUI/matchUi.h"
  "environments/game/match/matchUI/matchUi.cpp"
  "environments/game/match/matchUI/topbar.h"
  "environments/game/match/matchUI/topbar.cpp"
  "environments/game/match/matchUI/bottombar.h"
  "environments/game/match/matchUI/bottombar.cpp"
  "environments/game/match/pathfinding/pathfinder.cpp"
  "environments/game/match/pathfinding/pathfinder.h"
  "environments/game/match/pathfinding/node.cpp"
  "environments/game/match/pathfinding/node.h"
  "environments/game/match/pathfinding/astar/astar.cpp"
  "environments/game/match/pathfinding/astar/astar.h"
  "environments/game/match/pathfinding/jpsplus/cell.cpp"
  "environments/game/match/pathfinding/jpsplus/cell.h"
  "environments/game/match/pathfinding/jpsplus/open_list.cpp"
  "environments/game/match/pathfinding/jpsplus/open_list.h"
  "environments/game/match/pathfinding/jpsplus/pathfinding.cpp"
  "environments/game/match/pathfinding/jpsplus/pathfinding.h"
  "environments/game/match/pathfinding/jpsplus/pathfinding_node.h"
  "environments/game/match/pathfinding/jpsplus/precomputed_map.cpp"
  "environments/game/match/pathfinding/jpsplus/precomputed_map.h"
  "environments/menu/menu.h"
  "environments/menu/menu.cpp"
  "mds/mds.h"
  "mds/mds.cpp"
  # Races
  "environments/game/races/race.cpp"
  "environments/game/races/race.h"
  "environments/game/races/races_interface.cpp"
  "environments/game/races/races_interface.h"

  # Classes - classes properties
  "environments/game/classes/objectsAttributes/gobjectAttributes.h"
  "environments/game/classes/objectsAttributes/gobjectAttributes.cpp"
  "environments/game/classes/objectsAttributes/playableAttributes.h"
  "environments/game/classes/objectsAttributes/playableAttributes.cpp"
  "environments/game/classes/objectsAttributes/buildingAttributes.h"
  "environments/game/classes/objectsAttributes/buildingAttributes.cpp"  
  "environments/game/classes/objectsAttributes/heroAttributes.h"
  "environments/game/classes/objectsAttributes/heroAttributes.cpp"  
  "environments/game/classes/objectsAttributes/unitAttributes.h"
  "environments/game/classes/objectsAttributes/unitAttributes.cpp"
  "environments/game/classes/objectsAttributes/wagonAttributes.h"
  "environments/game/classes/objectsAttributes/wagonAttributes.cpp"  
  "environments/game/classes/objectsAttributes/settlementAttributes.h"
  "environments/game/classes/objectsAttributes/settlementAttributes.cpp"  
  # Classes - Single
  "environments/game/classes/gobject.h"
  "environments/game/classes/gobject.cpp"
  "environments/game/classes/playable.h"
  "environments/game/classes/playable.cpp"
  "environments/game/classes/building.h"
  "environments/game/classes/building.cpp"
  "environments/game/classes/decoration.h"
  "environments/game/classes/decoration.cpp"
  "environments/game/classes/unit.h"
  "environments/game/classes/unit.cpp"
  "environments/game/classes/hero.h"
  "environments/game/classes/hero.cpp"
  "environments/game/classes/wagon.h"
  "environments/game/classes/wagon.cpp"
  # Classes - VFX
  "environments/game/classes/VFX/vfxCollection.h"
  "environments/game/classes/VFX/vfxCollection.cpp"
  "environments/game/classes/VFX/vfx.h"
  "environments/game/classes/VFX/vfx.cpp"
  "environments/game/classes/VFX/parabola.h"
  "environments/game/classes/VFX/parabola.cpp"
  # Classes - Collections
  "environments/game/classes/objectsSet/objects_collection.h"
  "environments/game/classes/objectsSet/objects_collection.cpp"
  "environments/game/classes/objectsSet/objects_list.h"
  "environments/game/classes/objectsSet/objects_list.cpp"
  "environments/game/classes/objectsSet/selected_objects.h"
  "environments/game/classes/objectsSet/selected_objects.cpp"
  "environments/game/classes/objectsSet/army.h"
  "environments/game/classes/objectsSet/army.cpp"  
  "environments/game/classes/objectsSet/garrison.h"
  "environments/game/classes/objectsSet/garrison.cpp"
  "environments/game/classes/objectsSet/ordered_units_list.h"
  "environments/game/classes/objectsSet/ordered_units_list.cpp"
  "environments/game/classes/objectsSet/settlement.h"
  "environments/game/classes/objectsSet/settlement.cpp"
  "environments/game/classes/objectsSet/settlement_collection.h"
  "environments/game/classes/objectsSet/settlement_collection.cpp"
  "environments/game/classes/objectsSet/building_settlement_bridge.h"
  "environments/game/classes/objectsSet/building_settlement_bridge.cpp"

  # Classes - Stuff 
  "environments/game/classes/objectsStuff/objectCommand.h"
  "environments/game/classes/objectsStuff/objectCommand.cpp"  
  "environments/game/classes/objectsStuff/objectTarget.h"
  "environments/game/classes/objectsStuff/objectTarget.cpp" 
  "environments/game/classes/objectsStuff/objectGrid.h" 
  "environments/game/classes/objectsStuff/objectGrid.cpp" 
  "environments/game/classes/objectsStuff/objectPoint.h" 
  "environments/game/classes/objectsStuff/objectPoint.cpp" 
  "environments/game/classes/objectsStuff/commandsQueue.h"
  "environments/game/classes/objectsStuff/commandsQueue.cpp"  

  # frame buffers
  "framebuffers/framebuffer.h"
  "framebuffers/framebuffer.cpp"
  "framebuffers/shadows_fb.h"
  "framebuffers/shadows_fb.cpp"

  # Players, diplomacy, colors and races
  "players/color.h"
  "players/color.cpp"
  "players/colors_array.h"
  "players/colors_array.cpp"
  "players/diplomacy.h"
  "players/diplomacy.cpp"
  "players/player.h"
  "players/player.cpp"
  "players/players_array.h"
  "players/players_array.cpp"
	
	# services
  "services/pyservice.cpp"
  "services/pyservice.h"
  "services/logservice.cpp"
  "services/logservice.h"
  "services/oggservice.cpp"
  "services/oggservice.h"
  "services/sqlservice.cpp"
  "services/sqlservice.h"
)

set(CENTURION_LIBRARY_ENGINE_INC_PATH "")

add_library(${CENTURION_LIBRARY_ENGINE_NAME}
    STATIC
        ${CENTURION_LIBRARY_ENGINE_SOURCES}
 )

target_include_directories(${CENTURION_LIBRARY_ENGINE_NAME}
    PUBLIC
        "${CENTURION_LIBRARY_ENGINE_INC_PATH}"
)

target_compile_features(${CENTURION_LIBRARY_ENGINE_NAME} PUBLIC cxx_std_20)
if (MSVC) #Visual Studio compiler
    target_compile_options(${CENTURION_LIBRARY_ENGINE_NAME} PUBLIC "/Zc:__cplusplus")
endif()

