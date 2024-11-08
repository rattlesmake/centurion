#include "shadows_fb.h"
#include <fileservice.h>
#include <gtc/type_ptr.hpp>

namespace centurion
{
	namespace fb
	{
		shadows_fb shadows_fb::instance;

		shadows_fb& shadows_fb::get_instance(void)
		{
			return instance;
		}
		void shadows_fb::initialize_shader(const glm::mat4& projMatrixInit, const glm::mat4& viewMatrixInit)
		{
			// compile shaders
			this->compile_shaders(this->get_vertex_shader().c_str(), this->get_fragment_shader().c_str());

			// generate buffers
			this->generate_buffers();

			// initialize uniforms
			glUseProgram(this->glData.shaderId);
			glUniformMatrix4fv(glGetUniformLocation(this->glData.shaderId, "uProjection"), 1, GL_FALSE, glm::value_ptr(projMatrixInit));
			glUniformMatrix4fv(glGetUniformLocation(this->glData.shaderId, "uView"), 1, GL_FALSE, glm::value_ptr(viewMatrixInit));
			glUniform2i(glGetUniformLocation(this->glData.shaderId, "uSize"), this->texture_width, this->texture_height);
			glUseProgram(0);
		}
		void shadows_fb::draw_texture(void) const
		{
			auto shdID = this->glData.shaderId;
			auto VAO = this->glData.VAO;
			auto IBO = this->glData.IBO;

			glUseProgram(shdID);

			glBindVertexArray(VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->get_texture_id());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glUseProgram(0);
		}
		std::string shadows_fb::get_vertex_shader(void)
		{
			auto& file_s = rattlesmake::services::file_service::get_instance();
			auto path = file_s.get_main_root() + "shaders/shadow_fb_vertex_shader.glsl";
			return file_s.read_file(path.c_str());
		}
		std::string shadows_fb::get_fragment_shader(void)
		{
			auto& file_s = rattlesmake::services::file_service::get_instance();
			auto path = file_s.get_main_root() + "shaders/shadow_fb_fragment_shader.glsl";
			return file_s.read_file(path.c_str());
		}
		void shadows_fb::generate_buffers(void)
		{
			unsigned int indices[6] = {
				0, 1, 3,   // first triangle
				1, 2, 3    // second triangle
			};
			GLfloat vertices[16] = {
				// positions	// uv coords		
				0.f,	0.f,	0.f, 1.f,	// in basso a sx
				1.f,	0.f,	1.f, 1.f,	// in basso a dx
				1.f,	1.f,	1.f, 0.f,	// in alto a dx
				0.f,	1.f,	0.f, 0.f	// in alto a sx
			};

			glGenVertexArrays(1, &this->glData.VAO);
			glBindVertexArray(this->glData.VAO);

			glGenBuffers(1, &this->glData.IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glData.IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glGenBuffers(1, &this->glData.VBO);
			glBindBuffer(GL_ARRAY_BUFFER, this->glData.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		void shadows_fb::clear_buffer(void) const
		{
			glClearColor(1.f, 1.f, 1.f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		shadows_fb::shadows_fb(void)
		{
		}
		shadows_fb::~shadows_fb(void)
		{
		}
	};
};

