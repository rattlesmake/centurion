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
#include <memory>

#include <GL/glew.h>
#include <glm.hpp>

namespace rattlesmake
{
	namespace utils
	{
		class opengl_shader
		{
			// this class is not instantiable (only interface)

		public:
			const unsigned int get_shader_id(void) const;
			const unsigned int get_shader_VAO(void) const;
			const unsigned int get_shader_IBO(void) const;
			const unsigned int get_shader_VBO(void) const;
			void apply_matrices(const glm::mat4& projection, const glm::mat4& view);

		protected:
			opengl_shader();
			~opengl_shader();

			#pragma region [OPENGL] Compile shaders and prepare program
			enum class opengl_shaderType
			{
				fragment = 0,
				vertex = 1,
				tessellation_control= 2,
				tessellation_evaluation = 3,
				geometry = 4
			};
			void compile_shaders(const char* vShaderCode, const char* fShaderCode, const char* tcShaderCode = nullptr, const char* teShaderCode = nullptr, const char* gShaderCode = nullptr);
			int compile(const char* code, opengl_shaderType shaderType);
			void check_compiler_success(const int shader, const char* shaderName);
			uint32_t create_program(const int vertexShader, const int fragmentShader, const int tessControlShader = -1, const int tessEvalShader = -1, const int geomShader = -1);
			void delete_shaders(const int vertexShader, const int fragmentShader, const int tessControlShader = -1, const int tessEvalShader = -1, const int geomShader = -1);
			#pragma endregion

			struct glData
			{
				GLuint shaderId = 0, VAO = 0, VBO = 0, IBO = 0;
			} glData;

			virtual void generate_buffers(void);
			virtual std::string get_vertex_shader(void);
			virtual std::string get_fragment_shader(void);

			[[nodiscard]] glm::vec3 convert_picking_id_to_vec3(const uint32_t pickingID);

			void apply_projection_matrix(glm::mat4 mat = glm::mat4(1.f));
			void apply_view_matrix(glm::mat4 mat = glm::mat4(1.f));
		};
	};
};
