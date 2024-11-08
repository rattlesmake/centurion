/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <new_shader.h>

class terrainchunk_shader : public rattlesmake::utils::opengl_shader
{
public:
	static terrainchunk_shader& get_instance(void);

	void initialize(void);


	/*
		method to call BEFORE drawing all chunks
	*/
	void pre_draw(const uint32_t textureArrayId, const uint32_t numberOfTextures, const glm::vec2& size, const bool wireframe, float terrainChunkSize, bool tracing, float minZ, float maxZ, float passableSeaHeight, float notPassableSeaHeight, float minGrassHeight);
	
	/*
		methods to call to draw ONE chunk
	*/
	void draw(const int xpos, const int ypos, float* textureHeight_bufferData, size_t textureHeight_bufferSize, float* normals_bufferData, size_t normals_bufferSize);

	/*
		method to call at the beginning of a new game frame
	*/
	void begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix);

	~terrainchunk_shader(void);
protected:
	explicit terrainchunk_shader(void);

	std::string get_vertex_shader(void) override;
	std::string get_fragment_shader(void) override;
	void generate_buffers(void) override;

	void update_buffer(float* bufferData, size_t bufferSize, GLuint _VBO);

	GLuint terrain_data_vbo;
	GLuint terrain_data_ubo; // uniform buffer object
	GLuint terrain_data_ubo_index;

	GLuint VERT_VBO = 0, HEIGHTS_TEX_VBO = 0, NORM_VBO = 0; // for terrain chunk
	static terrainchunk_shader instance;
};
