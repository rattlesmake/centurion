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

#include <new_shader.h> // needed

namespace rattlesmake
{
	namespace geometry
	{
		/*
			this class has the task to draw circles.
			now it is used ONLY to draw in game (map) coordinates.
		*/

		class circle_shader : public rattlesmake::utils::opengl_shader
		{
		public:
			static circle_shader& get_instance(void);

			/*
				method to call just once, in your engine init function
			*/
			void initialize(void);

			/*
				method to call at the beginning of a new game frame
			*/
			void begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix);

			// 0 = bottom-left, 1 = top-left, 2 = center, 3 = top-right, 4 = bottom-right
			void draw(const glm::vec4& borderColor, const uint32_t x_pos, const  uint32_t y_pos, const uint32_t radius, const uint8_t origin) const;

			~circle_shader(void);
		protected:
			explicit circle_shader(void);

			std::string get_vertex_shader(void) override;
			std::string get_fragment_shader(void) override;
			void generate_buffers(void) override;

			static circle_shader instance;
		};
	};
};
