/*
* ---------------------------
* CENTURION IMAGE LIBRARY
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <queue>

#include <new_shader.h> // needed

// origin
// 0 = bottom-left, 1 = top-left, 2 = center, 3 = top-right, 4 = bottom-right

namespace rattlesmake
{
	namespace geometry
	{
		/*
			this class has the task to draw rectangles.
			now it is used ONLY to draw in game (map) coordinates.
		*/
		class rectangle_shader : public rattlesmake::utils::opengl_shader
		{
		public:

			// singleton
			static rectangle_shader& get_instance(void);

			/*
				method to call just once, in your engine init function
			*/
			void initialize(void);

			/*
				method to call at the beginning of a new game frame
			*/
			void begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix);

			// 0 = bottom-left, 1 = top-left, 2 = center, 3 = top-right, 4 = bottom-right
			void draw(int32_t x, int32_t y, uint32_t width, uint32_t height, glm::vec4 borderColor, glm::vec4 backgroundColor, uint8_t origin);

			void add_to_queue(int32_t x, int32_t y, uint32_t width, uint32_t height, glm::vec4 borderColor, glm::vec4 backgroundColor, uint8_t origin);
			void draw_queue(void);

			~rectangle_shader(void);
		protected:
			explicit rectangle_shader(void);

			std::string get_vertex_shader(void) override;
			std::string get_fragment_shader(void) override;
			void generate_buffers(void) override;

			struct rectangle_shader_draw_data 
			{
				glm::vec4 borderColor, backgroundColor;
				int32_t x, y, width, height;
				uint8_t origin;
			};
			std::queue<rectangle_shader_draw_data> queue_to_draw;

			static rectangle_shader instance;
		};
	};
};
