/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace rattlesmake
{
	namespace peripherals
	{
		class keyboard
		{
		public:

			/*
				This function has to be called in Init function of
				your engine instance (before while loop)
			*/
			void initialize(void);

			/*
				This function has to be called at the end of
				while loop
			*/
			void end_engine_frame(void);

			/// <summary>
			/// This function detects whenever a button is being pressed.
			/// </summary>
			/// <param name="key_code">Button code. Accepted value is between 0 and 347</param>
			/// <param name="reset">If true, the key will be set to false</param>
			/// <returns>This value it's true if the specified button is begin pressed; it's false otherwise</returns>
			static bool IsKeyPressedStatic(unsigned int key_code, const bool reset = false);
			bool IsKeyPressed(unsigned int key_code, const bool reset = false);

			/// <summary>
			/// This function sets the current status of a button.
			/// </summary>
			/// <param name="key_code">Button code. Accepted value is between 0 and 347</param>
			/// <param name="action">State of the button to be set. Accepted values are 0 = released; 1 = pressed; 2 = hold;</param>
			void SetKeyStatus(unsigned int key_code, int action);

			/// <summary>
			/// This function detects whenever a button is being released.
			/// </summary>
			/// <param name="key_code">Button code. Accepted value is between 0 and 347</param>
			/// <returns>This value it's true if the specified button is begin released; it's false otherwise</returns>
			bool IsKeyReleased(unsigned int key_code);

			/// <summary>
			/// This function detects whenever a button is being hold.
			/// </summary>
			/// <param name="key_code">Button code. Accepted value is between 0 and 347</param>
			/// <returns>This value it's true if the specified button is begin hold; it's false otherwise</returns>
			bool IsKeyHold(unsigned int key_code);

			/// <summary>
			/// This function detects whenever a button is not being pressed or hold.
			/// </summary>
			/// <param name="key_code">Button code. Accepted value is between 0 and 347</param>
			/// <returns>This value it's true if the specified button is not being pressed or hold; it's false otherwise</returns>
			bool IsKeyNotReleased(unsigned int key_code);

			/// <summary>
			/// This function detects whenever any directional button is not being pressed or hold.
			/// </summary>
			/// <returns>This value it's true if any directional button is not being pressed or hold; it's false otherwise</returns>
			bool IsAnyDirectionalButtonNotReleased(void);

			/// <summary>
			/// This function gets the char codepoint whenever is being pressed
			/// </summary>
			/// <returns>Char codepoint that should be returned, expressed in integer</returns>
			int GetCharCodepointPressed(void);

			/// <summary>
			/// This function sets the char codepoint, for example from a callback
			/// </summary>
			/// <param name="codepoint">Selected char codepoint.It supports only unsigned int</param>
			void SetCharCodepointPressed(unsigned int codepoint);

			/// <summary>
			/// This function resets all the key status.
			/// </summary>
			void ResetKeys(void);

			bool IsAnyKeyPressed(void);

			static int GetGlfwKeyIdFromString(std::string keyName);

#pragma region Singleton
			///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
			keyboard(const keyboard& other) = delete;
			keyboard& operator=(keyboard const& other) = delete;
			[[nodiscard]] static keyboard& get_instance(void);
			~keyboard();
#pragma endregion

		private:
			keyboard();

			int keyCode[348] = { 0 };
			int charCodepointPressed = -1;

			static std::unordered_map<std::string, uint32_t> keysMap;

			bool bAnyKeyPressed = false;

			static keyboard instance;
		};
	};
};
