#include "keyboard.h"
#include "viewport.h"
#include "mouse.h"
#include "camera.h"
#include "user_input.h"

#include <GLFW/glfw3.h>

namespace rattlesmake
{
	namespace peripherals
	{
		keyboard keyboard::instance;
		std::unordered_map<std::string, uint32_t> keyboard::keysMap;

		keyboard::keyboard()
		{
		}
		keyboard::~keyboard() { }

		keyboard& keyboard::get_instance(void)
		{
			return instance;
		}
		void keyboard::end_engine_frame(void)
		{
			this->SetCharCodepointPressed(-1);
			this->ResetKeys();
		}
		void keyboard::SetKeyStatus(unsigned int key_code, int action)
		{
			keyCode[key_code] = action;
			if (action != 0)
				bAnyKeyPressed = true;
		}

		bool keyboard::IsKeyReleased(unsigned int key_code)
		{
			return keyCode[key_code] == 0;
		}

		bool keyboard::IsKeyPressedStatic(unsigned int key_code, const bool reset)
		{
			bool b = instance.keyCode[key_code] == 1;
			if (reset && b) instance.SetKeyStatus(key_code, 0);
			return b;
		}

		bool keyboard::IsKeyPressed(unsigned int key_code, const bool reset)
		{
			bool b = this->keyCode[key_code] == 1;
			if (reset && b) this->SetKeyStatus(key_code, 0);
			return b;
		}

		bool keyboard::IsKeyHold(unsigned int key_code)
		{
			return keyCode[key_code] == 2;
		}

		bool keyboard::IsKeyNotReleased(unsigned int key_code)
		{
			return keyCode[key_code] != 0;
		}

		bool keyboard::IsAnyDirectionalButtonNotReleased(void)
		{
			return keyCode[GLFW_KEY_RIGHT] != 0 || keyCode[GLFW_KEY_LEFT] != 0 || keyCode[GLFW_KEY_UP] != 0 || keyCode[GLFW_KEY_DOWN] != 0;
		}

		int keyboard::GetCharCodepointPressed(void)
		{
			return charCodepointPressed;
		}

		void keyboard::SetCharCodepointPressed(unsigned int codepoint)
		{
			if (codepoint > 0)
				charCodepointPressed = codepoint;
			else
				charCodepointPressed = -1;
		}

		bool keyboard::IsAnyKeyPressed(void)
		{
			return this->bAnyKeyPressed;
		}

