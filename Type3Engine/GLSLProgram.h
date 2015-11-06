#pragma once

#include <string>
#include <GL/glew.h>

namespace Type3Engine
{

	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();

		void compileShaders(const std::string& vertexShader, const std::string& pixelShader);

		void addAttribute(const std::string& attributeName);

		void linkShaders();

		GLint getUniformLocation(const std::string& uniformName);

		void use();
		void stopUse();
	private:

		void CompileShader(const std::string& filePath, GLuint id);

		int _numAttributes;
		GLuint _programID;
		GLuint _vertexShaderID;
		GLuint _pixelShaderID;
	};

}