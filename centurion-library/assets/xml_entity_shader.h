/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <png_shader.h>
#include <map>

namespace centurion
{
	namespace assets
	{
		class xml_entity_layer;
		class xml_entity_shadow;
		struct xml_entity_layer_data;

		class xml_entity_shader : public rattlesmake::image::png_shader
		{
			friend class xml_entity_layer;
			friend class xml_entity_shadow;

		public:

			// singleton
			static xml_entity_shader& get_instance(void);

			/*
				method to call just once, in your engine init function
			*/
			void initialize(void);

			/*
				method to call in any environment constructor
			*/
			void clear_cache(void);

			void begin_frame_game(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const bool minimap_is_active, const bool is_editor, const float game_time);

			/*
				activate and deactivate picking boolean
			*/
			void activate_picking(void);
			void deactivate_picking(void);

			/*
				xml_entity_shader_data void functions 
			*/
			void initialize_draw_data(void);
			void new_object_draw_data(const uint32_t current_frame, const uint8_t current_direction, const glm::vec3& player_color, const uint32_t level, bool is_selected, bool is_placeable, bool is_unit);
			void new_object_picking_draw_data(const uint32_t current_frame, const uint8_t current_direction, const uint32_t picking_id);
			void new_entity_draw_data(const uint32_t x_pos, const uint32_t y_pos, const uint32_t total_frames, const uint32_t total_directions);
			void new_layer_draw_data(const bool has_mask, const glm::uvec2& base_image_size, uint32_t base_image_opengl_id, int32_t mask_image_opengl_id);
			void new_shadow_draw_data(const glm::uvec2& image_size, const int32_t opengl_texture_id);
			
			void render_current_draw_data(void);
			void push_current_draw_data(void);
			void render_all_draw_data(void);

			// properties
			bool picking = false;

			/*
				cache functions
			*/
			[[nodiscard]] std::shared_ptr<rattlesmake::image::png_data> add_image_data(const uint32_t openglTextureId, const std::string& imageID, uint32_t image_width, uint32_t image_height, uint8_t image_channels);
			[[nodiscard]] std::shared_ptr<rattlesmake::image::png_data> get_image_data(const std::string& imageID) const;
			[[nodiscard]] bool check_if_png_in_cache(const std::string& imageID) const;

			~xml_entity_shader();
		protected:



			//
			// struct that contains data needed in the shader
			//
			struct xml_entity_shader_data
			{
				//
				//	structure:
				//
				//	buffer_data[0]  = x_pos
				//	buffer_data[1]  = y_pos
				//	buffer_data[2]  = image_width
				//	buffer_data[3]  = image_height
				//
				//	buffer_data[4]  = frames
				//	buffer_data[5]  = directions
				//	buffer_data[6]  = current_frame
				//	buffer_data[7]  = current_direction
				//
				//	buffer_data[8]  = level
				//	buffer_data[9]  = player_color_id
				//	buffer_data[10] = has_mask
				//	buffer_data[11] = shadow
				//
				//	buffer_data[12] = is_selected
				//	buffer_data[13] = is_placeable
				//	buffer_data[14] = picking_id
				//	buffer_data[15] = empty
				//

				inline xml_entity_shader_data()
				{

				}

				inline xml_entity_shader_data(const uint32_t current_frame, const uint8_t current_direction, const glm::vec3& player_color, const uint32_t level, bool is_selected, bool is_placeable, bool is_unit) :
					is_unit(is_unit)
				{
					buffer_data[6] = (float)current_frame;
					buffer_data[7] = (float)current_direction;
					buffer_data[8] = (float)level;
					buffer_data[9] = vec3_to_float(player_color);
					buffer_data[12] = (float)is_selected;
					buffer_data[13] = (float)is_placeable;
					buffer_data[15] = 0.f;
				}
				inline void set_flags(const bool shadow, const bool has_mask)
				{
					this->has_mask = has_mask;
					this->shadow_flag = shadow;
					buffer_data[10] = (float)has_mask;
					buffer_data[11] = (float)shadow;
				}

				inline float vec3_to_float(const glm::vec3& color)
				{
					return (float)(color.r + color.g * 256 + color.b * 256 * 256);
				}
				inline void set_picking_id(const uint32_t picking_id)
				{
					buffer_data[14] = (float)picking_id;
				}
				inline void set_position(const uint32_t x_pos, const uint32_t y_pos)
				{
					buffer_data[0] = (float)x_pos;
					buffer_data[1] = (float)y_pos;
					sorting_key = -1 * (int)y_pos;
				}
				inline float get_position_x(void)
				{
					return buffer_data[0];
				}
				inline float get_position_y(void)
				{
					return buffer_data[1];
				}
				inline void set_image_size(const uint32_t image_width, const uint32_t image_height)
				{
					buffer_data[2] = (float)image_width;
					buffer_data[3] = (float)image_height;
				}
				inline void set_frames_and_directions(const int32_t frames, const int32_t directions)
				{
					buffer_data[4] = (float)frames;
					buffer_data[5] = (float)directions;
				}
				inline const bool get_shadow_flag(void) const
				{
					return shadow_flag;
				}

				int32_t opengl_texture_id_normal{ -1 };
				int32_t opengl_texture_id_shadow{ -1 };
				int32_t opengl_texture_id_mask{ -1 };
				bool shadow_flag = false;
				bool has_mask = false;
				bool is_unit = false;
				int32_t sorting_key = 0;

				float buffer_data[16] = { 0.f };
			} current_entity_data;

			std::vector<xml_entity_shader_data> unordered_layers_data;
			std::map<int32_t, std::vector<xml_entity_shader_data>> ordered_layers_data; // ordered by Y
			std::map<int32_t, std::vector<xml_entity_shader_data>> ordered_alpha_layers_data; // ordered by Y




			xml_entity_shader();

			/*
				private functions
			*/
			std::string get_vertex_shader(void) override;
			std::string get_fragment_shader(void) override;
			void generate_buffers(void) override;

			void draw(xml_entity_shader_data& entity_data);

			uint32_t entity_data_vbo;
			uint32_t entity_ubo, entity_ubo_data_index;
			uint32_t u_minimap_index, u_editor_index, u_alpha_index, u_picking_index;

			/*
				activate and deactivate alpha boolean (opengl)
			*/
			void activate_alpha(void);
			void deactivate_alpha(void);

			/*
				container
			*/
			std::unordered_map<std::string, std::shared_ptr<rattlesmake::image::png_data>> entities_data;

			/*
				static members
			*/
			static xml_entity_shader instance;
		};

	};
};
