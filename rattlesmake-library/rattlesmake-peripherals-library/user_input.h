/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cstdint>
#include <vector>

namespace rattlesmake
{
	namespace peripherals
	{
		class mouse;
		class keyboard;

		class user_input
		{
		public:

			void apply(const bool ui_want_capture_mouse, rattlesmake::peripherals::mouse& mouse, rattlesmake::peripherals::keyboard& keyboard);
			void reset_mouse(void);

			struct user_input_mouse
			{
				bool bMouseMoved = false;
				bool bScrollActive = false;
				bool bMouseClicked = false;

				int action = 0, button = 0;
				float scrollValue = 0.0;
				float xPosition = 0.f, yPosition = 0.f;
			} mouse;

			struct user_input_keyboard
			{
				bool bCodepointPressed = false;
				uint32_t codepointValue = 0;

				struct Key
				{
					int action = 0, key = 0;
				};
				std::vector<Key> keys;
			} keyboard;
		};
	};
};
