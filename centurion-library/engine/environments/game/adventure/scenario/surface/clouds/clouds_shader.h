/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <new_shader.h>

class clouds_shader : public rattlesmake::utils::opengl_shader
{
public:
	static clouds_shader& get_instance(void);

	/*
		method to call just once, in your engine init function
	*/
	void initialize(void);

	/*
		method to call in surface initialize function
	*/
	void set_surface_constants(const uint32_t mapWidth, const uint32_t mapHeight, const uint32_t cloudsHeight);

	/*
		method to call at the beginning of a new game frame
	*/
	void begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const float gameTime);


	void draw(void) const;

	~clouds_shader(void);
protected:
	explicit clouds_shader(void);

	std::string get_vertex_shader(void) override;
	std::string get_fragment_shader(void) override;
	void generate_buffers(void) override;

	uint32_t map_width, map_height, clouds_height;
	static clouds_shader instance;
};
