#include "png_shader.h"
#include "png.h"

#include <fileservice.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

namespace rattlesmake
{
	namespace image
	{
		png_shader png_shader::instance;

		png_shader& png_shader::get_instance(void)
		{
			return instance;
		}

		png_shader::png_shader(void)
		{
		}

		std::shared_ptr<png_data> png_shader::get_ui_image_data(const std::string& imageID) const
		{
			if (this->check_if_png_in_cache(imageID) == false)
			{
				// this image is created just to read and save image data
				// then it's deleted
				auto img_temp = png::create("?ui.zip", imageID, png_flags_::png_flags_None, 1.f);
			}

			return this->get_image_data(imageID);
		}

		void png_shader::initialize(const glm::mat4& projMatrixInit, const glm::mat4& viewMatrixInit)
		{
			// compile shaders
			this->compile_shaders(this->get_vertex_shader().c_str(), this->get_fragment_shader().c_str());

			// generate buffers
			this->generate_buffers();

			// initialize matrices
			this->apply_matrices(projMatrixInit, viewMatrixInit);
		}
		void png_shader::clear_cache(void)
		{
			std::list<std::string>::iterator keysIt = this->textureDataKeys.begin();
			while (keysIt != this->textureDataKeys.end())
			{
				std::string key = (*keysIt);

				// we keep in cache just "common" images
				bool bKeepCondition = (key.find("common") != std::string::npos);

				if (bKeepCondition == false)
				{
					auto thisKeysIt = keysIt;
					keysIt++; //Go to the next unit.
					this->textureData.erase(key);
					this->textureDataKeys.erase(thisKeysIt);
				}
				else
				{
					keysIt++; //Go to the next unit.
				}
			}
		}
		png_shader::~png_shader(void) {}

