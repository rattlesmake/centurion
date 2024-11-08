#include "circle_shader.h"

#include <fileservice.h>
#include <gtc/type_ptr.hpp>

namespace rattlesmake
{
	namespace geometry
	{
		circle_shader circle_shader::instance;

		circle_shader& circle_shader::get_instance(void)
		{
			return circle_shader::instance;
		}
		circle_shader::circle_shader(void)
		{

		}
		std::string circle_shader::get_vertex_shader(void)
		{
			auto& file_s = rattlesmake::services::file_service::get_instance();
			auto path = file_s.get_main_root() + "shaders/circle_vertex_shader.glsl";
			return file_s.read_file(path.c_str());
		}
		std::string circle_shader::get_fragment_shader(void)
		{
			auto& file_s = rattlesmake::services::file_service::get_instance();
			auto path = file_s.get_main_root() + "shaders/circle_fragment_shader.glsl";
			return file_s.read_file(path.c_str());
		}
		void circle_shader::generate_buffers(void)
		{
			glGenVertexArrays(1, &this->glData.VAO);
			glBindVertexArray(this->glData.VAO);
			glGenBuffers(1, &this->glData.VBO);
			glBindBuffer(GL_ARRAY_BUFFER, this->glData.VBO);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		void circle_shader::initialize(void)
		{
			// compile shaders
			this->compile_shaders(this->get_vertex_shader().c_str(), this->get_fragment_shader().c_str());

			// generate buffers
			this->generate_buffers();

			// initialize matrices
			this->apply_matrices(glm::mat4(1.f), glm::mat4(1.f));
		}
		void circle_shader::begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix)
		{
			auto shdID = this->glData.shaderId;
			glUseProgram(shdID);
			glUniformMatrix4fv(glGetUniformLocation(shdID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
			glUniformMatrix4fv(glGetUniformLocation(shdID, "uView"), 1, GL_FALSE, glm::value_ptr(view_matrix));
			glUseProgram(0);
		}
		void circle_shader::draw(const glm::vec4& borderColor, const uint32_t x_pos, const uint32_t y_pos, const uint32_t radius, const uint8_t origin) const
		{
			glm::vec2 center = glm::vec2(0.f);

			switch (origin)
			{
			case 0: // bottom-left
				center = glm::vec2(x_pos + radius, y_pos + radius);
				break;
			case 1: // top left
				center = glm::vec2(x_pos + radius, y_pos - radius);
				break;
			case 2: // center
				center = glm::vec2(x_pos, y_pos);
				break;
			case 3: // top right
				center = glm::vec2(x_pos - radius, y_pos - radius);
				break;
			case 4: // bottom right
				center = glm::vec2(x_pos - radius, y_pos + radius);
				break;
			}

			// calculate segments
			uint16_t num_segments = uint16_t(4 + 8 * log((float)radius));

			// method 1 ---
			float theta = float(2 * 3.1415926 / float(num_segments));
			float tangetial_factor = tanf(theta);//calculate the tangential factor 
			float radial_factor = cosf(theta);//calculate the radial factor 
			float x = (float)radius; //we start at angle = 0 
			float y = 0;

			std::vector<float> _vertices;

			for (int ii = 0; ii < num_segments; ii++)
			{
				_vertices.push_back(x + center.x);
				_vertices.push_back(y + center.y);
				float tx = -y;
				float ty = x;
				x += tx * tangetial_factor; //add the tangential vector 
				y += ty * tangetial_factor;
				x *= radial_factor; //correct using the radial factor 
				y *= radial_factor;
			}

			auto shdID = this->glData.shaderId;
			auto VAO = this->glData.VAO;
			auto VBO = this->glData.VBO;
			auto IBO = this->glData.IBO;

			glUseProgram(shdID);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glUniform4f(glGetUniformLocation(shdID, "uBorderColor"), borderColor.r / 255.f, borderColor.g / 255.f, borderColor.b / 255.f, borderColor.a / 255.f);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_LOOP, 0, num_segments);
			glBindVertexArray(0);
			
			glUseProgram(0);
		}
		circle_shader::~circle_shader(void)
		{

		}
	};
};
