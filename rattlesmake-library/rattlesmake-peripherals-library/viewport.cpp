#include "viewport.h"
#include "keyboard.h"
#include "mouse.h"
#include "camera.h"
#include "user_input.h"

#include <png.h>
#include <ctime>

#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

namespace rattlesmake
{
	namespace peripherals
	{
		viewport viewport::instance;

#pragma region Singleton
		viewport& viewport::get_instance(void)
		{
			return instance;
		}
#pragma endregion

		viewport::viewport() : camera(rattlesmake::peripherals::camera::get_instance()) {}
		viewport::~viewport() {}
		void viewport::initialize(const float vpWidth, const float vpHeight, const bool fullscreen, const std::string& game_name)
		{
			// set width and height
			this->initialize_size(vpWidth, vpHeight);

			// update projection matrix
			this->viewportProjectionMatrix = glm::ortho(0.0f, this->viewportWidth, 0.0f, this->viewportHeight, -100.0f, 100.0f);

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			
			//Windowed
			if (fullscreen == false)
				this->glfwWindow = glfwCreateWindow((int)viewportWidth, (int)viewportHeight, game_name.c_str(), nullptr, nullptr);
			
			//Full Screen
			else
			{
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				this->glfwWindow = glfwCreateWindow((int)viewportWidth, (int)viewportHeight, game_name.c_str(), monitor, nullptr);
				glfwSetWindowMonitor(this->glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			}
			glfwMakeContextCurrent(this->glfwWindow);

			// Initialize GLEW to setup the OpenGL Function pointers
			glewExperimental = GL_TRUE;
			glewInit();
			glfwGetFramebufferSize(glfwWindow, &bufferWidth, &bufferHeight);

			// Define the viewport dimensions
			glViewport(0, 0, bufferWidth, bufferHeight);
			glfwSetWindowUserPointer(glfwWindow, this); // this function is for taking inputs
			this->CreateCallbacks(); // handle key + mouse input

			// Set OpenGL options
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthFunc(GL_LEQUAL);
			glfwSwapInterval(1); // 0 = no v-sync (need to sleep) ; 1 = uses windows vsync
			//glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);	// hide the mouse cursor in the window
		}
		void viewport::begin_engine_frame(const bool gui_capture_mouse)
		{
			this->userInput = new rattlesmake::peripherals::user_input();

			// here GLFW gets the USER INPUT:
			glfwPollEvents();

			// apply the user input
			this->userInput->apply(gui_capture_mouse, mouse::get_instance(), keyboard::get_instance());

			// clear the pointer
			delete this->userInput;
			 
			// clear buffers
			this->ClearBuffers();
		}
		void viewport::end_engine_frame(void)
		{
			this->SwapBuffers();

			// check if window is closed
			if (glfwWindowShouldClose(glfwWindow))
				this->shouldClose = true;
		}
		float viewport::GetWidth(void)
		{
			return this->viewportWidth;
		}
		float viewport::GetWidthZoomed(void)
		{
			return this->viewportWidthZoomed;
		}
		float viewport::GetHeight(void)
		{
			return this->viewportHeight;
		}
		float viewport::GetHeightZoomed(void)
		{
			return this->viewportHeightZoomed;
		}
		float viewport::GetViewportRatio(void)
		{
			return this->viewportRatio;
		}
		glm::vec2 viewport::GetSize(void)
		{
			return glm::vec2(this->viewportWidth, this->viewportHeight);
		}
		glm::vec2 viewport::GetSizeZoomed(void)
		{
			return glm::vec2(this->viewportWidthZoomed, this->viewportHeightZoomed);
		}
		glm::mat4 viewport::GetProjectionMatrix(void) const
		{
			return this->viewportProjectionMatrix;
		}
		glm::mat4 viewport::GetViewMatrix(void) const
		{
			return this->viewportViewMatrix;
		}
		void viewport::initialize_size(const float width, const float height)
		{
			this->SetWidth(width);
			this->SetHeight(height);
			this->UpdateRatio();
			this->UpdateZoomedValues();
		}
		void viewport::SetWidth(const float width)
		{
			this->viewportWidth = width;
		}
		void viewport::SetHeight(const float height)
		{
			this->viewportHeight = height;
		}
		void viewport::UpdateZoomedValues(void)
		{
			this->viewportWidthZoomed = this->viewportWidth + (camera.GetCurrentZoom() - 1.f) * camera.GetZoomFactor();
			this->viewportHeightZoomed = this->viewportHeight + (camera.GetCurrentZoom() - 1.f) * camera.GetZoomFactor() / this->viewportRatio;
		}
		void viewport::UpdateRatio(void)
		{
			this->viewportRatio = this->viewportWidth / this->viewportHeight;
		}
		GLFWwindow* viewport::GetGlfwWindow()
		{
			return this->glfwWindow;
		}
		void viewport::ClearBuffers()
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
		}
		void viewport::ClearAndSwapBuffers(void)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glfwSwapBuffers(glfwWindow);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		void viewport::SwapBuffers(void)
		{
			glfwSwapBuffers(glfwWindow);
		}
		bool viewport::GetShouldClose(void)
		{
			return this->shouldClose;
		}
		void viewport::SetShouldClose(void)
		{
			this->shouldClose = true;
		}
		void viewport::CreateCallbacks()  // important for taking the keyboard / mouse input 
		{
			glfwSetKeyCallback(glfwWindow, handle_keys);
			glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
			glfwSetCursorPosCallback(glfwWindow, handle_mouse);
			glfwSetScrollCallback(glfwWindow, scroll_callback);
			glfwSetCharCallback(glfwWindow, character_callback);
		}
		bool viewport::TakeScreenshot(std::string&& fileName) const
		{
			try
			{
				const int width = static_cast<int>(this->viewportWidth);
				const int height = static_cast<int>(this->viewportHeight);
				const uint8_t nChannels = 3; //3 since is RGB
				std::vector<uint8_t> imageData(static_cast<uint64_t>(width) * height * nChannels);
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glReadBuffer(GL_FRONT);
				glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());
				rattlesmake::image::stb::flip_vertically_on_write(1);
				const int saved = rattlesmake::image::stb::write_png_to_file(fileName.c_str(), width, height, nChannels, imageData.data(), width * nChannels);
				rattlesmake::image::stb::flip_vertically_on_write(0);
				glReadBuffer(GL_BACK);
				return saved;
			}
			catch (...)
			{
				rattlesmake::image::stb::flip_vertically_on_write(0);
				glReadBuffer(GL_BACK);
				return false;
			}
		}

#pragma region Static methods