		std::string png_shader::get_vertex_shader(void)
		{
			return R"(
#version 330 core
// Uniform variables
uniform int uPosX;
uniform int uPosY;
uniform int uSizeW;
uniform int uSizeH;
uniform int uRepeat;
uniform int uYFlipped;
uniform float uRatioX;
uniform float uRatioY;
uniform mat4 uProjection;
uniform mat4 uView;

// Input variables
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

// Output variables
out vec2 FragTex;

float x1, y1;

void main()
{   
	x1 = pos.x * uSizeW + uPosX;
    y1 = pos.y * uSizeH + uPosY;
	FragTex = tex;
    
	if (uYFlipped == 1) FragTex.y = -FragTex.y;

    gl_Position = uProjection * uView * vec4(x1, y1, 0.0, 1.0);
	
	if (uRepeat == 1) // repeat-xy
	{
		FragTex = vec2(tex.x * uRatioX, tex.y * uRatioY);
	}
	else if (uRepeat == 2) // repeat-x
	{
		FragTex = vec2(tex.x * uRatioX, tex.y);
	}
	else if (uRepeat == 3) // repeat-y
	{
		FragTex = vec2(tex.x, tex.y * uRatioX);
	}
}  
)";
		}
		std::string png_shader::get_fragment_shader(void)
		{
			return R"(
#version 330 core
// Uniform variables
uniform int uPicking;
uniform int uRgb255;
uniform float uAlpha;
uniform vec4 uPickingColor;
uniform sampler2D uTexture1;

// Input variables
in vec2 FragTex;

// Output variables
out vec4 FragColor;

void main()
{
    if (uPicking == 1){
        FragColor = vec4(uPickingColor.r, uPickingColor.g, uPickingColor.b, texture(uTexture1, FragTex).a);
    }
    else {
        vec4 col = texture(uTexture1, FragTex);
		float a = col.a * uAlpha;
        if (uRgb255 == 1)
        {
            col = vec4(col.xyz * 255.f, a);
        }
        FragColor = vec4(col.xyz, a);
    }
}
)";
		}

		void png_shader::generate_buffers(void)
		{
			unsigned int indices[6] = {
				0, 1, 3,   // first triangle
				1, 2, 3    // second triangle
			};
			GLfloat vertices[16] = {
				// positions	// uv coords		
				0.f,	0.f,	0.f, 1.f,	// in basso a sx
				1.f,	0.f,	1.f, 1.f,	// in basso a dx
				1.f,	1.f,	1.f, 0.f,	// in alto a dx
				0.f,	1.f,	0.f, 0.f	// in alto a sx
			};

			glGenVertexArrays(1, &this->glData.VAO);
			glBindVertexArray(this->glData.VAO);

			glGenBuffers(1, &this->glData.IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glData.IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glGenBuffers(1, &this->glData.VBO);
			glBindBuffer(GL_ARRAY_BUFFER, this->glData.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void png_shader::draw(std::shared_ptr<png> image, const bool bPicking)
		{
			auto shdID = this->glData.shaderId;
			auto texData = this->get_image_data(image->stringID);

			// check that image data exists
			if (!texData)
				return;

			auto VAO = this->glData.VAO;
			auto IBO = this->glData.IBO;

			glUseProgram(shdID);

			// set uniforms
			bPicking ? this->set_picking_uniforms(image, texData) : this->set_uniforms(image, texData);

			glBindVertexArray(VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texData->openglTextureId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

			if (bPicking == true)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glUseProgram(0);
		}
		void png_shader::read_texture(const std::string& image_path, uint32_t* opengl_texture_id) const
		{
			int w = 0, h = 0, n = 0;
			uint8_t* data = stb::load_from_file(image_path.c_str(), &w, &h, &n, 4);
			assert(data != nullptr);

			glGenTextures(1, opengl_texture_id);
			glBindTexture(GL_TEXTURE_2D, *opengl_texture_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// create texture and generate mipmaps
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		void png_shader::draw_texture(const glm::ivec2& position, const glm::ivec2& img_size, const uint32_t opengl_texture_id) const
		{
			// use this function for testing
			// e.g. render a simple texture

			auto shdID = this->glData.shaderId;
			auto VAO = this->glData.VAO;
			auto IBO = this->glData.IBO;

			glUseProgram(shdID);

			// set uniforms
			glUniform1i(glGetUniformLocation(shdID, "uPosX"), position.x);
			glUniform1i(glGetUniformLocation(shdID, "uPosY"), position.y);
			glUniform1i(glGetUniformLocation(shdID, "uSizeW"), img_size.x);
			glUniform1i(glGetUniformLocation(shdID, "uSizeH"), img_size.y);
			glUniform1f(glGetUniformLocation(shdID, "uAlpha"), 1.f);
			glUniform1f(glGetUniformLocation(shdID, "uRatioX"), 1.f);
			glUniform1f(glGetUniformLocation(shdID, "uRatioY"), 1.f);
			glUniform1i(glGetUniformLocation(shdID, "uRepeat"), 0);
			glUniform1i(glGetUniformLocation(shdID, "uPicking"), 0);
			glUniform1i(glGetUniformLocation(shdID, "uYFlipped"), 0);
			glUniform1i(glGetUniformLocation(shdID, "uRgb255"), 0);

			glBindVertexArray(VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, opengl_texture_id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glUseProgram(0);
		}
		void png_shader::set_uniforms(const std::shared_ptr<png>& image, const std::shared_ptr<png_data>& imageData)
		{
			int imageWidth = int((image->customWidth != RIL_IMAGE_DEFAULT_VAL) ? image->customWidth : imageData->imageWidth);
			int imageHeight = int((image->customHeight != RIL_IMAGE_DEFAULT_VAL) ? image->customHeight : imageData->imageHeight);

			auto shdID = this->glData.shaderId;

			glUniform1i(glGetUniformLocation(shdID, "uPosX"), image->xPosition);
			glUniform1i(glGetUniformLocation(shdID, "uPosY"), image->yPosition);

			glUniform1i(glGetUniformLocation(shdID, "uSizeW"), imageWidth);
			glUniform1i(glGetUniformLocation(shdID, "uSizeH"), imageHeight);
			glUniform1f(glGetUniformLocation(shdID, "uAlpha"), image->alpha);

			if (image->customWidth != RIL_IMAGE_DEFAULT_VAL) glUniform1f(glGetUniformLocation(shdID, "uRatioX"), float(image->customWidth) / float(imageData->imageWidth));
			else glUniform1f(glGetUniformLocation(shdID, "uRatioX"), 1);
			if (image->customHeight != RIL_IMAGE_DEFAULT_VAL) glUniform1f(glGetUniformLocation(shdID, "uRatioY"), float(image->customHeight) / float(imageData->imageHeight));
			else glUniform1f(glGetUniformLocation(shdID, "uRatioY"), 1);

			if (image->flags & png_flags_::png_flags_NoRepeat)
			{
				glUniform1i(glGetUniformLocation(shdID, "uRepeat"), 0);
				glUniform1f(glGetUniformLocation(shdID, "uRatioX"), 1);
				glUniform1f(glGetUniformLocation(shdID, "uRatioY"), 1);
			}
			else if (image->flags & png_flags_::png_flags_RepeatXY)
			{
				float ratioX = imageWidth / (float(imageData->imageWidth) * imageHeight / float(imageData->imageHeight));
				float ratioY = imageHeight / (float(imageData->imageHeight) * imageWidth / float(imageData->imageWidth));
				glUniform1i(glGetUniformLocation(shdID, "uRepeat"), 1);
				glUniform1f(glGetUniformLocation(shdID, "uRatioX"), ratioX);
				glUniform1f(glGetUniformLocation(shdID, "uRatioY"), ratioY);
			}
			else if (image->flags & png_flags_::png_flags_RepeatX)
			{
				float ratioX = imageWidth / (float(imageData->imageWidth) * imageHeight / float(imageData->imageHeight));
				glUniform1i(glGetUniformLocation(shdID, "uRepeat"), 2);
				glUniform1f(glGetUniformLocation(shdID, "uRatioX"), ratioX);
				glUniform1f(glGetUniformLocation(shdID, "uRatioY"), 1);
			}
			else if (image->flags & png_flags_::png_flags_RepeatY)
			{
				float ratioY = imageHeight / (float(imageData->imageHeight) * imageWidth / float(imageData->imageWidth));
				glUniform1i(glGetUniformLocation(shdID, "uRepeat"), 3);
				glUniform1f(glGetUniformLocation(shdID, "uRatioX"), 1);
				glUniform1f(glGetUniformLocation(shdID, "uRatioY"), ratioY);
			}

			glUniform1i(glGetUniformLocation(shdID, "uPicking"), 0);
			glUniform1i(glGetUniformLocation(shdID, "uYFlipped"), int(image->bVerticallyFlipped));
			glUniform1i(glGetUniformLocation(shdID, "uRgb255"), int(image->rgb255));
		}

		void png_shader::set_picking_uniforms(const std::shared_ptr<png>& image, const std::shared_ptr<png_data>& imageData)
		{
			int imageWidth = int((image->customWidth != RIL_IMAGE_DEFAULT_VAL) ? image->customWidth : imageData->imageWidth);
			int imageHeight = int((image->customHeight != RIL_IMAGE_DEFAULT_VAL) ? image->customHeight : imageData->imageHeight);

			auto shdID = this->glData.shaderId;

			glUniform1i(glGetUniformLocation(shdID, "uPosX"), image->xPosition);
			glUniform1i(glGetUniformLocation(shdID, "uPosY"), image->yPosition);
			glUniform1i(glGetUniformLocation(shdID, "uSizeW"), imageWidth);
			glUniform1i(glGetUniformLocation(shdID, "uSizeH"), imageHeight);

			if (image->customWidth != RIL_IMAGE_DEFAULT_VAL) glUniform1f(glGetUniformLocation(shdID, "uRatioX"), float(image->customWidth) / float(imageData->imageWidth));
			else glUniform1f(glGetUniformLocation(shdID, "uRatioX"), 1);
			if (image->customHeight != RIL_IMAGE_DEFAULT_VAL) glUniform1f(glGetUniformLocation(shdID, "uRatioY"), float(image->customHeight) / float(imageData->imageHeight));
			else glUniform1f(glGetUniformLocation(shdID, "uRatioY"), 1);

			glUniform1i(glGetUniformLocation(shdID, "uRepeat"), 0);
			glUniform1i(glGetUniformLocation(shdID, "uPicking"), 1);
			glUniform1i(glGetUniformLocation(shdID, "uYFlipped"), int(image->bVerticallyFlipped));

			glm::vec3 pickCol = this->convert_picking_id_to_vec3(image->pickingId);
			glUniform4f(glGetUniformLocation(shdID, "uPickingColor"), pickCol.r / 255.f, pickCol.y / 255.f, pickCol.b / 255.f, 1.0f);
		}

		std::shared_ptr<png_data> png_shader::get_image_data(const std::string& imageID) const
		{
			if (this->textureData.contains(imageID) == false)
			{
				std::cout << "[DEBUG] image " << imageID << " not in cache" << std::endl;
				return std::shared_ptr<png_data>();
			}

			return this->textureData.at(imageID);
		}
		std::shared_ptr<png_data> png_shader::add_image_data(const uint32_t openglTextureId, const std::string& imageID, uint32_t image_width, uint32_t image_height, uint8_t image_channels)
		{
			this->textureData[imageID] = std::shared_ptr<png_data>(new png_data(openglTextureId, image_width, image_height, image_channels));
			this->textureDataKeys.push_back(imageID);
			return this->textureData[imageID];
		}
		bool png_shader::check_if_png_in_cache(const std::string& imageID) const
		{
			return this->textureData.contains(imageID);
		}
		png_data::png_data(uint32_t opengl_texture_id, uint32_t image_width, uint32_t image_height, uint8_t image_channels) :
			openglTextureId(opengl_texture_id),
			imageWidth(image_width),
			imageHeight(image_height),
			imageChannels(image_channels)
		{
		}
		uint32_t png_data::get_opengl_texture_id(void) const
		{
			return this->openglTextureId;
		}
		uint32_t png_data::get_image_width(void) const
		{
			return this->imageWidth;
		}
		uint32_t png_data::get_image_height(void) const
		{
			return this->imageHeight;
		}
		uint32_t png_data::get_image_channels(void) const
		{
			return this->imageChannels;
		}
		png_data::~png_data()
		{
			if (this->openglTextureId)
				glDeleteTextures(1, &this->openglTextureId);
		}

	};
};

