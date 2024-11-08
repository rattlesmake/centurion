#include "png.h"
#include "png_shader.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <execution>

#include <zipservice.h>
#include <fileservice.h>

#include <GL/glew.h>

namespace rattlesmake
{
	namespace image
	{
		std::unordered_map<std::string, png_flags> png::flagsMap{
			{ "background", png_flags_::png_flags_NoRepeat },
			{ "repeat-x", png_flags_::png_flags_RepeatX },
			{ "repeat-y", png_flags_::png_flags_RepeatY },
			{ "repeat-xy", png_flags_::png_flags_RepeatXY }
		};
		png_flags png::get_image_flag(const std::string& flagID)
		{
			if (flagsMap.contains(flagID) == false)
				return png_flags_::png_flags_NoRepeat;
			return flagsMap.at(flagID);
		}

#pragma region Constructor & Destructor
		png::png(const std::string& zipKey, const std::string& internalPath, png_flags flags_, float alpha_) :
			alpha(alpha_),
			flags(flags_)
		{
			this->bVerticallyFlipped = flags & png_flags_::png_flags_VerticallyFlipped ? true : false;
			this->rgb255 = flags & png_flags_::png_flags_RGB255 ? true : false;

			this->read_image_from_path(zipKey, internalPath, png_type::RGBA);
		}
		png::png(const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t n_channels, const std::string& imageID, png_flags flags_, float alpha_)
		{
			this->bVerticallyFlipped = flags & png_flags_::png_flags_VerticallyFlipped ? true : false;
			this->rgb255 = flags & png_flags_::png_flags_RGB255 ? true : false;

			this->read_image_from_bytes(data, width, height, n_channels, imageID, png_type::RGBA);
		}
		png_ptr png::create_from_bytes(const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t n_channels, const std::string& imageID, png_flags flags_, float alpha_)
		{
			return std::shared_ptr<png>();
		}
		png_ptr png::create(const std::string& zipKey, const std::string& internalPath, png_flags flags_, float alpha_)
		{
			auto new_image = std::shared_ptr<png>(new png(zipKey, internalPath, flags_, alpha_));
			new_image->me = new_image;
			return new_image;
		}
		png_ptr png::create(const std::string& internalPath, png_flags flags_, float alpha_)
		{
			return create("?ui.zip", internalPath, flags_, alpha_);
		}
		bool png::check_if_clicked(const int32_t x, const int32_t y, const uint32_t xLeftclick, const uint32_t yLeftclick, const uint32_t picking_ID, const int custom_width, const int custom_height)
		{
			this->render(x, y, this->flags, custom_width, custom_height, true, picking_ID);
			unsigned char data[4] = { 0 };
			glReadPixels(xLeftclick, yLeftclick, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
			unsigned int clickid = data[0] + data[1] * 256 + data[2] * 256 * 256;
			return (clickid == picking_ID);
		}
		void png::render(const int32_t x, const int32_t y, png_flags flags, const uint32_t customWidth, const uint32_t customHeight, bool picking, uint32_t pickingID)
		{
			if (this->is_png_read_correctly() == false)
				return;

			this->set_flags(flags);
			this->set_position(x, y);
			this->set_custom_size(customWidth, customHeight);
			this->pickingId = pickingID;

			png_shader::get_instance().draw(this->me.lock(), picking);
		}
		png::~png()
		{
		}
#pragma endregion

#pragma region Get & Set
		void png::set_flipped_value(const bool flipped)
		{
			this->bFlipped = flipped;
		}
		uint32_t png::get_width(void) const
		{
			return this->textureData.lock()->get_image_width();
		}
		uint32_t png::get_height(void) const
		{
			return this->textureData.lock()->get_image_height();
		}
		uint32_t png::get_channels(void) const
		{
			return this->textureData.lock()->get_image_channels();
		}
		bool png::is_png_read_correctly(void)
		{
			return this->textureData.expired() == false;
		}
		void png::set_custom_width(const uint32_t val)
		{
			this->customWidth = val;
		}
		void png::set_custom_height(const uint32_t val)
		{
			this->customHeight = val;
		}
		void png::set_flags(const int val)
		{
			this->flags = val;
		}
		void png::set_position(const int32_t x, const int32_t y)
		{
			this->xPosition = x;
			this->yPosition = y;
		}
		void png::set_custom_size(const uint32_t w, const uint32_t h)
		{
			this->set_custom_width(w);
			this->set_custom_height(h);
		}
		uint32_t png::get_opengl_texture_id(void) const
		{
			return this->textureData.lock()->get_opengl_texture_id();
		}
#pragma endregion

#pragma region Read & Write
		void png::read_image_from_path(const std::string& zipKey, const std::string& internalPath, const png_type imgType)
		{
			this->stringID = internalPath;
			auto& imageShader = png_shader::get_instance();

			if (imageShader.check_if_png_in_cache(this->stringID) == false)
			{
				int w = 0, h = 0, n = 0;
				auto data = rattlesmake::services::zip_service::get_instance().get_image_data(zipKey, internalPath + ".png", &w, &h, &n, (int)imgType);
				if (data == nullptr)
				{
					std::cout << "[DEBUG] error reading image " << internalPath << " in zip " << zipKey << std::endl;
					return;
				}
				uint32_t u_width = (uint32_t)w;
				uint32_t u_height = (uint32_t)h;
				uint32_t u_nchannels = (uint32_t)n;
				this->textureData = imageShader.add_image_data(this->bind_texture(data, u_width, u_height), this->stringID, u_width, u_height, u_nchannels);
				stb::free(data);
			}
			else
			{
				this->textureData = imageShader.get_image_data(this->stringID);
			}
		}
		void png::read_image_from_bytes(const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t n_channels, const std::string& imageID, const png_type imgType)
		{
			this->stringID = imageID;
			auto& imageShader = png_shader::get_instance();

			if (imageShader.check_if_png_in_cache(this->stringID) == false)
			{
				int w = 0, h = 0, n = 0;
				this->textureData = imageShader.add_image_data(this->bind_texture(data, width, height), this->stringID, width, height, n_channels);
			}
			else
			{
				this->textureData = imageShader.get_image_data(this->stringID);
			}
		}
		uint32_t png::bind_texture(const uint8_t* image_data, const uint32_t width, const uint32_t height)
		{
			uint32_t textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// create texture and generate mipmaps
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return textureID;
		}
		void png::update_texture(const uint8_t* image_data)
		{
			auto texData = this->textureData.lock();
			if (texData)
			{
				glBindTexture(GL_TEXTURE_2D, texData->get_opengl_texture_id());
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texData->get_image_width(), texData->get_image_height(), GL_RGBA, GL_UNSIGNED_BYTE, image_data);
			}
		}
#pragma endregion
	};
};
