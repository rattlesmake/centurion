#include "xml_entity_shader.h"
#include "xml_entity_layer.h"
#include "xml_entity.h"
#include <iostream>
#include <gtc/type_ptr.hpp>

#include <fileservice.h>
#include <framebuffers/shadows_fb.h>

namespace centurion
{
	namespace assets
	{
		xml_entity_shader xml_entity_shader::instance;

		xml_entity_shader& xml_entity_shader::get_instance(void)
		{
			return instance;
		}

		void xml_entity_shader::initialize(void)
		{
			// compile shaders
			this->compile_shaders(this->get_vertex_shader().c_str(), this->get_fragment_shader().c_str());

			// generate buffers
			this->generate_buffers();

			// initialize matrices
			this->apply_matrices(glm::mat4(1.f), glm::mat4(1.f));
		}
		void xml_entity_shader::clear_cache(void)
		{
			entities_data.clear();
		}
		void xml_entity_shader::begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const bool minimap_is_active, const bool is_editor, const float game_time)
		{
			glUseProgram(this->glData.shaderId);

			glBindBuffer(GL_UNIFORM_BUFFER, this->entity_ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_matrix)); // uProjection
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view_matrix)); // uView
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glUniform1i(this->u_minimap_index, (int)minimap_is_active);
			glUniform1i(this->u_editor_index, (int)is_editor);
			glUseProgram(0);
		}
		void xml_entity_shader::activate_picking(void)
		{
			this->picking = true;
			glUseProgram(this->glData.shaderId);
			glUniform1i(this->u_picking_index, 1);
			glUseProgram(0);
		}
		void xml_entity_shader::deactivate_picking(void)
		{
			this->picking = false;
			glUseProgram(this->glData.shaderId);
			glUniform1i(this->u_picking_index, 0);
			glUseProgram(0);
		}
		void xml_entity_shader::activate_alpha(void)
		{
			glUseProgram(this->glData.shaderId);
			glUniform1i(this->u_alpha_index, 1);
			glUseProgram(0);
		}
		void xml_entity_shader::deactivate_alpha(void)
		{
			glUseProgram(this->glData.shaderId);
			glUniform1i(this->u_alpha_index, 0);
			glUseProgram(0);
		}

		void xml_entity_shader::initialize_draw_data(void)
		{
			this->ordered_layers_data.clear();
			this->ordered_alpha_layers_data.clear();
			this->unordered_layers_data.clear();
		}
		void xml_entity_shader::new_object_draw_data(const uint32_t current_frame, const uint8_t current_direction, const glm::vec3& player_color, const uint32_t level, bool is_selected, bool is_placeable, bool is_unit)
		{
			this->current_entity_data = xml_entity_shader_data(current_frame, current_direction, player_color, level, is_selected, is_placeable, is_unit);
		}
		void xml_entity_shader::new_object_picking_draw_data(const uint32_t current_frame, const uint8_t current_direction, const uint32_t picking_id)
		{
			this->current_entity_data = xml_entity_shader_data(current_frame, current_direction, glm::vec3(), 0, false, false, false);
			this->current_entity_data.set_picking_id(picking_id);
		}
		void xml_entity_shader::new_entity_draw_data(const uint32_t x_pos, const uint32_t y_pos, const uint32_t total_frames, const uint32_t total_directions)
		{
			this->current_entity_data.set_position(x_pos, y_pos);
			this->current_entity_data.set_frames_and_directions(total_frames, total_directions);
		}

		void xml_entity_shader::new_layer_draw_data(const bool has_mask, const glm::uvec2& base_image_size, uint32_t base_image_opengl_id, int32_t mask_image_opengl_id)
		{
			this->current_entity_data.set_flags(false, has_mask);
			this->current_entity_data.set_image_size(base_image_size.x, base_image_size.y);
			this->current_entity_data.opengl_texture_id_normal = base_image_opengl_id;
			this->current_entity_data.opengl_texture_id_mask = mask_image_opengl_id;
			this->current_entity_data.opengl_texture_id_shadow = -1;
		}

		void xml_entity_shader::new_shadow_draw_data(const glm::uvec2& image_size, const int32_t opengl_texture_id)
		{
			this->current_entity_data.set_flags(true, false);
			this->current_entity_data.set_image_size(image_size.x, image_size.y);
			this->current_entity_data.opengl_texture_id_normal = -1;
			this->current_entity_data.opengl_texture_id_mask = -1;
			this->current_entity_data.opengl_texture_id_shadow = opengl_texture_id;
		}
		void xml_entity_shader::render_current_draw_data(void)
		{
			this->draw(this->current_entity_data);
		}
		void xml_entity_shader::push_current_draw_data(void)
		{
			if (this->current_entity_data.get_shadow_flag() == true)
			{
				this->unordered_layers_data.push_back(this->current_entity_data);
			}
			else
			{
				this->ordered_layers_data[this->current_entity_data.sorting_key].push_back(this->current_entity_data);

				// add also to alpha map
				if (this->current_entity_data.shadow_flag == false && this->current_entity_data.is_unit == true)
				{
					this->ordered_alpha_layers_data[this->current_entity_data.sorting_key].push_back(this->current_entity_data);
				}
			}
		}
		void xml_entity_shader::render_all_draw_data(void)
		{
			auto& shadow_fb = centurion::fb::shadows_fb::get_instance();
			shadow_fb.activate();
			shadow_fb.clear_buffer();

			// shadows
			for (size_t i = 0; i < this->unordered_layers_data.size(); i++)
			{
				this->draw(this->unordered_layers_data[i]);
			}

			shadow_fb.back_to_default_fb();
			shadow_fb.draw_texture();

			// layers
			if (this->ordered_layers_data.empty() == false)
			{
				std::map<int32_t, std::vector<xml_entity_shader_data>>::iterator it;
				for (it = this->ordered_layers_data.begin(); it != this->ordered_layers_data.end(); it++)
				{
					auto& vec = it->second;
					size_t length = vec.size();
					for (size_t i = 0; i < length; i++)
					{
						this->draw(vec[i]);
					}
				}
			}

			// alpha layers
			if (this->ordered_alpha_layers_data.empty() == false)
			{
				this->activate_alpha();
				std::map<int32_t, std::vector<xml_entity_shader_data>>::iterator it;
				for (it = this->ordered_alpha_layers_data.begin(); it != this->ordered_alpha_layers_data.end(); it++)
				{
					auto& vec = it->second;
					size_t length = vec.size();
					for (size_t i = 0; i < length; i++)
					{
						this->draw(vec[i]);
					}
				}
				this->deactivate_alpha();
			}
		}



		std::string xml_entity_shader::get_vertex_shader(void)
		{
			auto& file_s = rattlesmake::services::file_service::get_instance();
			auto path = file_s.get_main_root() + "shaders/entity_vertex_shader.glsl";
			return file_s.read_file(path.c_str());
		}
		std::string xml_entity_shader::get_fragment_shader(void)
		{
			auto& file_s = rattlesmake::services::file_service::get_instance();
			auto path = file_s.get_main_root() + "shaders/entity_fragment_shader.glsl";
			return file_s.read_file(path.c_str());
		}
		void xml_entity_shader::generate_buffers(void)
		{
			unsigned int indices[6] = {
				0, 1, 3,   // first triangle
				1, 2, 3    // second triangle
			};
			GLfloat vertices[16] = {
				// positions	// uv coords		
				-0.5f,	-0.5f,	0.f, 1.f,	// in basso a sx
				0.5f,	-0.5f,	1.f, 1.f,	// in basso a dx
				0.5f,	0.5f,	1.f, 0.f,	// in alto a dx
				-0.5f,	0.5f,	0.f, 0.f	// in alto a sx
			};

			glGenVertexArrays(1, &this->glData.VAO);
			glBindVertexArray(this->glData.VAO);

			glGenBuffers(1, &this->glData.IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glData.IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glGenBuffers(1, &this->glData.VBO);
			glBindBuffer(GL_ARRAY_BUFFER, this->glData.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


			const int n_data = 16;
			GLfloat entity_data[n_data] = { 0.f };
			glGenBuffers(1, &entity_data_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, entity_data_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(entity_data), entity_data, GL_STATIC_DRAW);

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, n_data * sizeof(float), (void*)0);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, n_data * sizeof(float), (void*)(4 * sizeof(float)));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, n_data * sizeof(float), (void*)(8 * sizeof(float)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, n_data * sizeof(float), (void*)(12 * sizeof(float)));

			glVertexAttribDivisor(2, 1);
			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			// uniform buffer object
			this->entity_ubo_data_index = glGetUniformBlockIndex(this->glData.shaderId, "entity_ubo_data");
			auto buffer_size = sizeof(glm::mat4) * 2;
			glGenBuffers(1, &this->entity_ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, this->entity_ubo);
			glBufferData(GL_UNIFORM_BUFFER, buffer_size, NULL, GL_STREAM_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindBufferRange(GL_UNIFORM_BUFFER, this->entity_ubo_data_index, this->entity_ubo, 0, buffer_size);

			// save uniform indices
			this->u_editor_index = glGetUniformLocation(this->glData.shaderId, "u_editor");
			this->u_minimap_index = glGetUniformLocation(this->glData.shaderId, "u_minimap");
			this->u_picking_index = glGetUniformLocation(this->glData.shaderId, "u_picking");
			this->u_alpha_index = glGetUniformLocation(this->glData.shaderId, "u_alpha");
		}
		void xml_entity_shader::draw(xml_entity_shader_data& entity_data)
		{
			auto shdID = this->glData.shaderId;
			auto VAO = this->glData.VAO;
			auto IBO = this->glData.IBO;

			glUseProgram(shdID);

			// bind buffer data
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, this->entity_data_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(entity_data.buffer_data), entity_data.buffer_data, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// bind textures
			int tex_count = 0;
			if (entity_data.shadow_flag == false)
			{
				// normal
				glUniform1i(glGetUniformLocation(shdID, "uTexture0"), tex_count);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, entity_data.opengl_texture_id_normal);
				tex_count++;

				// skin
				if (entity_data.has_mask == true)
				{
					glUniform1i(glGetUniformLocation(shdID, "uTexture1"), tex_count);
					glActiveTexture(GL_TEXTURE0 + tex_count);
					glBindTexture(GL_TEXTURE_2D, entity_data.opengl_texture_id_mask);
					tex_count++;
				}
			}
			else
			{
				// when i'm rendering in shadow mode
				glUniform1i(glGetUniformLocation(shdID, "uTexture0"), tex_count);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, entity_data.opengl_texture_id_shadow);
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

			//if (b_picking == true)
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glUseProgram(0);
		}

		std::shared_ptr<rattlesmake::image::png_data> xml_entity_shader::add_image_data(const uint32_t openglTextureId, const std::string& imageID, uint32_t image_width, uint32_t image_height, uint8_t image_channels)
		{
			entities_data.insert({ imageID, std::shared_ptr<rattlesmake::image::png_data>(new rattlesmake::image::png_data(openglTextureId, image_width, image_height, image_channels)) });
			return entities_data.at(imageID);
		}
		std::shared_ptr<rattlesmake::image::png_data> xml_entity_shader::get_image_data(const std::string& imageID) const
		{
			if (entities_data.contains(imageID) == false)
			{
				std::cout << "[DEBUG] image " << imageID << " not in cache" << std::endl;
				return std::shared_ptr<rattlesmake::image::png_data>();
			}

			return entities_data.at(imageID);
		}
		bool xml_entity_shader::check_if_png_in_cache(const std::string& imageID) const
		{
			return entities_data.contains(imageID);
		}

		xml_entity_shader::xml_entity_shader()
		{
		}
		xml_entity_shader::~xml_entity_shader()
		{
		}
	};
};
