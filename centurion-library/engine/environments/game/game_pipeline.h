/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <environments/ienvironment.h>
#include <memory>
#include <functional>
#include <unordered_map>

namespace rattlesmake
{
	namespace peripherals {
		class viewport;
		class mouse;
		class keyboard;
		class camera;
	};
	namespace geometry
	{
		class rectangle_shader;
	}
};
class Engine;
class Adventure;
class Scenario;
class Surface;
class Terrain;
class IGame;
class AreaArray;
class Minimap;
class IGameUI;
class SelectionArea;

namespace centurion
{
	namespace assets
	{
		class xml_entity_shader;
	};

	namespace game
	{
		class game_pipeline
		{
		public:
			
			~game_pipeline();
			
			void execute(void);

			#pragma region Shortcuts
			IGame& igame;
			Engine& ref_engine;
			rattlesmake::peripherals::mouse& ref_mouse;
			rattlesmake::peripherals::keyboard& ref_keyboard;
			rattlesmake::peripherals::camera& ref_camera;
			centurion::assets::xml_entity_shader& ref_entity_shader;
			rattlesmake::geometry::rectangle_shader& ref_rectangle_shader;
			std::shared_ptr<IGameUI> sp_ui;
			std::shared_ptr<Adventure> sp_adventure;
			std::shared_ptr<Scenario> sp_scenario;
			std::shared_ptr<Surface> sp_surface;
			std::shared_ptr<Terrain> sp_terrain;
			std::shared_ptr<Minimap> sp_minimap;
			std::shared_ptr<AreaArray> sp_area_array;
			std::shared_ptr<SelectionArea> sp_selection_area;
			#pragma endregion

			#pragma region Booleans
			bool b_editor = false;
			bool b_match = false;
			bool b_inserting_object = false;
			bool b_shifting_object = false;
			bool b_changing_terrain = false;
			bool b_drawing_areas = false;
			bool b_is_gizmo_active = false;
			bool b_user_is_performing_actions = false;
			bool b_imgui_capture_keyboard = false;
			bool b_gui_any_dialog_window_active = false;
			bool b_gui_any_iframe_focused = false;
			bool b_gui_any_iframe_opened = false;
			bool b_perform_mouse_control = false;
			bool b_game_paused = false;
			#pragma endregion

			#pragma region Other variables
			IEnvironment::Environments v_environment_id = IEnvironment::Environments::e_unknown;
			float v_total_seconds = 0.f;
			#pragma endregion

		protected:

			game_pipeline(IGame* creator);
			game_pipeline() = delete;

			void execute_first_run(void);
			void execute_preparation(void);
			void execute_calculation_draw(void); // tracing, picking
			void execute_tracing(void);
			void execute_picking(void);
			void execute_logics(void);
			virtual void execute_game_logics(void);
			void execute_draw(void);
			void execute_draw_surface(const bool render_only_active_chunks);

			// conditions
			virtual bool picking_condition(void);
			bool tracing_condition(void);

			// handle controls
			virtual void handle_controls(void);
			virtual void handle_controls_SPACE(void);

			// set
			void set_variables(void);
			void set_shared_ptrs(void);
			void set_booleans(void);
		};
	};
};
