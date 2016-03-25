#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
#include "errors.h"
#include "IOManager.h"
#include <vector>
#include <string>

namespace T3E
{

	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();
		void destroy();

		void compileShaders(const std::string& vertexShader, const std::string& pixelShader);

		void addAttribute(const std::string& attributeName);

		void linkShaders();

		GLint getUniformLocation(const std::string& uniformName);

		void use();
		void stopUse();

		GLuint getProgramID(){ return programID_; };
		
	private:
		void CompileShader(const std::string& filePath, GLuint id);

		int numAttributes_;
		GLuint programID_;
		GLuint vertexShaderID_;
		GLuint pixelShaderID_;
	};

}

#endif