#include "terrainchunk_shader.h"

#include <fileservice.h>

#include <array>
#include <gtc/type_ptr.hpp>

terrainchunk_shader terrainchunk_shader::instance;

terrainchunk_shader::terrainchunk_shader(void)
{
}
terrainchunk_shader::~terrainchunk_shader(void)
{
}
terrainchunk_shader& terrainchunk_shader::get_instance(void)
{
	return terrainchunk_shader::instance;
}
void terrainchunk_shader::initialize(void)
{
	// compile shaders
	this->compile_shaders(this->get_vertex_shader().c_str(), this->get_fragment_shader().c_str());

	// generate buffers
	this->generate_buffers();

	// initialize matrices
	this->apply_matrices(glm::mat4(1.f), glm::mat4(1.f));
}
std::string terrainchunk_shader::get_vertex_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/terrainchunk_vertex_shader.glsl";
	return file_s.read_file(path.c_str());
}

std::string terrainchunk_shader::get_fragment_shader(void)
{
	auto& file_s = rattlesmake::services::file_service::get_instance();
	auto path = file_s.get_main_root() + "shaders/terrainchunk_fragment_shader.glsl";
	return file_s.read_file(path.c_str());
}
void terrainchunk_shader::generate_buffers(void)
{
	glGenVertexArrays(1, &this->glData.VAO);
	glBindVertexArray(this->glData.VAO);

	// indices
	unsigned int indices[726] = { 0, 1, 2, 3, 4, 5, 6, 7, 4, 8, 9, 7, 10, 11, 9, 12, 13, 11, 14, 15, 13, 16, 17, 15, 18, 19, 17, 20, 21, 19, 22, 2, 21, 23, 24, 25, 26, 27, 23, 28, 29, 26,
		30, 31, 28, 32, 33, 30, 34, 35, 32, 36, 37, 34, 38, 39, 36, 40, 41, 38, 21, 42, 40, 43, 25, 44, 45, 23, 43, 46, 26, 45, 47, 28, 46, 48, 30, 47, 49, 32, 48, 50, 34, 49, 51, 36, 50,
		52, 38, 51, 19, 40, 52, 53, 44, 54, 55, 43, 53, 56, 45, 55, 57, 46, 56, 58, 47, 57, 59, 48, 58, 60, 49, 59, 61, 50, 60, 62, 51, 61, 17, 52, 62, 63, 54, 64, 65, 53, 63, 66, 55, 65,
		67, 56, 66, 68, 57, 67, 69, 58, 68, 70, 59, 69, 71, 60, 70, 72, 61, 71, 15, 62, 72, 73, 64, 74, 75, 63, 73, 76, 65, 75, 77, 66, 76, 78, 67, 77, 79, 68, 78, 80, 69, 79, 81, 70, 80,
		82, 71, 81, 13, 72, 82, 83, 74, 84, 85, 73, 83, 86, 75, 85, 87, 76, 86, 88, 77, 87, 89, 78, 88, 90, 79, 89, 91, 80, 90, 92, 81, 91, 11, 82, 92, 93, 84, 94, 95, 83, 93, 96, 85, 95,
		97, 86, 96, 98, 87, 97, 99, 88, 98, 100, 89, 99, 101, 90, 100, 102, 91, 101, 9, 92, 102, 103, 94, 104, 105, 93, 103, 106, 95, 105, 107, 96, 106, 108, 97, 107, 109, 98, 108, 110,
		99, 109, 111, 100, 110, 112, 101, 111, 7, 102, 112, 113, 104, 114, 115, 103, 113, 116, 105, 115, 117, 106, 116, 118, 107, 117, 119, 108, 118, 120, 109, 119, 121, 110, 120, 122,
		111, 121, 4, 112, 122, 123, 114, 124, 125, 113, 123, 126, 115, 125, 127, 116, 126, 128, 117, 127, 129, 118, 128, 130, 119, 129, 131, 120, 130, 132, 121, 131, 5, 122, 132, 27, 133,
		24, 29, 134, 27, 31, 135, 29, 33, 136, 31, 35, 137, 33, 37, 138, 35, 39, 139, 37, 41, 140, 39, 42, 141, 41, 2, 142, 42, 0, 143, 1, 3, 6, 4, 6, 8, 7, 8, 10, 9, 10, 12, 11, 12, 14,
		13, 14, 16, 15, 16, 18, 17, 18, 20, 19, 20, 22, 21, 22, 0, 2, 23, 27, 24, 26, 29, 27, 28, 31, 29, 30, 33, 31, 32, 35, 33, 34, 37, 35, 36, 39, 37, 38, 41, 39, 40, 42, 41, 21, 2, 42,
		43, 23, 25, 45, 26, 23, 46, 28, 26, 47, 30, 28, 48, 32, 30, 49, 34, 32, 50, 36, 34, 51, 38, 36, 52, 40, 38, 19, 21, 40, 53, 43, 44, 55, 45, 43, 56, 46, 45, 57, 47, 46, 58, 48, 47,
		59, 49, 48, 60, 50, 49, 61, 51, 50, 62, 52, 51, 17, 19, 52, 63, 53, 54, 65, 55, 53, 66, 56, 55, 67, 57, 56, 68, 58, 57, 69, 59, 58, 70, 60, 59, 71, 61, 60, 72, 62, 61, 15, 17, 62,
		73, 63, 64, 75, 65, 63, 76, 66, 65, 77, 67, 66, 78, 68, 67, 79, 69, 68, 80, 70, 69, 81, 71, 70, 82, 72, 71, 13, 15, 72, 83, 73, 74, 85, 75, 73, 86, 76, 75, 87, 77, 76, 88, 78, 77,
		89, 79, 78, 90, 80, 79, 91, 81, 80, 92, 82, 81, 11, 13, 82, 93, 83, 84, 95, 85, 83, 96, 86, 85, 97, 87, 86, 98, 88, 87, 99, 89, 88, 100, 90, 89, 101, 91, 90, 102, 92, 91, 9, 11,
		92, 103, 93, 94, 105, 95, 93, 106, 96, 95, 107, 97, 96, 108, 98, 97, 109, 99, 98, 110, 100, 99, 111, 101, 100, 112, 102, 101, 7, 9, 102, 113, 103, 104, 115, 105, 103, 116, 106,
		105, 117, 107, 106, 118, 108, 107, 119, 109, 108, 120, 110, 109, 121, 111, 110, 122, 112, 111, 4, 7, 112, 123, 113, 114, 125, 115, 113, 126, 116, 115, 127, 117, 116, 128, 118, 117,
		129, 119, 118, 130, 120, 119, 131, 121, 120, 132, 122, 121, 5, 4, 122, 27, 134, 133, 29, 135, 134, 31, 136, 135, 33, 137, 136, 35, 138, 137, 37, 139, 138, 39, 140, 139, 41, 141,
		140, 42, 142, 141, 2, 1, 142 };

	glGenBuffers(1, &this->glData.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glData.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertices
	GLfloat vertices[576] = { 10, 9, 1, 0.1, 9, 10, 0.9, 0, 9, 9, 0.9, 0.1, 10, -1, 1, 1.1, 9, 0, 0.9, 1, 9, -1, 0.9, 1.1, 10, 0, 1, 1, 9, 1, 0.9, 0.9, 10, 1, 1, 0.9, 9, 2, 0.9, 0.8, 10,
		2, 1, 0.8, 9, 3, 0.9, 0.7, 10, 3, 1, 0.7, 9, 4, 0.9, 0.6, 10, 4, 1, 0.6, 9, 5, 0.9, 0.5, 10, 5, 1, 0.5, 9, 6, 0.9, 0.4, 10, 6, 1, 0.4, 9, 7, 0.9, 0.3, 10, 7, 1, 0.3, 9, 8, 0.9, 0.2,
		10, 8, 1, 0.2, 0, 8, -0, 0.2, -1, 9, -0.1, 0.1, -1, 8, -0.1, 0.2, 1, 8, 0.1, 0.2, 0, 9, -0, 0.1, 2, 8, 0.2, 0.2, 1, 9, 0.1, 0.1, 3, 8, 0.3, 0.2, 2, 9, 0.2, 0.1, 4, 8, 0.4, 0.2, 3,
		9, 0.3, 0.1, 5, 8, 0.5, 0.2, 4, 9, 0.4, 0.1, 6, 8, 0.6, 0.2, 5, 9, 0.5, 0.1, 7, 8, 0.7, 0.2, 6, 9, 0.6, 0.1, 8, 8, 0.8, 0.2, 7, 9, 0.7, 0.1, 8, 9, 0.8, 0.1, 0, 7, -0, 0.3, -1, 7,
		-0.1, 0.3, 1, 7, 0.1, 0.3, 2, 7, 0.2, 0.3, 3, 7, 0.3, 0.3, 4, 7, 0.4, 0.3, 5, 7, 0.5, 0.3, 6, 7, 0.6, 0.3, 7, 7, 0.7, 0.3, 8, 7, 0.8, 0.3, 0, 6, -0, 0.4, -1, 6, -0.1, 0.4, 1, 6,
		0.1, 0.4, 2, 6, 0.2, 0.4, 3, 6, 0.3, 0.4, 4, 6, 0.4, 0.4, 5, 6, 0.5, 0.4, 6, 6, 0.6, 0.4, 7, 6, 0.7, 0.4, 8, 6, 0.8, 0.4, 0, 5, -0, 0.5, -1, 5, -0.1, 0.5, 1, 5, 0.1, 0.5, 2, 5,
		0.2, 0.5, 3, 5, 0.3, 0.5, 4, 5, 0.4, 0.5, 5, 5, 0.5, 0.5, 6, 5, 0.6, 0.5, 7, 5, 0.7, 0.5, 8, 5, 0.8, 0.5, 0, 4, -0, 0.6, -1, 4, -0.1, 0.6, 1, 4, 0.1, 0.6, 2, 4, 0.2, 0.6, 3, 4,
		0.3, 0.6, 4, 4, 0.4, 0.6, 5, 4, 0.5, 0.6, 6, 4, 0.6, 0.6, 7, 4, 0.7, 0.6, 8, 4, 0.8, 0.6, 0, 3, -0, 0.7, -1, 3, -0.1, 0.7, 1, 3, 0.1, 0.7, 2, 3, 0.2, 0.7, 3, 3, 0.3, 0.7, 4, 3,
		0.4, 0.7, 5, 3, 0.5, 0.7, 6, 3, 0.6, 0.7, 7, 3, 0.7, 0.7, 8, 3, 0.8, 0.7, 0, 2, -0, 0.8, -1, 2, -0.1, 0.8, 1, 2, 0.1, 0.8, 2, 2, 0.2, 0.8, 3, 2, 0.3, 0.8, 4, 2, 0.4, 0.8, 5, 2,
		0.5, 0.8, 6, 2, 0.6, 0.8, 7, 2, 0.7, 0.8, 8, 2, 0.8, 0.8, 0, 1, -0, 0.9, -1, 1, -0.1, 0.9, 1, 1, 0.1, 0.9, 2, 1, 0.2, 0.9, 3, 1, 0.3, 0.9, 4, 1, 0.4, 0.9, 5, 1, 0.5, 0.9, 6, 1,
		0.6, 0.9, 7, 1, 0.7, 0.9, 8, 1, 0.8, 0.9, 0, 0, -0, 1, -1, 0, -0.1, 1, 1, 0, 0.1, 1, 2, 0, 0.2, 1, 3, 0, 0.3, 1, 4, 0, 0.4, 1, 5, 0, 0.5, 1, 6, 0, 0.6, 1, 7, 0, 0.7, 1, 8, 0,
		0.8, 1, 0, -1, -0, 1.1, -1, -1, -0.1, 1.1, 1, -1, 0.1, 1.1, 2, -1, 0.2, 1.1, 3, -1, 0.3, 1.1, 4, -1, 0.4, 1.1, 5, -1, 0.5, 1.1, 6, -1, 0.6, 1.1, 7, -1, 0.7, 1.1, 8, -1, 0.8,
		1.1, -1, 10, -0.1, 0, 0, 10, -0, 0, 1, 10, 0.1, 0, 2, 10, 0.2, 0, 3, 10, 0.3, 0, 4, 10, 0.4, 0, 5, 10, 0.5, 0, 6, 10, 0.6, 0, 7, 10, 0.7, 0, 8, 10, 0.8, 0, 10, 10, 1, 0 };

	glGenBuffers(1, &this->VERT_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VERT_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // x, y
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // texX, texY
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// heights_textures
	std::array<float, 144> heights_textures_array{};
	glGenBuffers(1, &this->HEIGHTS_TEX_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->HEIGHTS_TEX_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(heights_textures_array[0]) * heights_textures_array.size(), &heights_textures_array[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	// normal
	std::array<float, 144 * 3> normal_array{};
	glGenBuffers(1, &this->NORM_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->NORM_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal_array[0]) * normal_array.size(), &normal_array[0], GL_STATIC_DRAW);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);

	// x_pos & y_pos
	const int n_data = 2;
	GLfloat entity_data[n_data] = { 0.f };
	glGenBuffers(1, &terrain_data_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_data_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(entity_data), entity_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, n_data * sizeof(float), (void*)0);
	glVertexAttribDivisor(4, 1);

	// end
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// uniform buffer object
	this->terrain_data_ubo_index = glGetUniformBlockIndex(this->glData.shaderId, "terrain_ubo_data");
	auto buffer_size = sizeof(glm::mat4) * 2 + sizeof(glm::vec2) * 2 + sizeof(glm::ivec3) + sizeof(glm::vec3);
	glGenBuffers(1, &this->terrain_data_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->terrain_data_ubo);
	glBufferData(GL_UNIFORM_BUFFER, buffer_size, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, this->terrain_data_ubo_index, this->terrain_data_ubo, 0, buffer_size);
}
void terrainchunk_shader::update_buffer(float* bufferData, size_t bufferSize, GLuint _VBO)
{
	glBindVertexArray(this->glData.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferSize, bufferData, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void terrainchunk_shader::begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix)
{
	auto shdID = this->glData.shaderId;

	// set uniforms
	glUseProgram(shdID);

	glBindBuffer(GL_UNIFORM_BUFFER, this->terrain_data_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_matrix)); // uProjection
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view_matrix)); // uView
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUseProgram(0);
}
void terrainchunk_shader::pre_draw(const uint32_t textureArrayId, const uint32_t numberOfTextures, const glm::vec2& size, const bool wireframe, float terrainChunkSize, bool tracing, float minZ, float maxZ, float passableSeaHeight, float notPassableSeaHeight, float minGrassHeight)
{
	auto shdID = this->glData.shaderId;
	
	wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glUseProgram(shdID);

	glBindBuffer(GL_UNIFORM_BUFFER, this->terrain_data_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::vec2), glm::value_ptr(glm::vec2(minZ, maxZ))); // uHeightMinMax
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(glm::vec2), sizeof(glm::vec2), glm::value_ptr(size)); // uSize
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(glm::vec2) * 2, sizeof(glm::ivec3), glm::value_ptr(glm::ivec3((int)tracing, (int)wireframe, (int)numberOfTextures))); // uTracingWireframeTerrainsnumber
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(glm::vec2) * 2 + sizeof(glm::ivec3), sizeof(glm::vec3), glm::value_ptr(glm::vec3(minGrassHeight, passableSeaHeight, notPassableSeaHeight))); // uHeights
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	// bind texture
	glUniform1i(glGetUniformLocation(shdID, "uSampleTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
	glUseProgram(0);
}
void terrainchunk_shader::draw(const int xpos, const int ypos, float* textureHeight_bufferData, size_t textureHeight_bufferSize, float* normals_bufferData, size_t normals_bufferSize)
{
	auto shdID = this->glData.shaderId;
	float terrain_data_buf[2] = { (GLfloat)xpos, (GLfloat)ypos };
	
	glUseProgram(shdID);

	this->update_buffer(textureHeight_bufferData, textureHeight_bufferSize, this->HEIGHTS_TEX_VBO);
	this->update_buffer(normals_bufferData, normals_bufferSize, this->NORM_VBO);
	this->update_buffer(terrain_data_buf, 2, this->terrain_data_vbo);

	glBindVertexArray(this->glData.VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glData.IBO);
	glDrawElements(GL_TRIANGLES, 726, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}
