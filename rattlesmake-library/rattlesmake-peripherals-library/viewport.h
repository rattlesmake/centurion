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

#include <GL/glew.h>
#include <glm.hpp>

struct GLFWwindow;

namespace rattlesmake
{
	namespace peripherals
	{
		class user_input;
		class camera;
		
		class viewport
		{
		public:
			
			/*
				This function has to be called in Init function of 
				your engine instance (before while loop)
				It launches a new GLFW window
			*/
			void initialize(const float vpWidth, const float vpHeight, const bool fullscreen, const std::string& game_name);

			/*
				This function has to be called at the beginning of 
				while loop
			*/
			void begin_engine_frame(const bool gui_capture_mouse);

			/*
				This function has to be called at the end of
				while loop
			*/
			void end_engine_frame(void);

			float GetWidth(void);
			float GetWidthZoomed(void);
			float GetHeight(void);
			float GetHeightZoomed(void);
			float GetViewportRatio(void);
			glm::vec2 GetSize(void);
			glm::vec2 GetSizeZoomed(void);
			glm::mat4 GetProjectionMatrix(void) const;
			glm::mat4 GetViewMatrix(void) const;
			
			void UpdateZoomedValues(void);
			void UpdateRatio(void);

			bool TakeScreenshot(std::string&& fileName) const;
			GLFWwindow* GetGlfwWindow();
			
			bool GetShouldClose(void);
			void SetShouldClose(void);

#pragma region Singleton
			///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
			viewport(const viewport& other) = delete;
			viewport& operator=(viewport const& other) = delete;
			[[nodiscard]] static viewport& get_instance(void);
			~viewport();
#pragma endregion

		private:
			// private because it's singleton
			viewport();

			void initialize_size(const float width, const float height);

			void ClearAndSwapBuffers(void);
			void ClearBuffers();
			void SwapBuffers(void);

			rattlesmake::peripherals::camera& camera;
			rattlesmake::peripherals::user_input* userInput;

			void SetWidth(const float width);
			void SetHeight(const float height);
			void CreateCallbacks();
			GLFWwindow* glfwWindow = nullptr;
			GLint bufferWidth = 0, bufferHeight = 0;
			bool shouldClose = false;
			GLfloat lastX = 0, lastY = 0;

			float viewportWidthZoomed = 1366.f;
			float viewportHeightZoomed = 768.f;
			float viewportWidth = 1366.f;
			float viewportHeight = 768.f;
			float viewportRatio = 1.f;

			glm::mat4 viewportProjectionMatrix{ 1.f };
			glm::mat4 viewportViewMatrix{ 1.f };

			static viewport instance;

#pragma region Static (opengl requires them static) methods

			static void character_callback(GLFWwindow* window, unsigned int codepoint);
			static void handle_mouse(GLFWwindow* window, double xPos, double yPos);
			static void handle_keys(GLFWwindow* window, int key, int code, int action, int mode);
			static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
			static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#pragma endregion
		};
	};
};

