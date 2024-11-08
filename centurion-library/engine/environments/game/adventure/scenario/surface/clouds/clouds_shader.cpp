#include "clouds_shader.h"
#include <gtc/type_ptr.hpp>

#include <fileservice.h>

clouds_shader clouds_shader::instance;

clouds_shader& clouds_shader::get_instance(void)
{
	return clouds_shader::instance;
}

void clouds_shader::initialize(void)
{
	// compile shaders
	this->compile_shaders(this->get_vertex_shader().c_str(), this->get_fragment_shader().c_str());

	// generate buffers
	this->generate_buffers();

	// initialize matrices
	this->apply_matrices(glm::mat4(1.f), glm::mat4(1.f));
}

void clouds_shader::set_surface_constants(const uint32_t mapWidth, const uint32_t mapHeight, const uint32_t cloudsHeight)
{
	this->map_width = mapWidth;
	this->map_height = mapHeight;
	this->clouds_height = cloudsHeight;
}

void clouds_shader::begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const float gameTime)
{
	auto shdID = this->glData.shaderId;

	// set uniforms
	glUseProgram(shdID);
	glUniformMatrix4fv(glGetUniformLocation(shdID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(glGetUniformLocation(shdID, "uView"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniform1i(glGetUniformLocation(shdID, "uMapWidth"), this->map_width);
	glUniform1i(glGetUniformLocation(shdID, "uMapHeight"), this->map_height);
	glUniform1i(glGetUniformLocation(shdID, "uCloudsHeight"), this->clouds_height);
	glUniform1f(glGetUniformLocation(shdID, "uTime"), gameTime);

	glUseProgram(0);
}

void clouds_shader::draw(void) const
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

clouds_shader::~clouds_shader(void)
{
}

clouds_shader::clouds_shader(void)
{
}

std::string clouds_shader::get_vertex_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/clouds_vertex_shader.glsl";
	return file_s.read_file(path.c_str());
}

std::string clouds_shader::get_fragment_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/clouds_fragment_shader.glsl";
	return file_s.read_file(path.c_str());
}

void clouds_shader::generate_buffers(void)
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
