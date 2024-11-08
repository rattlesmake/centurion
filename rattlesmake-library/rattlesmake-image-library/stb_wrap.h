/*
* ---------------------------
* CENTURION IMAGE LIBRARY
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <stdint.h>

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
	};
};
