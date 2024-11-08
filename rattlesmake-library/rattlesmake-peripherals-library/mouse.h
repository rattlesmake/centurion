/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#ifndef TIME_LEFT_HOLD 
#define TIME_LEFT_HOLD 0.0f
#endif

#ifndef CURSOR_TYPE_DEFAULT
#define CURSOR_TYPE_DEFAULT 0
#endif

#ifndef CURSOR_TYPE_CIRCLE
#define CURSOR_TYPE_CIRCLE 1
#endif

#include <optional>
#include <glm.hpp>

namespace rattlesmake
{
	namespace peripherals
	{
		class viewport;
		class camera;

		class mouse
		{
		public:

			/*
				This function has to be called at the beginning of
				while loop
			*/
			void begin_engine_frame(const double time_total_seconds);

			/*
				This function has to be called at the end of
				while loop
			*/
			void end_engine_frame(void);



			/// <summary>
			/// This function performs the read pixel on the clicked point and returns the corresponding RGB color
			/// </summary>
			[[nodiscard]] std::optional<glm::vec3> GetColorOfClickedPixel(void) const;
			[[nodiscard]] std::optional<glm::vec3> GetColorOfLastMousePositionPixel(void) const;

			/// <summary>
			/// This function gets the x coordinate of the mouse.
			/// </summary>
			/// <returns>This value it's true if the specified button is begin released; it's false otherwise</returns>
			[[nodiscard]] double GetXPosition(void) const;

			/// <summary>
			/// This function gets the y coordinate of the mouse.
			/// </summary>
			[[nodiscard]] double GetYPosition(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetY2DPosition(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetXLeftClick(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetYLeftClick(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetXRightClick(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetYRightClick(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetY2DRightClick(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetXMapCoordinate(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetYMapCoordinate(void) const;

			[[nodiscard]] std::pair<double, double> GetMapCoordinates(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] double GetY2DMapCoordinate(void) const;

			/// <summary>
			/// 
			/// </summary>
			[[nodiscard]] bool IsCursorInGameScreen(void) const;

			/// <summary>
			/// This function sets the mouse Z noise, useful for mouse-Y-2D-position calculation
			/// </summary>
			void SetMouseZNoise(const float z);

			void SetMouseInViewport(const float x, const float y);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="lastX"></param>
			/// <param name="lastY"></param>
			void Control();

			/// <summary>
			/// 
			/// </summary>
			void IsHolding();

			/// <summary>
			/// 
			/// </summary>
			/// <param name="button"></param>
			/// <param name="action"></param>
			void Input(const int button, const int action);

			/// <summary>
			///  This method changes the cursor type
			/// </summary>
			/// <param name="type">This value indicates the type. Accepted values are CURSOR_TYPE_DEFAULT or CURSOR_TYPE_CIRCLE</param>
			void ChangeCursorType(const int type = CURSOR_TYPE_DEFAULT);

			/// <summary>
			///  This method returns the cursor type
			/// </summary>
			[[nodiscard]] int GetCursorType(void) const;

			void ResetAllButtons(void);

			// Properties
			float ScrollValue = 0.f;
			bool LeftClick = false;
			bool RightClick = false;
			bool MiddleClick = false;
			bool LeftHold = false;
			bool RightHold = false;
			bool MiddleHold = false;
			bool Release = false;
			bool ScrollBool = false;

			#pragma region Singleton
			///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
			mouse(const mouse& other) = delete;
			mouse& operator=(const mouse& other) = delete;
			[[nodiscard]] static mouse& get_instance(void);
			~mouse(void);
			#pragma endregion

		private:
			mouse(void);

			rattlesmake::peripherals::viewport& viewport;
			rattlesmake::peripherals::camera& camera;

			struct HoldClickData
			{
				double lastTime = 0;
				bool bIsTimeSaved = false;
			} leftHoldClickData, rightHoldClickData, middleHoldClickData;

			glm::vec3 position{ 0,0,0 };
			double mouseZNoise = 0, xPosGrid = 0, yPosGrid = 0, xLeftClick = 0, yLeftClick = 0, xRightClick = 0, yRightClick = 0, y2DPosition = 0, y2DRightClick = 0;
			int CURSOR_TYPE = CURSOR_TYPE_DEFAULT;
			double elapsedSeconds = 0.0;

			static mouse instance;
		};
	};
};
