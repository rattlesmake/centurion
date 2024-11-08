#include "game_pipeline.h"
#include "igame.h"
#include "editor/editor.h"
#include "match/match.h"

#include <engine.h>
#include <camera.h>
#include <keyboard.h>
#include <mouse.h>

#include <xml_entity_shader.h>
#include <rectangle_shader.h>

#include <environments/game/adventure/adventure.h>
#include <environments/game/adventure/scenario/scenario.h>
#include <environments/game/adventure/scenario/minimap.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/terrain/terrain.h>
#include <players/player.h>

#include <imgui.h>
#include <dialogWindows.h>

#include <functional>
#include <unordered_map>
#include <GLFW/glfw3.h>

namespace centurion
{
	namespace game
	{
		game_pipeline::game_pipeline(IGame* creator) :
			igame(*creator),
			ref_engine(Engine::GetInstance()),
			ref_mouse(rattlesmake::peripherals::mouse::get_instance()),
			ref_keyboard(rattlesmake::peripherals::keyboard::get_instance()),
			ref_camera(rattlesmake::peripherals::camera::get_instance()),
			ref_rectangle_shader(rattlesmake::geometry::rectangle_shader::get_instance()),
			ref_entity_shader(centurion::assets::xml_entity_shader::get_instance())
		{
			set_variables();
			set_shared_ptrs();
			set_booleans();
		}
		void game_pipeline::execute(void)
		{
			// First Run
			execute_first_run();

			// Preparation
			execute_preparation();

			// Calculation Draws
			execute_calculation_draw();

			// Logics
			execute_logics();

			// draw 
			execute_draw();
		}
		void game_pipeline::execute_first_run(void)
		{
			if (igame.firstRun == true)
			{
				ref_camera.GoToPoint(1.f, 1.f);
				igame.firstRun = false;
			}
		}
		void game_pipeline::execute_preparation(void)
		{
			// Handle Keyboard and Mouse Controls
			handle_controls();

			// update camera and matrices
			ref_camera.begin_game_frame(sp_minimap->IsOpen(), b_gui_any_iframe_opened, b_perform_mouse_control);

			// update shaders
			igame.shaders_begin_frame(ref_camera.GetProjectionMatrix(), ref_camera.GetViewMatrix(), v_total_seconds, sp_minimap->IsOpen());

			// initialize objects entity
			ref_entity_shader.initialize_draw_data();
		}
		void game_pipeline::execute_calculation_draw(void)
		{
			if (sp_minimap->IsOpen() == false)
			{
				// Tracing
				execute_tracing();

				// Picking
				execute_picking();
			}
		}
		void game_pipeline::execute_tracing(void)
		{
			if (tracing_condition())
			{
				sp_terrain->EnableTracing();
				sp_terrain->RenderActiveChunks(false);
				sp_terrain->DisableTracing();

				std::optional<glm::vec3> colorOpt;
				if (ref_mouse.RightClick == false && ref_mouse.LeftClick == false)
					colorOpt = ref_mouse.GetColorOfLastMousePositionPixel();
				else
					colorOpt = ref_mouse.GetColorOfClickedPixel();

				if (colorOpt.has_value() == false)
					return;

				ref_mouse.SetMouseZNoise(sp_scenario->GetNoiseFromRGBColor(colorOpt.value().r));

				glClear(GL_DEPTH_BUFFER_BIT);
			}
		}
		void game_pipeline::execute_picking(void)
		{
			// Picking scope
			if (picking_condition() == true)
			{
				ref_entity_shader.activate_picking();
				sp_scenario->GetPlayersArray()->Picking();
				ref_entity_shader.deactivate_picking();
			}
		}
		void game_pipeline::execute_logics(void)
		{
			if (sp_minimap->IsOpen() == false)
			{
				// Game Specific Logics
				execute_game_logics(); 

				// GObject Logics
				sp_scenario->GetPlayersArray()->Play(false, false);
			}
			else
			{
				if (ref_engine.GetGameTime().IsFrameMultipleOf(60))
				{
					sp_minimap->Update();
				}
				if (sp_minimap->IsUpdating())
				{
					// GObject Logics
					sp_scenario->GetPlayersArray()->Play(false, true);
				}
				else
				{
					sp_scenario->GetPlayersArray()->Play(true, true);
				}
			}
		}
		void game_pipeline::execute_game_logics(void)
		{
			assert(false); // we must NEVER be here! 
			// this class is an interface.
		}
		void game_pipeline::execute_draw(void)
		{
			if (sp_minimap->IsOpen() == false)
			{
				execute_draw_surface(true);

				ref_entity_shader.render_all_draw_data();

				sp_area_array->Render();
				if (b_user_is_performing_actions == false) 
					sp_selection_area->Render();

				ref_rectangle_shader.draw_queue(); // render hitboxes
			}
			else
			{
				if (sp_minimap->IsUpdating())
				{
					execute_draw_surface(false);
					ref_entity_shader.render_all_draw_data();
				}
				sp_minimap->Render();
				ref_camera.draw_camera_rectangle(); // minimap rectangle
			}

			sp_ui->Render();
		}
		void game_pipeline::execute_draw_surface(const bool render_only_active_chunks)
		{
			if (sp_scenario->IsTracingDebuggingActive() == false) // if tracing debugging is active i don't want to render the surface
			{
				sp_surface->Render(render_only_active_chunks, sp_scenario->IsWireframeActive(), sp_scenario->IsSeaRenderingActive());
			}
			
			sp_surface->RenderSurfaceGrid();
			
			if (sp_scenario->IsCloudRenderingActive())
			{
				sp_surface->RenderClouds();
			}
		}
		bool game_pipeline::picking_condition(void)
		{
			if (ref_mouse.LeftClick == false && ref_mouse.RightClick == false)
				return false;
			if (b_gui_any_iframe_opened || b_gui_any_dialog_window_active)
				return false;
			return true;
		}
		bool game_pipeline::tracing_condition(void)
		{
			bool b_condition = false;
			if (b_editor)
			{
				b_condition = b_changing_terrain || (ref_mouse.LeftClick && b_inserting_object);
			}
			else if (b_match)
			{
				b_condition = ref_mouse.RightClick || ref_mouse.LeftClick;
			}
			return b_condition || sp_scenario->IsTracingDebuggingActive();
		}
		void game_pipeline::handle_controls(void)
		{
		}
		void game_pipeline::handle_controls_SPACE(void)
		{
			if (b_gui_any_iframe_focused == false)
			{
				if (sp_minimap->IsOpen() == true)
				{
					sp_ui->ShowMenubar();
					sp_minimap->Close();
					sp_selection_area->ResetDrawingPoints();
				}
				else
				{
					sp_ui->HideMenubar();
					sp_minimap->Open();
					if (ref_mouse.GetCursorType() != CURSOR_TYPE_DEFAULT)
						ref_mouse.ChangeCursorType(CURSOR_TYPE_DEFAULT);
				}
			}
		}
		void game_pipeline::set_variables(void)
		{
			v_environment_id = ref_engine.GetEnvironmentId();
			v_total_seconds = (float)ref_engine.GetGameTime().GetTotalSeconds();
		}
		void game_pipeline::set_shared_ptrs(void)
		{
			sp_adventure = igame.adventure;
			sp_scenario = sp_adventure->GetScenario();
			sp_minimap = sp_scenario->GetMinimap().lock();
			sp_area_array = sp_scenario->GetAreaArrayPtr();
			sp_selection_area = sp_scenario->GetSelectionAreaPtr().lock();
			sp_surface = sp_scenario->GetSurface();
			sp_terrain = sp_surface->GetTerrain();
			sp_ui = igame.UI;
		}
		void game_pipeline::set_booleans(void)
		{
			b_editor = v_environment_id == IEnvironment::Environments::e_editor;
			b_match = v_environment_id == IEnvironment::Environments::e_match;
			b_inserting_object = b_editor && igame.AsEditor()->IsInsertingObject();
			b_shifting_object = b_editor && igame.AsEditor()->IsShiftingObject();
			b_changing_terrain = b_editor && igame.AsEditor()->IsChangingTerrain();
			b_drawing_areas = b_editor && sp_area_array->IsDrawingArea();
			b_is_gizmo_active = b_editor && sp_area_array->IsGizmoActive();
			b_user_is_performing_actions = b_inserting_object || b_shifting_object || b_changing_terrain || b_drawing_areas || b_is_gizmo_active;
			b_imgui_capture_keyboard = ImGui::GetIO().WantCaptureKeyboard;
			b_gui_any_dialog_window_active = gui::IsAnyDialogWindowActive();
			b_gui_any_iframe_focused = sp_ui->IsAnyIframeFocused();
			b_gui_any_iframe_opened = sp_ui->AnyIframeIsOpened();
			b_perform_mouse_control = !b_gui_any_dialog_window_active && !ref_keyboard.IsAnyDirectionalButtonNotReleased();
			b_game_paused = igame.bIsPaused;
		}
		game_pipeline::~game_pipeline()
		{
			igame.ExecuteInternalMethods();
			if (sp_minimap->IsOpen())
			{
				sp_minimap->go_to_point();
			}
			if (b_game_paused == false)
			{
				ref_engine.GetGameTime().Update();
			}
		}
	};
};


