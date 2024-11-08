#pragma once
#include <vector>
#include <string>
#include <new_shader.h>
#include <png.h>

class Terrain : rattlesmake::utils::opengl_shader
{
public:
	Terrain();
	~Terrain();

	void Draw(const glm::mat4& projection, const glm::mat4& view);

	int tessellation_value_outer = 1;
	int tessellation_value_inner = 1;
	int vertices_gap_px = 64;
	bool wireframe = false;

protected:
	std::string get_vertex_shader(void) override;
	std::string get_fragment_shader(void) override;
	std::string get_tessellation_control_shader(void);
	std::string get_tessellation_evaluation_shader(void);
	std::string get_geometry_shader(void);
	void generate_buffers(void) override;
	int n_vertices_x = 12;
	int n_vertices_y = 12;
	int chunk_size = 10;

	void read_texture(const char* path, unsigned int* textureID);

	unsigned int textureID1 = 0;
	unsigned int textureID2 = 0;
};