		void keyboard::ResetKeys(void)
		{
			for (int key = 0; key < GLFW_KEY_LAST; key++)
			{
				if (key == GLFW_KEY_LEFT_CONTROL) continue;
				if (key == GLFW_KEY_RIGHT_CONTROL) continue;
				if (key == GLFW_KEY_LEFT_SHIFT) continue;
				if (key == GLFW_KEY_RIGHT_SHIFT) continue;
				if (key == GLFW_KEY_LEFT_ALT) continue;
				if (key == GLFW_KEY_RIGHT_ALT) continue;
				if (key == GLFW_KEY_RIGHT) continue;
				if (key == GLFW_KEY_LEFT) continue;
				if (key == GLFW_KEY_UP) continue;
				if (key == GLFW_KEY_DOWN) continue;

				keyCode[key] = GLFW_RELEASE;
			}
			bAnyKeyPressed = false;
		}
		int keyboard::GetGlfwKeyIdFromString(std::string keyName)
		{
			if (keysMap.contains(keyName) == false) return -1;
			return (int)keysMap[keyName];
		}
		void keyboard::initialize(void)
		{
			keysMap["KEY_SPACE"] = GLFW_KEY_SPACE;
			keysMap["KEY_APOSTROPHE"] = GLFW_KEY_APOSTROPHE;
			keysMap["KEY_COMMA"] = GLFW_KEY_COMMA;
			keysMap["KEY_MINUS"] = GLFW_KEY_MINUS;
			keysMap["KEY_PERIOD"] = GLFW_KEY_PERIOD;
			keysMap["KEY_SLASH"] = GLFW_KEY_SLASH;
			keysMap["KEY_0"] = GLFW_KEY_0;
			keysMap["KEY_1"] = GLFW_KEY_1;
			keysMap["KEY_2"] = GLFW_KEY_2;
			keysMap["KEY_3"] = GLFW_KEY_3;
			keysMap["KEY_4"] = GLFW_KEY_4;
			keysMap["KEY_5"] = GLFW_KEY_5;
			keysMap["KEY_6"] = GLFW_KEY_6;
			keysMap["KEY_7"] = GLFW_KEY_7;
			keysMap["KEY_8"] = GLFW_KEY_8;
			keysMap["KEY_9"] = GLFW_KEY_9;
			keysMap["KEY_SEMICOLON"] = GLFW_KEY_SEMICOLON;
			keysMap["KEY_EQUAL"] = GLFW_KEY_EQUAL;
			keysMap["KEY_A"] = GLFW_KEY_A;
			keysMap["KEY_B"] = GLFW_KEY_B;
			keysMap["KEY_C"] = GLFW_KEY_C;
			keysMap["KEY_D"] = GLFW_KEY_D;
			keysMap["KEY_E"] = GLFW_KEY_E;
			keysMap["KEY_F"] = GLFW_KEY_F;
			keysMap["KEY_G"] = GLFW_KEY_G;
			keysMap["KEY_H"] = GLFW_KEY_H;
			keysMap["KEY_I"] = GLFW_KEY_I;
			keysMap["KEY_J"] = GLFW_KEY_J;
			keysMap["KEY_K"] = GLFW_KEY_K;
			keysMap["KEY_L"] = GLFW_KEY_L;
			keysMap["KEY_M"] = GLFW_KEY_M;
			keysMap["KEY_N"] = GLFW_KEY_N;
			keysMap["KEY_O"] = GLFW_KEY_O;
			keysMap["KEY_P"] = GLFW_KEY_P;
			keysMap["KEY_Q"] = GLFW_KEY_Q;
			keysMap["KEY_R"] = GLFW_KEY_R;
			keysMap["KEY_S"] = GLFW_KEY_S;
			keysMap["KEY_T"] = GLFW_KEY_T;
			keysMap["KEY_U"] = GLFW_KEY_U;
			keysMap["KEY_V"] = GLFW_KEY_V;
			keysMap["KEY_W"] = GLFW_KEY_W;
			keysMap["KEY_X"] = GLFW_KEY_X;
			keysMap["KEY_Y"] = GLFW_KEY_Y;
			keysMap["KEY_Z"] = GLFW_KEY_Z;
			keysMap["KEY_LEFT_BRACKET"] = GLFW_KEY_LEFT_BRACKET;
			keysMap["KEY_BACKSLASH"] = GLFW_KEY_BACKSLASH;
			keysMap["KEY_RIGHT_BRACKET"] = GLFW_KEY_RIGHT_BRACKET;
			keysMap["KEY_GRAVE_ACCENT"] = GLFW_KEY_GRAVE_ACCENT;
			keysMap["KEY_WORLD_1"] = GLFW_KEY_WORLD_1;
			keysMap["KEY_WORLD_2"] = GLFW_KEY_WORLD_2;
			keysMap["KEY_ESCAPE"] = GLFW_KEY_ESCAPE;
			keysMap["KEY_ENTER"] = GLFW_KEY_ENTER;
			keysMap["KEY_TAB"] = GLFW_KEY_TAB;
			keysMap["KEY_BACKSPACE"] = GLFW_KEY_BACKSPACE;
			keysMap["KEY_INSERT"] = GLFW_KEY_INSERT;
			keysMap["KEY_DELETE"] = GLFW_KEY_DELETE;
			keysMap["KEY_RIGHT"] = GLFW_KEY_RIGHT;
			keysMap["KEY_LEFT"] = GLFW_KEY_LEFT;
			keysMap["KEY_DOWN"] = GLFW_KEY_DOWN;
			keysMap["KEY_UP"] = GLFW_KEY_UP;
			keysMap["KEY_PAGE_UP"] = GLFW_KEY_PAGE_UP;
			keysMap["KEY_PAGE_DOWN"] = GLFW_KEY_PAGE_DOWN;
			keysMap["KEY_HOME"] = GLFW_KEY_HOME;
			keysMap["KEY_END"] = GLFW_KEY_END;
			keysMap["KEY_CAPS_LOCK"] = GLFW_KEY_CAPS_LOCK;
			keysMap["KEY_SCROLL_LOCK"] = GLFW_KEY_SCROLL_LOCK;
			keysMap["KEY_NUM_LOCK"] = GLFW_KEY_NUM_LOCK;
			keysMap["KEY_PRINT_SCREEN"] = GLFW_KEY_PRINT_SCREEN;
			keysMap["KEY_PAUSE"] = GLFW_KEY_PAUSE;
			keysMap["KEY_F1"] = GLFW_KEY_F1;
			keysMap["KEY_F2"] = GLFW_KEY_F2;
			keysMap["KEY_F3"] = GLFW_KEY_F3;
			keysMap["KEY_F4"] = GLFW_KEY_F4;
			keysMap["KEY_F5"] = GLFW_KEY_F5;
			keysMap["KEY_F6"] = GLFW_KEY_F6;
			keysMap["KEY_F7"] = GLFW_KEY_F7;
			keysMap["KEY_F8"] = GLFW_KEY_F8;
			keysMap["KEY_F9"] = GLFW_KEY_F9;
			keysMap["KEY_F10"] = GLFW_KEY_F10;
			keysMap["KEY_F11"] = GLFW_KEY_F11;
			keysMap["KEY_F12"] = GLFW_KEY_F12;
			keysMap["KEY_F13"] = GLFW_KEY_F13;
			keysMap["KEY_F14"] = GLFW_KEY_F14;
			keysMap["KEY_F15"] = GLFW_KEY_F15;
			keysMap["KEY_F16"] = GLFW_KEY_F16;
			keysMap["KEY_F17"] = GLFW_KEY_F17;
			keysMap["KEY_F18"] = GLFW_KEY_F18;
			keysMap["KEY_F19"] = GLFW_KEY_F19;
			keysMap["KEY_F20"] = GLFW_KEY_F20;
			keysMap["KEY_F21"] = GLFW_KEY_F21;
			keysMap["KEY_F22"] = GLFW_KEY_F22;
			keysMap["KEY_F23"] = GLFW_KEY_F23;
			keysMap["KEY_F24"] = GLFW_KEY_F24;
			keysMap["KEY_F25"] = GLFW_KEY_F25;
			keysMap["KEY_KP_0"] = GLFW_KEY_KP_0;
			keysMap["KEY_KP_1"] = GLFW_KEY_KP_1;
			keysMap["KEY_KP_2"] = GLFW_KEY_KP_2;
			keysMap["KEY_KP_3"] = GLFW_KEY_KP_3;
			keysMap["KEY_KP_4"] = GLFW_KEY_KP_4;
			keysMap["KEY_KP_5"] = GLFW_KEY_KP_5;
			keysMap["KEY_KP_6"] = GLFW_KEY_KP_6;
			keysMap["KEY_KP_7"] = GLFW_KEY_KP_7;
			keysMap["KEY_KP_8"] = GLFW_KEY_KP_8;
			keysMap["KEY_KP_9"] = GLFW_KEY_KP_9;
			keysMap["KEY_KP_DECIMAL"] = GLFW_KEY_KP_DECIMAL;
			keysMap["KEY_KP_DIVIDE"] = GLFW_KEY_KP_DIVIDE;
			keysMap["KEY_KP_MULTIPLY"] = GLFW_KEY_KP_MULTIPLY;
			keysMap["KEY_KP_SUBTRACT"] = GLFW_KEY_KP_SUBTRACT;
			keysMap["KEY_KP_ADD"] = GLFW_KEY_KP_ADD;
			keysMap["KEY_KP_ENTER"] = GLFW_KEY_KP_ENTER;
			keysMap["KEY_KP_EQUAL"] = GLFW_KEY_KP_EQUAL;
			keysMap["KEY_LEFT_SHIFT"] = GLFW_KEY_LEFT_SHIFT;
			keysMap["KEY_LEFT_CONTROL"] = GLFW_KEY_LEFT_CONTROL;
			keysMap["KEY_LEFT_ALT"] = GLFW_KEY_LEFT_ALT;
			keysMap["KEY_LEFT_SUPER"] = GLFW_KEY_LEFT_SUPER;
			keysMap["KEY_RIGHT_SHIFT"] = GLFW_KEY_RIGHT_SHIFT;
			keysMap["KEY_RIGHT_CONTROL"] = GLFW_KEY_RIGHT_CONTROL;
			keysMap["KEY_RIGHT_ALT"] = GLFW_KEY_RIGHT_ALT;
			keysMap["KEY_RIGHT_SUPER"] = GLFW_KEY_RIGHT_SUPER;
			keysMap["KEY_MENU"] = GLFW_KEY_MENU;
		}
	};
};
