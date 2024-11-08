/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#ifndef CAMERA_THRESHOLD
#define CAMERA_THRESHOLD 20
#endif

#include <cstdint>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm.hpp>

namespace rattlesmake
{
	namespace peripherals
	{
		class viewport;
		class mouse;
		class keyboard;

		class camera
		{
		public:

			/*
				This function has to be called in Init function of 
				your engine instance (before while loop)
			*/
			void initialize(const float maxZoom = 20.f, const float movementSpeed = 10.f, const float leftMargin = 0.f, const float bottomMargin = 0.f);

			/*
				This function has to be called during game instancing
			*/
			void new_game(const glm::vec2& visible_map_size, const float topbar_height, const float bottombar_height);

			/*
				This function has to be called at the beginning of
				while loop
			*/
			void begin_engine_frame(const uint32_t current_frame);

			/*
				This function has to be called at the beginning of
				run of the game environment
			*/
			void begin_game_frame(const bool minimap_is_active, const bool any_iframe_opened, const bool perform_mouse_control);

			/*
				this function draws a rectangle that fits the camera perimeter
				(for minimap rectangle purpose)
			*/
			void draw_camera_rectangle(void) const;

			void set_position(glm::vec2 pt);


			glm::mat4 CalculateViewMatrix(void);

			[[nodiscard]] bool IsPointInCameraView(glm::vec2 pt) const;

			float GetViewportXCoord(float xMap);
			float GetViewportYCoord(float yMap);

			glm::mat4 GetProjectionMatrix(void) const;
			glm::mat4 GetViewMatrix(void) const;

			/// <summary>
			/// This integer function gets the current zoom value (from 1 to 8).
			/// </summary>
			/// <returns>Current zoom value to return, expressed in float</returns>
			float GetCurrentZoom(void);

			/// <summary>
			/// This function sets the current zoom value (from 1 to 8).
			/// </summary>
			/// <param name="z">Current zoom value. It supports only const int values</param>
			void SetCurrentZoom(const float z);

			/// <summary>
			/// This floating function gets the current zoom factor, which determines the percentage of enlargement or reduction
			/// for each change of the zoom value.
			/// </summary>
			/// <returns>Current zoom factor percentage to return, expressed in float</returns>
			float GetZoomFactor(void);

			/// <summary>
			/// This function sets the current zoom factor percentage.
			/// </summary>
			/// <param name="f">Current zoom factor percentage. It supports only float values</param>
			void SetZoomFactor(const float f);

			/// <summary>
			/// This floating function returns the current x position of the camera.
			/// </summary>
			/// <returns>Current x position to return, expressd in float</returns>
			float GetXPosition();

			/// <summary>
			/// This floating function gets the current y position of the camera.
			/// </summary>
			/// <returns>Current y position to return, expressd in float</returns>>
			float GetYPosition();

			/// <summary>
			/// This glm::vec2 function calculates the zoomed coordinates of a x,y point.
			/// </summary>
			/// <param name="xCoord">X coordinate. It supports only float values</param>
			/// <param name="yCoord">Y coordinate. It supports only float values</param>
			/// <returns>Current x and y zoomed values to return, expressed in glm::vec2</returns>
			glm::vec2 GetZoomedCoords(const float xCoord, const float yCoord);

			/// <summary>
			/// This floating function gets the y coordinate on minimap of a y point.
			/// </summary>
			/// <param name="y">Y coordinate. It supports only const float values.</param>
			/// <returns>Current y minimap position to return, expressed in float</returns>
			float GetYMinimapCoordinate(const float y, const unsigned int bottomBarHeight = 0, const unsigned int topBarHeight = 0);
			
			/// <summary>
			/// This function move the camera to a specified point instantly.
			/// </summary>
			/// <param name="x">X coordinate to move to. It supports only const GLfloat</param>
			/// <param name="y">Y coordinate to move to. It supports only const GLfloat</param>
			void GoToPoint(const GLfloat x, const  GLfloat y);

			void SetMaxZoom(float z);
			void SetMovementSpeed(float s);
			float GetMaxZoom(void);
			float GetMovementSpeed(void);

#pragma region Singleton
			///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
			camera(const camera& other) = delete;
			camera& operator=(camera const& other) = delete;
			[[nodiscard]] static camera& get_instance(void);
			~camera();
#pragma endregion

		private:

			camera();
			/// <summary>
			/// This function manages camera behavior resulting from mouse interactions.
			/// </summary>
			void mouseControl(void);

			/// <summary>
			/// This function manages camera behavior resulting from keyboard interactions.
			/// </summary>
			void keyboardControl(void);

			

			rattlesmake::peripherals::viewport& viewport;
			rattlesmake::peripherals::mouse& mouse;
			rattlesmake::peripherals::keyboard& keyboard;

			class ZoomInstance
			{
			public:
				ZoomInstance();
				void Start(int value, int currentFrame);
				void Stop(void);
				void Run(int currentFrame);
				bool IsRunning(void);
			private:
				float TriweightKernel(float f);
				bool isRunning = false;
				int frameBegin = 0;
				int duration = 6; // frames
				int sign = 0;
				camera& camera_ref;
				rattlesmake::peripherals::viewport& window;
				rattlesmake::peripherals::mouse& mouse;
				rattlesmake::peripherals::keyboard& keyboard;
			};

			void FixCameraDuringZoom(glm::vec2* mousePos);
			void FixCameraPosition(glm::vec2* mousePos = nullptr);
			void CameraMovement(unsigned int DIRECTION, bool _mouseMovement);
			void ManageMouseMovement();
			void ManageKeyboardMovement();

			glm::mat4 cameraProjectionMatrix{ 1.f };
			glm::mat4 cameraMinimapProjectionMatrix{ 1.f };
			glm::mat4 cameraViewMatrix{ 1.f };
			glm::vec2 visibleMapSize{ 0 };
			float topBarHeight = 0.f;
			float bottomBarHeight = 0.f;
			float leftMargin = 0.f;
			float bottomMargin = 0.f;

			float MaxZoom = 20.f;
			float MovementSpeed = 10.f;
			glm::vec3 position{ 0,0,0 };
			glm::vec3 front{ 0,0,0 };
			glm::vec3 up{ 0,0,0 };
			glm::vec3 right{ 0,0,0 };
			glm::vec3 worldUp{ 0,0,0 };
			GLfloat yaw = 0.0f, pitch = 0.0f;
			float zoomCameraFactor = 100.f;
			float currentZoom = 8.f;
			bool mouseMovement = true;
			ZoomInstance zoom;
			uint32_t currentFrame = 0;

			static camera instance;
		};
	};
};
