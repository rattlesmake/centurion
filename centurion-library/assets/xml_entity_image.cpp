#include "xml_entity_image.h"
#include "xml_entity_shader.h"
#include "xml_entity.h"

// services
#include <zipservice.h>
#include <fileservice.h>

// png
#include <png.h>
#include <png_shader.h>
#include <stb_wrap.h>

// opengl
#include <GL/glew.h> 

// stl
#include <iostream>

namespace centurion
{
	namespace assets
	{
		std::shared_ptr<xml_entity_image> xml_entity_image::create(const bool load_texture, const int id, const bool reverse, const std::string& zip_file, const std::string& png_path)
		{
			std::shared_ptr<xml_entity_image> new_img{ new xml_entity_image(load_texture, id, reverse, zip_file, png_path) };
			if (!new_img || !new_img->valid)
				return std::shared_ptr<xml_entity_image>();
			return new_img;
		}
		
		xml_entity_image::~xml_entity_image()
		{
		}
		xml_entity_image::xml_entity_image(const bool load_texture, const int id, const bool reverse, const std::string& zip_file, const std::string& png_path) :
			id((uint32_t)id),
			reverse(reverse),
			load_texture(load_texture),
			zip_file(zip_file),
			png_path(png_path)
		{
			if (load_texture)
				read_image_from_path(zip_file, png_path);
		}
		const uint32_t xml_entity_image::get_width(void) const
		{
			return this->texture_data.lock()->get_image_width();
		}
		const uint32_t xml_entity_image::get_height(void) const
		{
			return this->texture_data.lock()->get_image_height();
		}
		const int32_t xml_entity_image::get_opengl_texture_id(void) const
		{
			return texture_data.lock()->get_opengl_texture_id();
		}
		void xml_entity_image::read_image_from_path(const std::string& zip_key, const std::string& internal_path)
		{
			auto& zip_s = rattlesmake::services::zip_service::get_instance();
			auto& xml_ent_shader = xml_entity_shader::get_instance();

			// generate id
			internal_string_id = zip_key + "##" + internal_path;

			if (xml_ent_shader.check_if_png_in_cache(internal_string_id) == false)
			{
				int w = 0, h = 0, n = 0;
				auto data = zip_s.get_image_data(zip_key, internal_path + ".png", &w, &h, &n, 4);
				if (data == nullptr)
				{
					std::cout << "[DEBUG] error reading image " << internal_path << " in zip " << zip_key << std::endl;
					valid = false;
					return;
				}
				uint32_t u_width = (uint32_t)w;
				uint32_t u_height = (uint32_t)h;
				uint32_t u_nchannels = (uint32_t)n;
				texture_data = xml_ent_shader.add_image_data(this->bind_texture(data, u_width, u_height), internal_string_id, u_width, u_height, u_nchannels);
				rattlesmake::image::stb::free(data);
			}
			else
			{
				texture_data = xml_ent_shader.get_image_data(internal_string_id);
			}
		}
		void xml_entity_image::read_image_from_path()
		{
			if (zip_file.empty() || png_path.empty())
			{
				valid = false;
				return;
			}
			read_image_from_path(zip_file, png_path);
		}
		uint32_t xml_entity_image::bind_texture(const uint8_t* image_data, const uint32_t width, const uint32_t height)
		{
			uint32_t textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// create texture and generate mipmaps
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return textureID;
		}
	};
};