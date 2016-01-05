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

		GLuint getProgramID(){ return _programID; };
		
	private:
		void CompileShader(const std::string& filePath, GLuint id);

		int _numAttributes;
		GLuint _programID;
		GLuint _vertexShaderID;
		GLuint _pixelShaderID;
	};

}

#endif