#include "opengl/shader.hpp"
#include "utils.hpp"

#include <third_party/Eigen/Dense>
#include <third_party/glad/glad.h>

#include <SDL3/SDL.h>
#include <cassert>

Shader::Shader(const std::string& vertName, const std::string& fragName) {
	compile(vertName, GL_VERTEX_SHADER, mVertexShader);
	compile(fragName, GL_FRAGMENT_SHADER, mFragmentShader);

	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragmentShader);
	glLinkProgram(mShaderProgram);

	int success;
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &success);
	[[unlikely]] if (!success) {
		int len = 0;
		glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &len);
		std::string log;
		log.resize(len);

		glGetProgramInfoLog(mShaderProgram, 512, nullptr, &log[0]);
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to link shader: %s\n", log.c_str());
		ERROR_BOX("Failed to link shader");

		throw 1;
	}
}

Shader::~Shader() {
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragmentShader);
	glDeleteProgram(mShaderProgram);
}

void Shader::activate() const { glUseProgram(mShaderProgram); }

int Shader::getUniform(const std::string& name) const {
	int location = glGetUniformLocation(mShaderProgram, name.c_str());

	[[unlikely]] if (location == -1) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed find uniform location: %s\n", name.c_str());
		ERROR_BOX("Failed to find uniform location");
		throw 1;
	}

	return location;
}

void Shader::set(const std::string& name, const bool& val) const {
	glUniform1i(getUniform(name), static_cast<int>(val));
}

void Shader::set(const std::string& name, const int& val) const {
	glUniform1i(getUniform(name), val);
}

void Shader::set(const std::string& name, const float& val) const {
	glUniform1f(getUniform(name), val);
}

void Shader::set(const std::string& name, const float& val, const float& val2) const {
	glUniform2f(getUniform(name), val, val2);
}

void Shader::set(const std::string& name, const float& val, const float& val2,
				 const float& val3) const {
	glUniform3f(getUniform(name), val, val2, val3);
}

void Shader::set(const std::string& name, const Eigen::Vector3f& val) const {
	glUniform3f(getUniform(name), val.x(), val.y(), val.z());
}

void Shader::set(const std::string& name, const Eigen::Affine3f& mat,
				 const GLboolean& transpose) const {
	glUniformMatrix4fv(getUniform(name), 1, transpose, mat.data());
}

void Shader::compile(const std::string& fileName, const GLenum& type, unsigned int& out) {
	char* shaderSource = static_cast<char*>(SDL_LoadFile(fileName.c_str(), nullptr));
#ifdef GLES
	// #version 400 core
	// to
	// #version 300 es
	shaderSource[9] = '3';
	shaderSource[13] = 'e';
	shaderSource[14] = 's';
	shaderSource[15] = ' ';
	shaderSource[16] = ' ';
#endif

	[[unlikely]] if (shaderSource == nullptr) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to read shader shource: %s\n",
						fileName.c_str());
		ERROR_BOX("Failed to read assets");

		throw 1;
	}
	SDL_Log("Loading %s", fileName.c_str());

	out = glCreateShader(type);
	glShaderSource(out, 1, &shaderSource, nullptr);
	glCompileShader(out);
	SDL_free(shaderSource);

	// Error checking
	int success;
	glGetShaderiv(out, GL_COMPILE_STATUS, &success);
	[[unlikely]] if (!success) {
		int len = 0;
		glGetShaderiv(out, GL_INFO_LOG_LENGTH, &len);
		std::string log;
		log.resize(len);

		glGetShaderInfoLog(out, len, nullptr, &log[0]);
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to compile shader %s: %s\n",
						fileName.c_str(), log.c_str());
		ERROR_BOX("Failed to compile vertex shader");

		throw 1;
	}
}