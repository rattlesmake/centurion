#include "camera.h"
#include "mouse.h"
#include "viewport.h"
#include "keyboard.h"
#include "user_input.h"

#include <rectangle_shader.h>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

#ifndef MOVEMENT_RIGHT
#define MOVEMENT_RIGHT 0
#endif // !MOVEMENT_RIGHT

#ifndef MOVEMENT_LEFT
#define MOVEMENT_LEFT 1
#endif // !MOVEMENT_RIGHT

#ifndef MOVEMENT_TOP
#define MOVEMENT_TOP 2
#endif // !MOVEMENT_RIGHT

#ifndef MOVEMENT_BOTTOM
#define MOVEMENT_BOTTOM 3
#endif // !MOVEMENT_RIGHT


namespace rattlesmake
{
	namespace peripherals
	{
		camera camera::instance;

		camera& camera::get_instance(void)
		{
			return camera::instance;
		}

		camera::camera() :
			viewport(rattlesmake::peripherals::viewport::get_instance()),
			zoom(ZoomInstance()),
			mouse(rattlesmake::peripherals::mouse::get_instance()),
			keyboard(rattlesmake::peripherals::keyboard::get_instance())
		{ }
		camera::~camera() {}
		void camera::initialize(const float maxZoom, const float movementSpeed, const float leftMargin, const float bottomMargin)
		{
			position = glm::vec3(0.0f, 0.0f, 0.0f); // Starting position coordinates (x, y and z)
			worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // Relative position (x, y and z) from the world
			yaw = -90.0f; // Degrees of rotation along the vertical axis
			pitch = 0.0f; // Degrees of rotation along the transversal axis
			front = glm::vec3(0.0f, 0.0f, 1.0f);

			front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			front.y = sin(glm::radians(pitch));
			front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			front = glm::normalize(front);

			right = glm::normalize(glm::cross(front, worldUp));
			up = glm::normalize(glm::cross(right, front));

			this->SetMaxZoom(maxZoom);
			this->SetMovementSpeed(movementSpeed);
			this->leftMargin = leftMargin;
			this->bottomMargin = bottomMargin;
		}
		void camera::new_game(const glm::vec2& visible_map_size, const float topbar_height, const float bottombar_height)
		{
			this->visibleMapSize = visible_map_size;
			this->bottomBarHeight = bottombar_height;
			this->topBarHeight = topbar_height;

			glm::vec2 viewportSize = viewport.GetSize();
			float bottom = -1.f * (this->visibleMapSize.y * bottomBarHeight / (viewportSize.y - bottomBarHeight - topBarHeight));
			float top = this->visibleMapSize.y + this->visibleMapSize.y * topBarHeight / (viewportSize.y - topBarHeight - topBarHeight);
			float left = 0.f;
			float right = this->visibleMapSize.x;
			float z_near_far = 1000000.f;
			this->cameraMinimapProjectionMatrix = glm::ortho(left, right, bottom, top, -z_near_far, z_near_far);
		}
		void camera::begin_engine_frame(const uint32_t current_frame)
		{
			this->currentFrame = current_frame;
		}
		void camera::begin_game_frame(const bool minimap_is_active, const bool any_iframe_opened, const bool perform_mouse_control)
		{
			mouseMovement = perform_mouse_control;

			// control
			if (!minimap_is_active && !any_iframe_opened)
			{
				if (perform_mouse_control)
				{
					this->mouseControl();
				}
				this->keyboardControl();
			}

			// update matrices
			if (!minimap_is_active)
			{
				glm::vec2 viewportSizeZoomed = viewport.GetSizeZoomed();
				this->cameraProjectionMatrix = glm::ortho(0.0f, viewportSizeZoomed.x, 0.0f, viewportSizeZoomed.y, -this->visibleMapSize.x, this->visibleMapSize.x);
				this->cameraViewMatrix = this->CalculateViewMatrix();
			}
			else
			{
				this->cameraProjectionMatrix = this->cameraMinimapProjectionMatrix;
				this->cameraViewMatrix = glm::mat4(1.f);
			}
		}
		void camera::draw_camera_rectangle(void) const
		{
			auto& rect_shader = geometry::rectangle_shader::get_instance();
			int32_t x = (int32_t)this->position.x; 
			int32_t y = (int32_t)this->position.y;
			uint32_t width = (uint32_t)this->viewport.GetWidthZoomed();
			uint32_t height = (uint32_t)this->viewport.GetHeightZoomed();
			rect_shader.draw(x, y, width, height, glm::vec4(255.f), glm::vec4(0.f), 0); //last parameter = bottom-left
		}
		void camera::set_position(glm::vec2 pt)
		{
			this->position.x = pt.x;
			this->position.y = pt.y;
			this->FixCameraPosition();
		}
		void camera::FixCameraDuringZoom(glm::vec2* mousePos)
		{
			if (mousePos == nullptr)
				return;
			const glm::vec2 newMousePos{ mouse.GetXMapCoordinate(), mouse.GetYMapCoordinate() };

			const float dx = mousePos->x - newMousePos.x;
			const float dy = mousePos->y - newMousePos.y;

			position.x += dx;
			position.y += dy;
		}
		void camera::FixCameraPosition(glm::vec2* mousePos)
		{
			this->FixCameraDuringZoom(mousePos);

			float rightMargin = visibleMapSize.x - viewport.GetWidthZoomed() - MovementSpeed;
			float topMargin = visibleMapSize.y - viewport.GetHeightZoomed() - MovementSpeed;
			float topBarOffset = this->topBarHeight * viewport.GetHeightZoomed() / viewport.GetHeight();
			float bottomBarOffset = this->bottomBarHeight * viewport.GetHeightZoomed() / viewport.GetHeight();

			position.x = std::min(std::max(position.x, this->leftMargin), rightMargin);
			position.y = std::min(std::max(position.y, this->bottomMargin - bottomBarOffset), topMargin + topBarOffset);
		}
		void camera::CameraMovement(unsigned int DIRECTION, bool _mouseMovement)
		{
			if (_mouseMovement) mouseMovement = true;
			switch (DIRECTION)
			{
			case MOVEMENT_RIGHT:
				position += right * MovementSpeed;
				break;
			case MOVEMENT_LEFT:
				position -= right * MovementSpeed;
				break;
			case MOVEMENT_TOP:
				position += up * MovementSpeed;
				break;
			case MOVEMENT_BOTTOM:
				position -= up * MovementSpeed;
				break;
			default:
				break;
			}
			this->FixCameraPosition();
		}
		void camera::ManageMouseMovement()
		{
			mouseMovement = false;
			const float MARGIN = 3.0;
			if (mouse.GetXPosition() <= MARGIN) CameraMovement(MOVEMENT_LEFT, true);
			else if (mouse.GetXPosition() >= static_cast<double>(viewport.GetWidth()) - MARGIN) CameraMovement(MOVEMENT_RIGHT, true);
			if (mouse.GetYPosition() >= static_cast<double>(viewport.GetHeight()) - MARGIN) CameraMovement(MOVEMENT_TOP, true);
			else if (mouse.GetYPosition() <= MARGIN) CameraMovement(MOVEMENT_BOTTOM, true);
		}
		void camera::ManageKeyboardMovement()
		{
			if (keyboard.IsKeyNotReleased(GLFW_KEY_LEFT)) CameraMovement(MOVEMENT_LEFT, false);
			else if (keyboard.IsKeyNotReleased(GLFW_KEY_RIGHT)) CameraMovement(MOVEMENT_RIGHT, false);
			if (keyboard.IsKeyNotReleased(GLFW_KEY_UP)) CameraMovement(MOVEMENT_TOP, false);
			else if (keyboard.IsKeyNotReleased(GLFW_KEY_DOWN)) CameraMovement(MOVEMENT_BOTTOM, false);
		}
		bool camera::IsPointInCameraView(glm::vec2 pt) const
		{
			bool bX = pt.x >= position.x && pt.x <= position.x + viewport.GetWidthZoomed();
			bool bY = pt.y >= position.y && pt.y <= position.y + viewport.GetHeightZoomed();
			return bX && bY;
		}
		float camera::GetViewportXCoord(float xMap)
		{
			return viewport.GetWidth() / viewport.GetWidthZoomed() * (xMap - position.x);
		}
		float camera::GetViewportYCoord(float yMap)
		{
			return viewport.GetHeight() / viewport.GetHeightZoomed() * (yMap - position.y);
		}
		glm::mat4 camera::GetProjectionMatrix(void) const
		{
			return this->cameraProjectionMatrix;
		}
		glm::mat4 camera::GetViewMatrix(void) const
		{
			return this->cameraViewMatrix;
		}
		float camera::GetCurrentZoom(void)
		{
			return currentZoom;
		}
		void camera::SetCurrentZoom(const float z)
		{
			currentZoom = z;
		}
		float camera::GetZoomFactor(void)
		{
			return zoomCameraFactor;
		}
		void camera::SetZoomFactor(const float f)
		{
			zoomCameraFactor = f;
		}
		float camera::GetXPosition(void)
		{
			return position.x;
		}
		float camera::GetYPosition(void)
		{
			return position.y;
		}
		glm::vec2 camera::GetZoomedCoords(const float xCoord, const float yCoord)
		{
			float x = xCoord * viewport.GetWidthZoomed() / viewport.GetWidth() + camera::GetXPosition();
			float y = yCoord * viewport.GetHeightZoomed() / viewport.GetHeight() + camera::GetYPosition();
			return glm::vec2(x, y);
		}		
		void camera::mouseControl(void)
		{
			mouseMovement = false;

			// pipeline
			this->zoom.Run(this->currentFrame);
			ManageMouseMovement();
		}
		void camera::keyboardControl(void)
		{
			// Conditions for NOT moving
			if (mouseMovement) return;

			// pipeline
			ManageKeyboardMovement();
		}
		void camera::GoToPoint(const GLfloat x, const GLfloat y)
		{
			position.x = x;
			position.y = y;
			this->FixCameraPosition();
		}
		glm::mat4 camera::CalculateViewMatrix(void)
		{
			return glm::lookAt(position, position + front, up);
		}
		void camera::SetMaxZoom(float z)
		{
			this->MaxZoom = z;
		}
		void camera::SetMovementSpeed(float s)
		{
			this->MovementSpeed = s;
		}
		float camera::GetMaxZoom(void)
		{
			return this->MaxZoom;
		}
		float camera::GetMovementSpeed(void)
		{
			return this->MovementSpeed;
		}
		float camera::GetYMinimapCoordinate(const float y, const unsigned int bottomBarHeight, const unsigned int topBarHeight)
		{
			return viewport.GetHeight() * (y - bottomBarHeight) / (viewport.GetHeight() - bottomBarHeight - topBarHeight);
		}

