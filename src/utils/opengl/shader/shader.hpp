
#ifndef SHADER_H
#define SHADER_H

#include "headers.hpp"
#include <string>

class Shader {

	private:
		unsigned int shader;
		std::string location;
		GLenum shaderType;

	public:
		Shader(const char* location, GLenum shaderType);
		Shader(std::string const &location, GLenum shaderType);

		unsigned int getShader() const;
		GLenum getShaderType() const;
		const std::string toStringShaderType() const;
		const std::string getLocation() const;
};

#endif /* end of include guard: SHADER_H */
