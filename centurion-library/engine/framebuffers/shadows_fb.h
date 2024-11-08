/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include "framebuffer.h"

#include <new_shader.h> // needed

namespace centurion
{
	namespace fb
	{
		class shadows_fb : public framebuffer, public rattlesmake::utils::opengl_shader
		{
		public:
			// singleton
			static shadows_fb& get_instance(void);

			void initialize_shader(const glm::mat4& projMatrixInit, const glm::mat4& viewMatrixInit);
			void draw_texture(void) const; 
			void clear_buffer(void) const;
			~shadows_fb(void);

		protected:
			explicit shadows_fb(void);

			std::string get_vertex_shader(void) override;
			std::string get_fragment_shader(void) override;
			void generate_buffers(void) override;

			static shadows_fb instance;
		};
	};
};
