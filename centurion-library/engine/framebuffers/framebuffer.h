/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <stdint.h>
#include <glm.hpp>

namespace centurion
{
	namespace fb
	{
		// interface
		class framebuffer
		{
		public:
			void initialize_buffer(const float width, const float height);
			void activate(void) const;
			void back_to_default_fb(void) const;
			const uint32_t get_texture_id(void) const;
			const uint32_t get_buffer_id(void) const;

			~framebuffer(void);

		protected:
			explicit framebuffer(void);
			
			uint32_t buffer_id = 0;
			uint32_t rbo_id = 0;
			uint32_t texture_id = 0;
			uint32_t texture_width = 0;
			uint32_t texture_height = 0;
		};
	};
};