		camera::ZoomInstance::ZoomInstance() :
			window(rattlesmake::peripherals::viewport::get_instance()),
			camera_ref(rattlesmake::peripherals::camera::get_instance()),
			mouse(rattlesmake::peripherals::mouse::get_instance()),
			keyboard(rattlesmake::peripherals::keyboard::get_instance())
		{
		}
		bool camera::ZoomInstance::IsRunning(void)
		{
			return this->isRunning;
		}
		void camera::ZoomInstance::Start(int value, int currentFrame)
		{
			this->sign = value / std::abs(value);
			this->frameBegin = currentFrame;
			this->isRunning = true;
		}
		void camera::ZoomInstance::Stop(void)
		{
			this->isRunning = false;
		}
		void camera::ZoomInstance::Run(int currentFrame)
		{
			if ((keyboard.IsKeyNotReleased(GLFW_KEY_LEFT_CONTROL) || keyboard.IsKeyNotReleased(GLFW_KEY_RIGHT_CONTROL)) && mouse.ScrollBool)
			{
				if ((mouse.ScrollValue > 0 && camera_ref.currentZoom > 1) || (mouse.ScrollValue < 0 && camera_ref.currentZoom < camera_ref.MaxZoom))
				{
					this->Start((int)mouse.ScrollValue, camera_ref.currentFrame);
				}
				mouse.ScrollBool = false;
			}
			if (this->isRunning == true)
			{
				float delta = float(currentFrame - frameBegin);
				if (delta <= duration)
				{
					float deltaNorm = 2.f * (1.f - (duration - delta) / duration) - 1.f; // to be in [-1, 1]
					float alpha = TriweightKernel(deltaNorm) * 1.5f;
					float value = alpha;

					if ((sign > 0 && camera_ref.currentZoom <= 1.f) || (sign < 0 && camera_ref.currentZoom >= camera_ref.MaxZoom)) 
					{
						this->Stop();
						return;
					}
					else if (camera_ref.currentZoom - sign * alpha < 1.f)
					{
						value = 1.f - (camera_ref.currentZoom - sign * alpha);
					}
					else if (camera_ref.currentZoom - sign * alpha > camera_ref.MaxZoom)
					{
						value = (camera_ref.currentZoom - sign * alpha) - camera_ref.MaxZoom;
					}
					camera_ref.currentZoom -= sign * value;

					glm::vec2 mousePos{ mouse.GetXMapCoordinate(), mouse.GetYMapCoordinate() };

					// update window zoomed
					window.UpdateZoomedValues();

					camera_ref.FixCameraPosition(&mousePos);
				}
				else
				{
					this->Stop();
				}
			}
		}
		float camera::ZoomInstance::TriweightKernel(float f)
		{
			return 35.f / 32.f * (float)std::pow((1.f - f * f), 3);
		}
	};
};
