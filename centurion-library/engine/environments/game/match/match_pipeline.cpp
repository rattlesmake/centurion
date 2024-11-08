#include "match_pipeline.h"
#include "match.h"

#include <environments/game/igame.h>
#include <environments/game/classes/objectsSet/selected_objects.h>

#include <engine.h>
#include <camera.h>
#include <keyboard.h>
#include <mouse.h>

#include <GLFW/glfw3.h>

namespace centurion
{
	namespace game
	{
		match_pipeline::match_pipeline(IGame* creator) : 
			game_pipeline(creator),
			ref_match(*creator->AsMatch())
		{
		}
		void match_pipeline::execute_game_logics(void)
		{
			ref_match.CheckSaveLoadMatch();
		}
		void match_pipeline::handle_controls(void)
		{
			// check condition
			bool b_condition =
				b_gui_any_dialog_window_active == false &&
				b_imgui_capture_keyboard == false;
			if (b_condition == false)
				return;

			// CTRL + S --> quick-save
			if (ref_keyboard.IsKeyPressed(GLFW_KEY_LEFT_CONTROL) == true && ref_keyboard.IsKeyPressed(GLFW_KEY_S) == true)
			{
				igame.AsMatch()->fileToSave = CENTURION_QUICKSAVE;
				return;
			}

			// CTRL + L --> quick-load
			if (ref_keyboard.IsKeyPressed(GLFW_KEY_LEFT_CONTROL) == true && ref_keyboard.IsKeyPressed(GLFW_KEY_L) == true)
			{
				igame.AsMatch()->Load(CENTURION_QUICKSAVE);  // Use always Match::Load  DO NOT use this->fileToLoad = CENTURION_QUICKSAVE
				return;
			}

			// ESC
			if (b_changing_terrain == false && ref_keyboard.IsKeyPressed(GLFW_KEY_ESCAPE))
			{
				handle_controls_ESC();
				return;
			}

			// SPACE or MIDDLE CLICK
			if (ref_keyboard.IsKeyPressed(GLFW_KEY_SPACE) || ref_mouse.MiddleClick)
			{
				handle_controls_SPACE();
				return;
			}
		}
		void match_pipeline::handle_controls_ESC(void)
		{
			auto selection = igame.Selection();
			if (selection->Count() >= 1)
			{
				selection->Reset();

				// If it stays "pressed", esc will open the iframe during iframe->Render()
				// So, it's better to reset it to 0
				ref_keyboard.SetKeyStatus(GLFW_KEY_ESCAPE, 0);
			}
		}
		bool match_pipeline::picking_condition(void)
		{
			if (game_pipeline::picking_condition() == false)
				return false;

			auto match_ui = igame.AsMatch()->GetMatchUI();
			if (match_ui.has_value() && match_ui.value().lock()->IsHovering())
				return false;

			return true;
		}
	};
};
