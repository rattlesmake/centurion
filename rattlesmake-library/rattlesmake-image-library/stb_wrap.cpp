#include "stb_wrap.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION  
#endif
#include <stb_image.h> 

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION  
#endif
#include <stb_image_write.h>

namespace rattlesmake
{
	namespace image
	{
		namespace stb
		{
			int write_png_to_func(write_func* func, void* context, int x, int y, int comp, const void* data, int stride_bytes)
			{
				return stbi_write_png_to_func(func, context, x, y, comp, data, stride_bytes);
			}
			int write_png_to_file(char const* filename, int x, int y, int comp, const void* data, int stride_bytes)
			{
				return stbi_write_png(filename, x, y, comp, data, stride_bytes);
			}
			void flip_vertically_on_write(int flag)
			{
				stbi_flip_vertically_on_write(flag);
			}
			void flip_vertically_on_load(int flag)
			{
				stbi_set_flip_vertically_on_load(flag);
			}
			void free(void* retval_from_stbi_load)
			{
				stbi_image_free(retval_from_stbi_load);
			}
			uint8_t* load_from_memory(uint8_t const* buffer, int len, int* x, int* y, int* comp, int req_comp)
			{
				return stbi_load_from_memory(buffer, len, x, y, comp, req_comp);
			}
			uint8_t* load_from_file(char const* filename, int* x, int* y, int* comp, int req_comp)
			{
				return stbi_load(filename, x, y, comp, req_comp);
			}
		};
	};
};
