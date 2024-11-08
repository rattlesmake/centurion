#include "mouse.h"
#include "viewport.h"
#include "keyboard.h"
#include "camera.h"
#include "user_input.h"

#include <math_utils.h>

#include <GLFW/glfw3.h>

namespace rattlesmake
{
	namespace peripherals
	{
		mouse mouse::instance;
		mouse& mouse::get_instance(void)
		{
			return instance;
		}
		mouse::mouse(void) : viewport(rattlesmake::peripherals::viewport::get_instance()), camera(rattlesmake::peripherals::camera::get_instance()) { }
		mouse::~mouse(void)
		{
		}
		void mouse::begin_engine_frame(const double time_total_seconds)
		{
			this->elapsedSeconds = time_total_seconds;
			this->Control();
			this->IsHolding();
		}
		void mouse::end_engine_frame(void)
		{
			this->ResetAllButtons();
		}
		double mouse::GetXMapCoordinate(void) const
		{
			return round(this->position.x * viewport.GetWidthZoomed() / viewport.GetWidth() + camera.GetXPosition());
		}

		double mouse::GetYMapCoordinate(void) const
		{
			return round(this->position.y * viewport.GetHeightZoomed() / viewport.GetHeight() + camera.GetYPosition());
		}

		std::pair<double, double> mouse::GetMapCoordinates(void) const
		{
			const double x = round(this->position.x * viewport.GetWidthZoomed() / viewport.GetWidth() + camera.GetXPosition());
			const double y = round(this->position.y * viewport.GetHeightZoomed() / viewport.GetHeight() + camera.GetYPosition());
			return std::pair<double, double>(x, y);
		}

		double mouse::GetY2DMapCoordinate(void) const
		{
			return this->GetYMapCoordinate() - this->mouseZNoise;
		}

		bool mouse::IsCursorInGameScreen(void) const
		{
			return (this->GetYLeftClick() > 0) && (this->GetYLeftClick() < (viewport.GetHeight()));
		}

		void mouse::SetMouseZNoise(const float z)
		{
			this->mouseZNoise = z;
		}

		void mouse::SetMouseInViewport(const float x, const float y)
		{
			this->position.x = x;
			this->position.y = y;
		}

