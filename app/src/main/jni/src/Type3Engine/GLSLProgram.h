#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif //__ANDROID__
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
		
		/**
		* Shuts down the GLSL program safely
		*/
		void destroy();
		
		/**
		* Compiles the shaders
		*
		* @param [in] The vertex chader string
		* @param [out] The pixel shader string
		*/
		void compileShaders(const std::string& vertexShader, const std::string& pixelShader);
		
		/**
		* Adds an attribute to the program
		*
		* @param [in] The string containing the name of the added attribute
		*/
		void addAttribute(const std::string& attributeName);
		
		/**
		* Links the shaders
		*/
		void linkShaders();

		/**
		* 
		*/
		GLint getUniformLocation(const std::string& uniformName);

		/**
		* Begins the use of the GLSL program
		*/
		void use();
		
		/**
		* Stops the use of the GLSL program
		*/
		void stopUse();

		/**
		* Gets the ID of the GLSL program
		*
		* @return [out] the value of the program ID
		*/
		GLuint getProgramID(){ return programID_; };
		
	private:
	
		/**
		* Compiles the shader for the program 
		*
		* @param [in] The string containing the file path of the shader
		* @param [in] The id of the program 
		*/
		void CompileShader(const std::string& filePath, GLuint id);

		int numAttributes_;
		GLuint programID_;
		GLuint vertexShaderID_;
		GLuint pixelShaderID_;
	};

}

#endif