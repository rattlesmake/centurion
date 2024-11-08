/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

#include <glm.hpp>

namespace rattlesmake
{
	namespace image
	{
		class png_data;
	};
};

namespace centurion
{
	namespace assets
	{
		class xml_entity;
		class xml_entity_image;
		class xml_entity_shader;
		enum class xml_entity_drawmode;

		/*
			class that contains information about entities' images (tag <image> in xml files)
		*/
		class xml_entity_image
		{
			friend class xml_entity;
			friend class xml_entity_shader;

		public:
			/*
				correct way to instantiate a new entity image
			*/
			static std::shared_ptr<xml_entity_image> create(const bool load_texture, const int id, const bool reverse, const std::string& zip_file, const std::string& png_path);

			const uint32_t get_width(void) const;
			const uint32_t get_height(void) const;
			const int32_t get_opengl_texture_id(void) const;

			~xml_entity_image();
		protected:
			/*
				private constructor
			*/
			xml_entity_image(const bool load_texture, const int id, const bool reverse, const std::string& zip_file, const std::string& png_path);

			/*
				private functions
			*/
			uint32_t bind_texture(const uint8_t* image_data, const uint32_t width, const uint32_t height);
			void read_image_from_path(const std::string& zip_key, const std::string& internal_path);
			void read_image_from_path();

			/*
				private members
			*/
			bool reverse = false;
			bool load_texture = true;
			bool valid = true;
			uint32_t id; // from xml

			std::string internal_string_id; // for texture caching
			std::string zip_file, png_path; // for texture caching

			std::weak_ptr<rattlesmake::image::png_data> texture_data;
		};
	};
};
