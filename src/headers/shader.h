#pragma once

#include "../includes/GLAD/glad/glad.h"
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

struct uniformInfo
{
	GLint uniformLocation;
	GLsizei uniformValue;
};

class Shader
{
public:
	unsigned int ID = 0;

	// Constructor for vertex and fragment shaders.
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

	// Constructor for compute shaders.
	Shader(const char* computePath)
	{
		std::string computeShaderCode;
		std::ifstream computeShaderFile;

		computeShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);		
		try
		{
			computeShaderFile.open(computePath);
			std::stringstream computeShaderStream;
			computeShaderStream << computeShaderFile.rdbuf();
			computeShaderFile.close();
			computeShaderCode = computeShaderStream.str();
		}
		catch (std::ifstream::failure& exc)
		{
			std::cout << "ERROR: Shader file read failed!" << exc.what() << std::endl;
		}

		const char* computeShaderCodeText = computeShaderCode.c_str();
		unsigned int computeShaderFinalID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShaderFinalID, 1, &computeShaderCodeText, NULL);
		glCompileShader(computeShaderFinalID);
		checkCompileErrors(computeShaderFinalID, "COMPUTE");

		ID = glCreateProgram();
		glAttachShader(ID, computeShaderFinalID);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		glDeleteShader(computeShaderFinalID);
	}


	void retrieveUniforms()
	{
		GLint uniformCount = 0;
		glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniformCount);

		if (uniformCount != 0)
		{
			GLint maxNameLen = 0;
			GLsizei length = 0;
			GLsizei size = 0;
			GLenum type = GL_NONE;
			
			glGetProgramiv(ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);

			auto uniformName = std::make_unique<char[]>(maxNameLen);

			for (GLint i = 0; i < uniformCount; ++i)
			{
				glGetActiveUniform(ID, i, maxNameLen, &length, &size, &type, uniformName.get());

				uniformInfo uniformInfoInstance = {};
				uniformInfoInstance.uniformLocation = glGetUniformLocation(ID, uniformName.get());
				uniformInfoInstance.uniformValue = size;

				uniformsList.emplace(std::make_pair(std::string(uniformName.get(), length), uniformInfoInstance));


			}
		}
	}

	void UseShader()
	{
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform1i(it->second.uniformLocation, value);
		}
	}
	void setInt(const std::string& name, int value) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform1i(it->second.uniformLocation, value);
		}
	}
	void setFloat(const std::string& name, float value) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform1f(it->second.uniformLocation, value);
		}
	}
	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform2fv(it->second.uniformLocation, 1, &value[0]);
		}
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform2f(it->second.uniformLocation, x, y);
		}
	}
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform3fv(it->second.uniformLocation, 1, &value[0]);
		}
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform3f(it->second.uniformLocation, x, y, z);
		}
	}
	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform4fv(it->second.uniformLocation, 1, &value[0]);
		}
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniform4f(it->second.uniformLocation, x, y, z, w);
		}
	}
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniformMatrix2fv(it->second.uniformLocation, 1, GL_FALSE, &mat[0][0]);
		}
	}
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniformMatrix3fv(it->second.uniformLocation, 1, GL_FALSE, &mat[0][0]);
		}
	}
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		auto it = uniformsList.find(name);
		if (it != uniformsList.end())
		{
			glUniformMatrix4fv(it->second.uniformLocation, 1, GL_FALSE, &mat[0][0]);
		}
	}

private:
	std::unordered_map<std::string, uniformInfo> uniformsList;

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