/*
* ---------------------------
* CENTURION IMAGE LIBRARY
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <tuple>
#include <list>
#include <unordered_map>
#include <memory>

#include <GL/glew.h>
#include <glm.hpp>

#include <new_shader.h> // needed

namespace rattlesmake
{
	namespace image
	{
		class png;
		class png_shader;

		class png_data
		{
			friend class png_shader;
		public:
			uint32_t get_opengl_texture_id(void) const;
			uint32_t get_image_width(void) const;
			uint32_t get_image_height(void) const;
			uint32_t get_image_channels(void) const;
			png_data(uint32_t opengl_texture_id, uint32_t image_width, uint32_t image_height, uint8_t image_channels);
			~png_data(void);
		private:
			uint32_t openglTextureId = 0;
			uint32_t imageWidth = 0, imageHeight = 0;
			uint8_t imageChannels = 0;
		};

		class png_shader : public rattlesmake::utils::opengl_shader
		{
			friend class png;

		public:

			// todo 
			// singleton
			static png_shader& get_instance(void);

			[[nodiscard]] std::shared_ptr<png_data> get_ui_image_data(const std::string& imageID) const;
			void initialize(const glm::mat4& projMatrixInit, const glm::mat4& viewMatrixInit);
			void clear_cache(void);

			[[nodiscard]] std::shared_ptr<png_data> add_image_data(const uint32_t openglTextureId, const std::string& imageID, uint32_t image_width, uint32_t image_height, uint8_t image_channels);
			[[nodiscard]] std::shared_ptr<png_data> get_image_data(const std::string& imageID) const;
			[[nodiscard]] bool check_if_png_in_cache(const std::string& imageID) const;

			// just for for testing
			void read_texture(const std::string& image_path, uint32_t* opengl_texture_id) const;
			void draw_texture(const glm::ivec2& position, const glm::ivec2& img_size, const uint32_t opengl_texture_id) const;

			~png_shader(void);

		protected:
			explicit png_shader(void);

			// only png can access to these methods
			void draw(std::shared_ptr<png> image, const bool bPicking);

			void set_uniforms(const std::shared_ptr<png>& image, const std::shared_ptr<png_data>& imageData);
			void set_picking_uniforms(const std::shared_ptr<png>& image, const std::shared_ptr<png_data>& imageData);

			std::string get_vertex_shader(void) override;
			std::string get_fragment_shader(void) override;
			void generate_buffers(void) override;

			std::unordered_map<std::string, std::shared_ptr<png_data>> textureData;
			std::list<std::string> textureDataKeys;

			static png_shader instance;
		};
	};
};

