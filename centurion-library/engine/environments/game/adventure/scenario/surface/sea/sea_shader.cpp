#include "sea_shader.h"
#include <gtc/type_ptr.hpp>

#include <fileservice.h>

sea_shader sea_shader::instance;

sea_shader& sea_shader::get_instance(void)
{
	return sea_shader::instance;
}

void sea_shader::initialize(void)
{
	// compile shaders
	this->compile_shaders(this->get_vertex_shader().c_str(), this->get_fragment_shader().c_str());

	// generate buffers
	this->generate_buffers();

	// initialize matrices
	this->apply_matrices(glm::mat4(1.f), glm::mat4(1.f));
}

void sea_shader::set_surface_constants(const uint32_t mapWidth, const uint32_t mapHeight, const uint32_t seaHeight)
{
	this->sea_data_1.x = mapWidth;
	this->sea_data_1.y = mapHeight;
	this->sea_data_1.z = seaHeight;
}

void sea_shader::begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const float gameTime)
{
	this->sea_data_1.w = gameTime;
	auto shdID = this->glData.shaderId;

	// set uniforms
	glUseProgram(shdID);

	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_matrix)); // uProjection
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view_matrix)); // uView
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUniform4f(this->sea_data_1_index, this->sea_data_1.x, this->sea_data_1.y, this->sea_data_1.z, this->sea_data_1.z);

	glUseProgram(0);
}

void sea_shader::draw(void) const
{
	auto shdID = this->glData.shaderId;
	auto VAO = this->glData.VAO;
	auto VBO = this->glData.VBO;
	auto IBO = this->glData.IBO;

	glUseProgram(shdID);

	// set uniforms
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

sea_shader::~sea_shader(void)
{
}

sea_shader::sea_shader(void)
{
}

std::string sea_shader::get_vertex_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/sea_vertex_shader.glsl";
	return file_s.read_file(path.c_str());
}

std::string sea_shader::get_fragment_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/sea_fragment_shader.glsl";
	return file_s.read_file(path.c_str());
}

void sea_shader::generate_buffers(void)
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

	// uniform buffer object
	this->ubo_index = glGetUniformBlockIndex(this->glData.shaderId, "sea_ubo_data");
	auto buffer_size = sizeof(glm::mat4) * 2;
	glGenBuffers(1, &this->ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferData(GL_UNIFORM_BUFFER, buffer_size, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, this->ubo_index, this->ubo, 0, buffer_size);

	// save uniform indices
	this->sea_data_1_index = glGetUniformLocation(this->glData.shaderId, "sea_data_1");
}
