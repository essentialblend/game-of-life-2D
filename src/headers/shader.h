#pragma once

#include "../includes/GLAD/glad.h"
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

class Shader
{
public:
	unsigned int ID = 0;

	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream vertShaderFile;
		std::ifstream fragShaderFile;

		vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vertShaderFile.open(vertexPath);
			fragShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vertShaderFile.rdbuf();
			fShaderStream << fragShaderFile.rdbuf();

			vertShaderFile.close();
			fragShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_READ: " << e.what() << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertexFinalID = 0;
		unsigned int fragmentFinalID = 0;

		vertexFinalID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexFinalID, 1,  &vShaderCode, NULL);
		glCompileShader(vertexFinalID);
		checkCompileErrors(vertexFinalID, "VERTEX");


		fragmentFinalID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentFinalID, 1, &fShaderCode, NULL);
		glCompileShader(fragmentFinalID);

		ID = glCreateProgram();
		glAttachShader(ID, vertexFinalID);
		glAttachShader(ID, fragmentFinalID);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		glDeleteShader(vertexFinalID);
		glDeleteShader(fragmentFinalID);
	};

	void UseShader()
	{
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	void checkCompileErrors(unsigned int program, std::string type)
	{
		int success = 0;
		char infoLog[1024];

		if (type != "PROGRAM")
		{
			glGetShaderiv(program, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(program, 1024, NULL, infoLog);
				std::cout << "SHADERCOMPILE::ERROR::SHADER_COMPILATION_ERR\n";
				std::cout << "SHADERCOMPILE::TYPE: " << type << "\n" << infoLog << "\n----";
			}
		}
		else
		{
			glGetProgramiv(program, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(program, 1024, NULL, infoLog);
				std::cout << "PROGRAMCOMPILE::ERROR::SHADER_COMPILATION_ERR\n";
				std::cout << "PROGRAMCOMPILE::TYPE: " << type << "\n" << infoLog << "\n----";
			}
		}
	}
};