		void viewport::character_callback(GLFWwindow* window, unsigned int codepoint)
		{
			instance.userInput->keyboard.bCodepointPressed = true;
			instance.userInput->keyboard.codepointValue = codepoint;
		}
		void viewport::handle_mouse(GLFWwindow* window, double xPos, double yPos)
		{
			auto x = (GLfloat)xPos;
			auto y = (GLfloat)(instance.viewportHeight - yPos);

			// set user input
			instance.userInput->mouse.bMouseMoved = true;
			instance.userInput->mouse.xPosition = std::min(std::max(x, 0.f), instance.viewportWidth);
			instance.userInput->mouse.yPosition = std::min(std::max(y, 0.f), instance.viewportHeight);
		}
		void viewport::handle_keys(GLFWwindow* window, int key, int code, int action, int mode)
		{
			if (key >= 0 && key < 348)
			{
				// set user input
				rattlesmake::peripherals::user_input::user_input_keyboard::Key ui_key;
				ui_key.action = action;
				ui_key.key = key;
				instance.userInput->keyboard.keys.push_back(ui_key);
			}
			else
			{
				std::cout << "[DEBUG] The button just pressed is unrecognized. No action will be taken." << std::endl;
				//Logger::LogMessage msg = Logger::LogMessage("The button just pressed is unrecognized. No action will be taken.", "Info", "Engine", "myWindow", __FUNCTION__);
				//Logger::Info(msg);
			}
		}
		void viewport::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
		{
			//if (ImGui::GetIO().WantCaptureMouse) return;

			// set user input
			instance.userInput->mouse.bMouseClicked = true;
			instance.userInput->mouse.button = button;
			instance.userInput->mouse.action = action;
		}
		void viewport::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
		{
			// set user input
			instance.userInput->mouse.bScrollActive = true;
			instance.userInput->mouse.scrollValue = float(yoffset);
		}

#pragma endregion
	};
};
