#pragma once

#include <string>
#include <GL/glew.h>

namespace T3E
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

		int numAttributes_;
		GLuint programID_;
		GLuint vertexShaderID_;
		GLuint pixelShaderID_;
	};

}