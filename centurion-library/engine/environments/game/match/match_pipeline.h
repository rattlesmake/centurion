/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <environments/game/game_pipeline.h>

namespace centurion
{
	namespace game
	{
		class match_pipeline : public game_pipeline
		{
		public:
			match_pipeline(IGame* creator);
			void execute_game_logics(void) override;

		protected:

			void handle_controls(void) override;
			void handle_controls_ESC(void);

			bool picking_condition(void) override;

		private:

			/*
			**	Shortcuts
			*/
			Match& ref_match;
		};
	};
};