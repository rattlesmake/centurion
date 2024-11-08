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
#include <unordered_map>
#include <memory>

#include "stb_wrap.h"

#ifndef RIL_IMAGE_DEFAULT_VAL
#define RIL_IMAGE_DEFAULT_VAL 0
#endif

namespace rattlesmake
{
	namespace image
	{
		// wrap stb image functions
		namespace stb
		{
			typedef void write_func(void* context, void* data, int size);
			int write_png_to_func(write_func* func, void* context, int x, int y, int comp, const void* data, int stride_bytes);
			int write_png_to_file(char const* filename, int x, int y, int comp, const void* data, int stride_bytes);
			void flip_vertically_on_write(int flag);
			void flip_vertically_on_load(int flag);
			void free(void* retval_from_stbi_load);
			uint8_t* load_from_memory(uint8_t const* buffer, int len, int* x, int* y, int* comp, int req_comp);
			uint8_t* load_from_file(char const* filename, int* x, int* y, int* comp, int req_comp);
		};

		class png;
		class png_shader;
		class png_data;
		typedef std::shared_ptr<png> png_ptr;

		typedef int png_flags;
		enum png_flags_
		{
			png_flags_None = 0,
			png_flags_VerticallyFlipped = 1 << 0,
			png_flags_RGB255 = 1 << 1,
			png_flags_RepeatX = 1 << 2,
			png_flags_RepeatY = 1 << 3,
			png_flags_RepeatXY = 1 << 4,
			png_flags_NoRepeat = 1 << 5
		};

		enum class png_type
		{
			RGBA = 4,
			RGB = 3
		};

		/*
		* This class is rendered by png_shader
		*/
		class png
		{
			friend class png_shader;

		public:

#pragma region Constructors and destructor:
			png(const png& other) = delete;
			virtual ~png(void);
#pragma endregion

			static png_ptr create_from_bytes(const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t n_channels, const std::string& imageID, png_flags flags_, float alpha_);
			static png_ptr create(const std::string& zipKey, const std::string& internalPath, png_flags flags_, float alpha_);
			static png_ptr create(const std::string& internalPath, png_flags flags_, float alpha_); // expected zipFile is UI.zip
			static png_flags get_image_flag(const std::string& flagID);

			[[nodiscard]] bool check_if_clicked(const int32_t x, const int32_t y, const uint32_t xLeftclick, const uint32_t yLeftclick, const uint32_t picking_ID, const int custom_width, const int custom_height);
			void render(const int32_t x, const int32_t y, png_flags flags, const uint32_t customWidth, const uint32_t customHeight, bool picking = false, uint32_t pickingID = 0);

			void set_flipped_value(const bool flipped);
			uint32_t get_opengl_texture_id(void) const;
			uint32_t get_width(void) const;
			uint32_t get_height(void) const;
			uint32_t get_channels(void) const;
			bool is_png_read_correctly(void);

			void set_custom_width(const uint32_t val);
			void set_custom_height(const uint32_t val);
			void set_flags(const int val);
			void set_position(const int32_t x, const int32_t y);
			void set_custom_size(const uint32_t w, const uint32_t h);

		protected:
			png(const std::string& zipKey, const std::string& internalPath, png_flags flags_, float alpha_);
			png(const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t n_channels, const std::string& imageID, png_flags flags_, float alpha_);
			png() {}
			void read_image_from_path(const std::string& zipKey, const std::string& internalPath, const png_type imgType);
			void read_image_from_bytes(const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t n_channels, const std::string& imageID, const png_type imgType);
			virtual uint32_t bind_texture(const uint8_t* image_data, const uint32_t width, const uint32_t height);
			virtual void update_texture(const uint8_t* image_data);

			bool bFlipped = false;
			std::string stringID;

			png_flags flags = png_flags_None;

			int32_t xPosition = 0, yPosition = 0; // it can be negative
			uint32_t customWidth = 0, customHeight = 0;
			uint32_t pickingId = 0;
			float alpha = 1.f;
			bool bVerticallyFlipped = false;
			bool rgb255 = false;

			std::weak_ptr<png_data> textureData;
			std::weak_ptr<png> me;

		private:

			static std::unordered_map<std::string, png_flags> flagsMap;
		};
	};
};
