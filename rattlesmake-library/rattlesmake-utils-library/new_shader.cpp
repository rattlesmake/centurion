#include "new_shader.h"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

namespace rattlesmake
{
	namespace utils
	{
		opengl_shader::opengl_shader()
		{
		}
		opengl_shader::~opengl_shader()
		{
		}
		const unsigned int opengl_shader::get_shader_id(void) const
		{
			return this->glData.shaderId;
		}
		const unsigned int opengl_shader::get_shader_VAO(void) const
		{
			return this->glData.VAO;
		}
		const unsigned int opengl_shader::get_shader_IBO(void) const
		{
			return this->glData.IBO;
		}
		const unsigned int opengl_shader::get_shader_VBO(void) const
		{
			return this->glData.VBO;
		}
		void opengl_shader::apply_matrices(const glm::mat4& projection, const glm::mat4& view)
		{
			this->apply_projection_matrix(projection);
			this->apply_view_matrix(view);
		}
		void opengl_shader::apply_projection_matrix(glm::mat4 mat)
		{
			glUseProgram(this->glData.shaderId);
			glUniformMatrix4fv(glGetUniformLocation(this->glData.shaderId, "uProjection"), 1, GL_FALSE, glm::value_ptr(mat));
			glUseProgram(0);
		}
		void opengl_shader::apply_view_matrix(glm::mat4 mat)
		{
			glUseProgram(this->glData.shaderId);
			glUniformMatrix4fv(glGetUniformLocation(this->glData.shaderId, "uView"), 1, GL_FALSE, glm::value_ptr(mat));
			glUseProgram(0);
		}

		#pragma region [OPENGL] Compile shaders and prepare program
		void opengl_shader::check_compiler_success(const int shader, const char* shaderName)
		{
			int result = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
			if (!result)
			{
				char infoLog[512];
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER " << shaderName << " COMPILATION_FAILED" << infoLog << std::endl;
			}
		}
		int opengl_shader::compile(const char* code, opengl_shaderType shaderType)
		{
			int shader = 0;

			switch (shaderType)
			{
			case rattlesmake::utils::opengl_shader::opengl_shaderType::fragment:
				shader = glCreateShader(GL_FRAGMENT_SHADER);
				break;
			case rattlesmake::utils::opengl_shader::opengl_shaderType::vertex:
				shader = glCreateShader(GL_VERTEX_SHADER);
				break;
			case rattlesmake::utils::opengl_shader::opengl_shaderType::tessellation_control:
				shader = glCreateShader(GL_TESS_CONTROL_SHADER);
				break;
			case rattlesmake::utils::opengl_shader::opengl_shaderType::tessellation_evaluation:
				shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
				break;
			case rattlesmake::utils::opengl_shader::opengl_shaderType::geometry:
				shader = glCreateShader(GL_GEOMETRY_SHADER);
				break;
			default:
				break;
			}
			
			glShaderSource(shader, 1, &code, NULL);
			glCompileShader(shader);

			switch (shaderType)
			{
			case rattlesmake::utils::opengl_shader::opengl_shaderType::fragment:
				this->check_compiler_success(shader, "fragment");
				break;
			case rattlesmake::utils::opengl_shader::opengl_shaderType::vertex:
				this->check_compiler_success(shader, "vertex");
				break;
			case rattlesmake::utils::opengl_shader::opengl_shaderType::tessellation_control:
				this->check_compiler_success(shader, "tessellation_control");
				break;
			case rattlesmake::utils::opengl_shader::opengl_shaderType::tessellation_evaluation:
				this->check_compiler_success(shader, "tessellation_evaluation");
				break;
			case rattlesmake::utils::opengl_shader::opengl_shaderType::geometry:
				this->check_compiler_success(shader, "geometry");
				break;
			default:
				break;
			}
			
			return shader;
		}

		uint32_t opengl_shader::create_program(const int vertexShader, const int fragmentShader, const int tessControlShader, const int tessEvalShader, const int geomShader)
		{
			int result = 0;
			uint32_t shaderID = 0;
			shaderID = glCreateProgram();
			glAttachShader(shaderID, vertexShader);
			glAttachShader(shaderID, fragmentShader);
			tessControlShader != -1 ? glAttachShader(shaderID, tessControlShader) : void(0);
			tessEvalShader != -1 ? glAttachShader(shaderID, tessEvalShader) : void(0);
			geomShader != -1 ? glAttachShader(shaderID, geomShader) : void(0);
			glLinkProgram(shaderID);

			// check for linking errors
			glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
			if (!result) {
				char infoLog[512];
				glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << infoLog;
			}

			return shaderID;
		}
		glm::vec3 opengl_shader::convert_picking_id_to_vec3(const uint32_t pickingID)
		{
			const uint32_t r = (pickingID & 0x000000FF) >> 0;
			const uint32_t g = (pickingID & 0x0000FF00) >> 8;
			const uint32_t b = (pickingID & 0x00FF0000) >> 16;
			return glm::vec3(r, g, b);
		}
		void opengl_shader::delete_shaders(const int vertexShader, const int fragmentShader, const int tessControlShader, const int tessEvalShader, const int geomShader)
		{
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			tessControlShader != -1 ? glDeleteShader(tessControlShader) : void(0);
			tessEvalShader != -1 ? glDeleteShader(tessEvalShader) : void(0);
		}
		void opengl_shader::generate_buffers(void)
		{
		}
		std::string opengl_shader::get_vertex_shader(void)
		{
			return std::string();
		}
		std::string opengl_shader::get_fragment_shader(void)
		{
			return std::string();
		}
		void opengl_shader::compile_shaders(const char* vShaderCode, const char* fShaderCode, const char* tcShaderCode, const char* teShaderCode, const char* gShaderCode)
		{
			// build and compile our shader program
			int vertexShader = this->compile(vShaderCode, opengl_shaderType::vertex);
			int fragmentShader = this->compile(fShaderCode, opengl_shaderType::fragment);
			int tessControlShader = tcShaderCode != nullptr ? this->compile(tcShaderCode, opengl_shaderType::tessellation_control) : -1;
			int tessEvalShader = teShaderCode != nullptr ? this->compile(teShaderCode, opengl_shaderType::tessellation_evaluation) : -1;
			int geomShader = gShaderCode != nullptr ? this->compile(gShaderCode, opengl_shaderType::geometry) : -1;

			// link shaders
			this->glData.shaderId = this->create_program(vertexShader, fragmentShader, tessControlShader, tessEvalShader, geomShader);

			// delete shaders
			this->delete_shaders(vertexShader, fragmentShader, tessControlShader, tessEvalShader, geomShader);
		}



		#pragma endregion
	};
};
