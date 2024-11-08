#include "user_input.h"
#include "mouse.h"
#include "keyboard.h"

#include <GLFW/glfw3.h>

namespace rattlesmake
{
	namespace peripherals
	{
		void user_input::apply(const bool ui_want_capture_mouse, rattlesmake::peripherals::mouse& mouse, rattlesmake::peripherals::keyboard& keyboard)
		{
			// 1. Mouse
			if (ui_want_capture_mouse)
				this->reset_mouse();

			// position
			if (this->mouse.bMouseMoved)
			{
				mouse.SetMouseInViewport(this->mouse.xPosition, this->mouse.yPosition);
			}

			// click
			if (this->mouse.bMouseClicked)
			{
				mouse.Input(this->mouse.button, this->mouse.action);
			}

			// scroll
			if (this->mouse.bScrollActive)
			{
				mouse.ScrollValue = this->mouse.scrollValue;
				mouse.ScrollBool = true;
			}

			// 2. Keyboard

			// keys
			for (auto const& key : this->keyboard.keys)
			{
				keyboard.SetKeyStatus(key.key, key.action);
			}

			// codepoint
			if (this->keyboard.bCodepointPressed == true)
			{
				keyboard.SetCharCodepointPressed(this->keyboard.codepointValue);
			}
		}

		void user_input::reset_mouse(void)
		{
			this->mouse.bMouseClicked = false;
			this->mouse.action = GLFW_FALSE;
			this->mouse.button = GLFW_FALSE;
		}
	};
};
