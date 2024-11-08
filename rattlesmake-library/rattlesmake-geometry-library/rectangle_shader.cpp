#include "rectangle_shader.h"

#include <fileservice.h>
#include <gtc/type_ptr.hpp>

namespace rattlesmake
{
	namespace geometry
	{
		rectangle_shader rectangle_shader::instance;

		rectangle_shader& rectangle_shader::get_instance(void)
		{
			return rectangle_shader::instance;
		}
		rectangle_shader::rectangle_shader(void)
		{

		}
		std::string rectangle_shader::get_vertex_shader(void)
		{
			auto& file_s = rattlesmake::services::file_service::get_instance();
			auto path = file_s.get_main_root() + "shaders/rectangle_vertex_shader.glsl";
			return file_s.read_file(path.c_str());
		}
		std::string rectangle_shader::get_fragment_shader(void)
		{
			auto& file_s = rattlesmake::services::file_service::get_instance();
			auto path = file_s.get_main_root() + "shaders/rectangle_fragment_shader.glsl";
			return file_s.read_file(path.c_str());
		}
		void rectangle_shader::generate_buffers(void)
		{
			/*
			x0,y0------x3,y3
			|          |
			|          |
			|          |
			x1,y1------x2,y2
			*/

			GLfloat vertices[16] = { 0.f };

			vertices[0] = (0.f); vertices[1] = (1.f);  // x0, y0
			vertices[2] = (0.f); vertices[3] = (0.f);  // x1, y1

			vertices[4] = (0.f); vertices[5] = (0.f);  // x1, y1
			vertices[6] = (1.f); vertices[7] = (0.f);  // x2, y2

			vertices[8] = (1.f); vertices[9] = (0.f);  // x2, y2
			vertices[10] = (1.f); vertices[11] = (1.f); // x3, y3

			vertices[12] = (1.f); vertices[13] = (1.f); // x3, y3
			vertices[14] = (0.f); vertices[15] = (1.f); // x0, y0

			glGenVertexArrays(1, &this->glData.VAO);
			glBindVertexArray(this->glData.VAO);
			glGenBuffers(1, &this->glData.VBO);
			glBindBuffer(GL_ARRAY_BUFFER, this->glData.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		void rectangle_shader::initialize(void)
		{
			// compile shaders
			this->compile_shaders(this->get_vertex_shader().c_str(), this->get_fragment_shader().c_str());

			// generate buffers
			this->generate_buffers();

			// initialize matrices
			this->apply_matrices(glm::mat4(1.f), glm::mat4(1.f));
		}
		void rectangle_shader::begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix)
		{
			auto shdID = this->glData.shaderId;
			glUseProgram(shdID);
			glUniformMatrix4fv(glGetUniformLocation(shdID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
			glUniformMatrix4fv(glGetUniformLocation(shdID, "uView"), 1, GL_FALSE, glm::value_ptr(view_matrix));
			glUseProgram(0);
		}
		void rectangle_shader::draw(int32_t x, int32_t y, uint32_t width, uint32_t height, glm::vec4 borderColor, glm::vec4 backgroundColor, uint8_t origin)
		{
			auto shdID = this->glData.shaderId;
			auto VAO = this->glData.VAO;
			auto IBO = this->glData.IBO;

			glUseProgram(shdID);

			glUniform4i(glGetUniformLocation(shdID, "uData"), x, y, width, height);
			glUniform1i(glGetUniformLocation(shdID, "uOrigin"), origin);
			glUniform4f(glGetUniformLocation(shdID, "uBorderColor"), borderColor.r / 255.f, borderColor.g / 255.f, borderColor.b / 255.f, borderColor.a / 255.f);
			glUniform4f(glGetUniformLocation(shdID, "uBackgroundColor"), backgroundColor.r / 255.f, backgroundColor.g / 255.f, backgroundColor.b / 255.f, backgroundColor.a / 255.f);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, 8);
			glBindVertexArray(0);

			glUseProgram(0);
		}
		void rectangle_shader::add_to_queue(int32_t x, int32_t y, uint32_t width, uint32_t height, glm::vec4 borderColor, glm::vec4 backgroundColor, uint8_t origin)
		{
			rectangle_shader_draw_data data;
			data.x = x;
			data.y = y;
			data.width = width;
			data.height = height;
			data.borderColor = borderColor;
			data.backgroundColor = backgroundColor;
			data.origin = origin;
			this->queue_to_draw.push(data);
		}
		void rectangle_shader::draw_queue(void)
		{
			while (!this->queue_to_draw.empty()) 
			{
				{
					auto const& data = this->queue_to_draw.front();
					this->draw(data.x, data.y, data.width, data.height, data.borderColor, data.backgroundColor, data.origin);
				}
				this->queue_to_draw.pop();
			}
		}
		rectangle_shader::~rectangle_shader(void)
		{
		}
	};
};
