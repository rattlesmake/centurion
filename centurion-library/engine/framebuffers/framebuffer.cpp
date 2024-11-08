#include "framebuffer.h"
#include <GL/glew.h>
#include <iostream>

namespace centurion
{
	namespace fb
	{
		void framebuffer::initialize_buffer(const float width, const float height)
		{
			this->texture_width = (uint32_t)width;
			this->texture_height = (uint32_t)height;

			glGenFramebuffers(1, &this->buffer_id);
			glBindFramebuffer(GL_FRAMEBUFFER, this->buffer_id);
			
			// create a color attachment texture
			glGenTextures(1, &this->texture_id);
			glBindTexture(GL_TEXTURE_2D, this->texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->texture_width, this->texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture_id, 0);
			
			// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
			glGenRenderbuffers(1, &this->rbo_id);
			glBindRenderbuffer(GL_RENDERBUFFER, this->rbo_id);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->texture_width, this->texture_height); // use a single renderbuffer object for both a depth AND stencil buffer.
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo_id); // now actually attach it
			
			// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw std::exception("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void framebuffer::activate(void) const
		{
			glBindFramebuffer(GL_FRAMEBUFFER, this->buffer_id);
		}
		void framebuffer::back_to_default_fb(void) const
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		
		const uint32_t framebuffer::get_texture_id(void) const
		{
			return this->texture_id;
		}
		const uint32_t framebuffer::get_buffer_id(void) const
		{
			return this->buffer_id;
		}
		framebuffer::~framebuffer(void)
		{
		}
		framebuffer::framebuffer(void)
		{
		}
	};
};