		std::optional<glm::vec3> mouse::GetColorOfClickedPixel(void) const
		{
			bool bClick = false;
			GLint xClick = 0;
			GLint yClick = 0;
			if (this->LeftClick)
			{
				bClick = true;
				xClick = (GLint)this->GetXLeftClick();
				yClick = (GLint)this->GetYLeftClick();
			}
			else if (this->RightClick)
			{
				bClick = true;
				xClick = (GLint)this->GetXRightClick();
				yClick = (GLint)this->GetYRightClick();
			}

			if (bClick == true)
			{
				unsigned char data[4] = { 0 };
				glReadPixels(xClick, yClick, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
				glm::vec3 color{};
				color.r = data[0];
				color.g = data[1];
				color.b = data[2];
				return color;
			}
			return std::nullopt;
		}

		std::optional<glm::vec3> mouse::GetColorOfLastMousePositionPixel(void) const
		{
			unsigned char data[4] = { 0 };
			glReadPixels((GLint)this->GetXPosition(), (GLint)this->GetYPosition(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
			glm::vec3 color{};
			color.r = data[0];
			color.g = data[1];
			color.b = data[2];
			return color;
		}

		double mouse::GetXPosition(void) const
		{
			return this->position.x;
		}

		double mouse::GetYPosition(void) const
		{
			return this->position.y;
		}

		double mouse::GetY2DPosition(void) const
		{
			return this->y2DPosition;
		}

		double mouse::GetXLeftClick(void) const
		{
			return this->xLeftClick;
		}

		double mouse::GetYLeftClick(void) const
		{
			return this->yLeftClick;
		}

		double mouse::GetXRightClick(void) const
		{
			return this->xRightClick;
		}

		double mouse::GetYRightClick(void) const
		{
			return this->yRightClick;
		}

		double mouse::GetY2DRightClick(void) const
		{
			return this->y2DRightClick;
		}

		void mouse::Control()
		{
			float yzoomed = camera.GetZoomedCoords(this->position.x, this->position.y).y;

			this->y2DPosition = this->GetYPosition() - this->mouseZNoise;

			if (!LeftClick)
			{
				this->xLeftClick = this->position.x;
				this->yLeftClick = this->position.y;
			}
			#if CENTURION_DEBUG_MODE
			else
			{
				std::stringstream ss;
				ss << "You have left-clicked on (X=" << (int)this->xLeftClick << ", Y=" << (int)this->yLeftClick << ")";
				//Logger::Info(ss.str());
			}
			#endif

			if (!RightClick)
			{
				this->xRightClick = this->position.x;
				this->yRightClick = this->position.y;
				this->y2DRightClick = this->y2DPosition;
			}
			#if CENTURION_DEBUG_MODE
			else
			{
				std::stringstream ss;
				ss << "You have right-clicked on (X=" << (int)this->xRightClick << ", Y=" << (int)this->yRightClick << ")";
				//Logger::Info(ss.str());
			}
			#endif
			if (this->Release)
			{
				//currentState = CURSOR_DEFAULT;
			}
		}

		void mouse::IsHolding()
		{
			if (elapsedSeconds - this->leftHoldClickData.lastTime > TIME_LEFT_HOLD && this->leftHoldClickData.bIsTimeSaved)
				this->LeftHold = true;
			if (elapsedSeconds - this->rightHoldClickData.lastTime > TIME_LEFT_HOLD && this->rightHoldClickData.bIsTimeSaved)
				this->RightHold = true;
			if (elapsedSeconds - this->middleHoldClickData.lastTime > TIME_LEFT_HOLD && this->middleHoldClickData.bIsTimeSaved)
				this->MiddleHold = true;
		}

		void mouse::Input(const int button, const int action)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				if (action == GLFW_PRESS)
				{
					this->LeftClick = true;
					if (this->leftHoldClickData.bIsTimeSaved == false)
					{
						this->leftHoldClickData.lastTime = this->elapsedSeconds;
						this->leftHoldClickData.bIsTimeSaved = true;
					}
				}
				else if (action == GLFW_RELEASE)
				{
					mouse::Release = true;
					if (this->leftHoldClickData.bIsTimeSaved)
					{
						this->leftHoldClickData.bIsTimeSaved = false;
						this->LeftHold = false;
					}
				}
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				if (action == GLFW_PRESS)
				{
					this->RightClick = true;
					if (this->rightHoldClickData.bIsTimeSaved == false)
					{
						this->rightHoldClickData.lastTime = this->elapsedSeconds;
						this->rightHoldClickData.bIsTimeSaved = true;
					}
				}
				else if (action == GLFW_RELEASE)
				{
					this->Release = true;
					if (this->rightHoldClickData.bIsTimeSaved)
					{
						this->rightHoldClickData.bIsTimeSaved = false;
						this->RightHold = false;
					}
				}
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				if (action == GLFW_PRESS)
				{
					this->MiddleClick = true;
					if (this->middleHoldClickData.bIsTimeSaved == false)
					{
						this->middleHoldClickData.lastTime = this->elapsedSeconds;
						this->middleHoldClickData.bIsTimeSaved = true;
					}
				}
				else if (action == GLFW_RELEASE)
				{
					this->MiddleClick = false;
					if (this->middleHoldClickData.bIsTimeSaved)
					{
						this->middleHoldClickData.bIsTimeSaved = false;
						this->MiddleHold = false;
					}
				}
			}
		}

		void mouse::ChangeCursorType(const int type)
		{
			if (type == CURSOR_TYPE_DEFAULT || type == CURSOR_TYPE_CIRCLE)
				this->CURSOR_TYPE = type;
		}

		int mouse::GetCursorType(void) const
		{
			return this->CURSOR_TYPE;
		}

		void mouse::ResetAllButtons(void)
		{
			this->LeftClick = false;
			this->RightClick = false;
			this->MiddleClick = false;
			this->Release = false;
			this->ScrollBool = false;

			this->LeftHold = false;
			this->RightHold = false;
			this->MiddleHold = false;
		}
	};
};
