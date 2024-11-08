#include "terrain.h"
#include <fileservice.h>
#include <gtc/type_ptr.hpp>

#include <stb_wrap.h>

#include <GL/glew.h>

Terrain::Terrain() 
{
	this->compile_shaders(
		this->get_vertex_shader().c_str(),
		this->get_fragment_shader().c_str(),
		this->get_tessellation_control_shader().c_str(),
		this->get_tessellation_evaluation_shader().c_str(),
		this->get_geometry_shader().c_str()
	);

	this->generate_buffers();
	
	this->apply_matrices(glm::mat4(1.f), glm::mat4(1.f));

	this->read_texture("GRASS_1.png", &this->textureID1);
	this->read_texture("ROAD_1.png", &this->textureID2);
}

Terrain::~Terrain() {
	glDeleteBuffers(1, &this->glData.VBO);
	glDeleteVertexArrays(1, &this->glData.VAO);
}
void Terrain::read_texture(const char* path, unsigned int* textureID)
{
	int width, height, nrChannels;
	unsigned char* data = rattlesmake::image::stb::load_from_file(path, &width, &height, &nrChannels, 0);
	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	rattlesmake::image::stb::free(data);
}

void Terrain::Draw(const glm::mat4& projection, const glm::mat4& view)
{
	this->apply_matrices(projection, view);

	glClear(GL_COLOR_BUFFER_BIT);
	
	wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(this->glData.shaderId);

	glUniform1f(glGetUniformLocation(this->glData.shaderId, "vertices_gap_px"), vertices_gap_px);
	glUniform1f(glGetUniformLocation(this->glData.shaderId, "outerSegs"), tessellation_value_outer);
	glUniform1f(glGetUniformLocation(this->glData.shaderId, "innerSegs"), tessellation_value_inner);
	glUniform1i(glGetUniformLocation(this->glData.shaderId, "wireframe"), int(wireframe));

	// bind textures
	glUniform1i(glGetUniformLocation(this->glData.shaderId, "grass"), 0);
	glUniform1i(glGetUniformLocation(this->glData.shaderId, "road"), 1);
	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, textureID1);
	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, textureID2);

	glBindVertexArray(this->glData.VAO);
	glDrawArrays(GL_PATCHES, 0, 4 * n_vertices_x * n_vertices_y);
}

std::string Terrain::get_vertex_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/terrain_vertex_shader.glsl";
	return file_s.read_file(path.c_str());
}


std::string Terrain::get_fragment_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/terrain_fragment_shader.glsl";
	return file_s.read_file(path.c_str());
}

std::string Terrain::get_tessellation_control_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/terrain_tess_control_shader.glsl";
	return file_s.read_file(path.c_str());
}

std::string Terrain::get_tessellation_evaluation_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/terrain_tess_eval_shader.glsl";
	return file_s.read_file(path.c_str());
}

std::string Terrain::get_geometry_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/terrain_geometry_shader.glsl";
	return file_s.read_file(path.c_str());
}

void Terrain::generate_buffers(void)
{
	std::vector<GLfloat> all_vertices;
	GLfloat size = (float)chunk_size;

	for (size_t ix = 0; ix < n_vertices_x - 1; ix++)
	{
		for (size_t iy = 0; iy < n_vertices_y - 1; iy++)
		{
			// bottom left
			all_vertices.push_back((float)ix - 1.f); // x
			all_vertices.push_back((float)iy - 1.f); // y 
			all_vertices.push_back((float)(ix - 1.f) / size); // ux
			all_vertices.push_back((float)(iy - 1.f) / size); // uy

			// bottom right
			all_vertices.push_back((float)(ix + 1) - 1.f); // x
			all_vertices.push_back((float)iy - 1.f); // y 
			all_vertices.push_back((float)((ix + 1) - 1.f) / size); // ux
			all_vertices.push_back((float)(iy - 1.f) / size); // uy

			// top right
			all_vertices.push_back((float)(ix + 1) - 1.f); // x
			all_vertices.push_back((float)(iy + 1) - 1.f); // y 
			all_vertices.push_back((float)((ix + 1) - 1.f) / size); // ux
			all_vertices.push_back((float)((iy + 1) - 1.f) / size); // uy

			// top left
			all_vertices.push_back((float)ix - 1.f); // x
			all_vertices.push_back((float)(iy + 1) - 1.f); // y 
			all_vertices.push_back((float)(ix - 1.f) / size); // ux
			all_vertices.push_back((float)((iy + 1) - 1.f) / size); // uy
		}
	}

	glCreateBuffers(1, &this->glData.VBO);

	glCreateVertexArrays(1, &this->glData.VAO);

	//(VAO, binding index, VBO, offset, stride)
	glVertexArrayVertexBuffer(this->glData.VAO, 0, this->glData.VBO, 0, 4 * sizeof(float));

	//(VBO, size in bytes, pointer to data start, mode)
	glNamedBufferStorage(this->glData.VBO, all_vertices.size() * sizeof(float), all_vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	glEnableVertexArrayAttrib(this->glData.VAO, 0);
	glEnableVertexArrayAttrib(this->glData.VAO, 1);

	//(VAO, location, size, type, should be normalized, (void*)offset)
	glVertexArrayAttribFormat(this->glData.VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(this->glData.VAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));

	//(VAO, location, binding)
	glVertexArrayAttribBinding(this->glData.VAO, 0, 0);
	glVertexArrayAttribBinding(this->glData.VAO, 1, 0);
}


