/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <environments/game/game_pipeline.h>

class ObjsList;
class GObject;

namespace centurion
{
	namespace game
	{
		class editor_pipeline : public game_pipeline
		{
		public:
			editor_pipeline(IGame* creator);
			void execute_game_logics(void) override;

		protected:

			void handle_controls(void) override;
			void handle_controls_ESC(void);

			bool picking_condition(void) override;

		private:
			
			void check_user_input_actions(void);
			bool check_object_shifting(const std::shared_ptr<ObjsList> selection, std::shared_ptr<ObjsList>& objects_to_shift);
			bool check_object_deletion();

			/*
			**	PIPELINE STAGES
			*/ 
			void perform_object_insertion();
			void perform_objects_shifting();
			void perform_objects_deletion(const std::shared_ptr<ObjsList> selection);
			void perform_terrain_change();

			/*
			**	Deletion Utils
			*/
			bool perform_object_deletion(const std::shared_ptr<GObject>& object_to_delete);
			
			/*
			**	Shifting Utils
			*/
			void initialize_object_shifting(const std::shared_ptr<ObjsList> selection);
			void stop_object_shifting(const uint32_t valid_positions);
			void finalize_object_shifting(void);
			static uint32_t set_shifting_positions(const bool b_only_for_not_placeable);
			static bool b_finalize_shifting;

			/*
			**	Shortcuts
			*/
			Editor& ref_editor;
		};
	};
};
