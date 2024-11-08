/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>

#include "xml_entity_structs.h"
#include <xml_typedef.h>

namespace tinyxml2
{
	class XMLElement;
};

namespace rattlesmake
{
	namespace services
	{
		class zip_service;
		class file_service;
	};
};

namespace centurion
{
	namespace assets
	{
		class xml_entity_image;
		struct xml_entity_layer_data;

		/*
			this class contains the information of entity files
			e.g. class_name.oe.xml
		*/
		class xml_entity
		{
		public:

			/*
				the right way to instantiate a new xml_entity object
			*/
			[[nodiscard]] static xmlEntitySP_t create(const std::string& ent_path);
			xml_entity(const xml_entity& other) = delete;
			xml_entity& operator=(const xml_entity& other) = delete;
			
			/*
				loads and binds ALL texture images
			*/
			void load_textures();

			/*
				loads and binds ONLY IDLE texture images
			*/
			void load_only_idle_textures(const std::string& state_name);
			
			/*
				render function
			*/
			void render(const std::string& state_name, const std::string& anim_name, const uint32_t x_pos, const uint32_t y_pos) const;

			/*
				get entity width and height
			*/
			[[nodiscard]] std::pair<uint32_t, uint32_t> get_entity_size(void) const noexcept;

			/*
				get entity origin offsets
			*/
			[[nodiscard]] std::pair<int32_t, int32_t> get_origin_offsets(void) const noexcept;

			/*
				get the number of frames / total milliseconds of a given animation (by id)
			*/
			[[nodiscard]] size_t get_anim_number_of_frames(const std::string& state_name, const std::string& anim_name) const noexcept;
			[[nodiscard]] uint32_t get_anim_total_duration(const std::string& state_name, const std::string& anim_name) const noexcept;
			[[nodiscard]] uint32_t get_anim_frame_duration(const std::string& state_name, const std::string& anim_name) const noexcept;
			[[nodiscard]] bool check_if_animation_exists(const std::string& state_name, const std::string& anim_name) const noexcept;

			/*
				get directions
			*/
			[[nodiscard]] uint16_t get_number_of_directions(void) const noexcept;

			/*
				get points by type
			*/
			[[nodiscard]] const entityPointsVec_t& get_points_by_type_cref(const std::string& type) const;

			/*
				get image by id
			*/
			[[nodiscard]] const std::shared_ptr<xml_entity_image> get_image_by_id(const uint32_t id) const;

			/*
				destructor
			*/
			~xml_entity(void);
		private:

			/*
				private constructor
			*/
			explicit xml_entity(std::string ent_path);
			
			/*
				private functions
			*/
			void read_basic_info(tinyxml2::XMLElement* xml_element);
			void read_hitbox(tinyxml2::XMLElement* xml_hitbox);
			void read_properties(tinyxml2::XMLElement* xml_properties);
			void read_points(tinyxml2::XMLElement* xml_points);
			void read_images(tinyxml2::XMLElement* xml_images);
			void read_states(tinyxml2::XMLElement* xml_states);
			void read_animations(tinyxml2::XMLElement* xml_animations, xml_entity_state& state);
			void read_animation_layers_and_shadow(tinyxml2::XMLElement* xml_layers, tinyxml2::XMLElement* xml_shadow, xml_entity_animation& anim);

			/*
				private members
			*/
			std::string ent_file_path;
			std::string ent_folder_path;
			std::string class_name;
			std::string zip_file;
			std::string type;
			uint32_t directions;
			int32_t origin_offset_x;
			int32_t origin_offset_y;
			uint32_t hitbox_width;
			uint32_t hitbox_height;

			/*
				main containers
			*/
			std::vector<xml_entity_property> properties_vec;
			std::unordered_map<int, entityPointsVec_t> points_map;
			std::unordered_map<uint32_t, std::shared_ptr<xml_entity_image>> images_map;
			std::unordered_map<std::string, xml_entity_state> states_map;

			/*
				shortcuts
			*/
			rattlesmake::services::zip_service& zip_s;
			rattlesmake::services::file_service& file_s;

			/*
				static members
			*/
			static std::unordered_map<std::string, int> static_points_map;
		};
	};
};
