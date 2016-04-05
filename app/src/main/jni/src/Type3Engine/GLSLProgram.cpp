#include "GLSLProgram.h"

namespace T3E
{
	GLSLProgram::GLSLProgram() : numAttributes_(0), programID_(0), vertexShaderID_(0), pixelShaderID_(0)
	{
	}

	GLSLProgram::~GLSLProgram()
	{
		destroy();
	}

	void GLSLProgram::destroy()
	{
		if( programID_ != 0 ) {
	    	glDeleteProgram( programID_ );
	    	programID_ = 0;
		}
		if( pixelShaderID_ != 0 ) {
	    	glDeleteShader( pixelShaderID_ );
	    	pixelShaderID_ = 0;
		}
		if( vertexShaderID_ != 0 ) {
	    	glDeleteShader( vertexShaderID_ );
	    	vertexShaderID_ = 0;
		}
	}

	// compile the shaders
	void GLSLProgram::compileShaders(const std::string& vertexShaderFilePath, const std::string& pixelShaderFilePath)
	{
		// create a new gl program
		programID_ = glCreateProgram();

		// get the vertex shader and check to see if it has been created
		vertexShaderID_ = glCreateShader(GL_VERTEX_SHADER);
		if (vertexShaderID_ == 0)
		{
			fatalError("Vertex Shader failed to be created!");
		}
		
		// get the pixel shader and check to see if it has been created
		pixelShaderID_ = glCreateShader(GL_FRAGMENT_SHADER);
		if (pixelShaderID_ == 0)
		{
			fatalError("Pixel Shader failed to be created!");
		}

		// compile the vertex and pixel shaders
		CompileShader(vertexShaderFilePath, vertexShaderID_);
		CompileShader(pixelShaderFilePath, pixelShaderID_);
	}

	void GLSLProgram::linkShaders()
	{
		// attach our shaders to our program

		glAttachShader(programID_, vertexShaderID_);
		glAttachShader(programID_, pixelShaderID_);

		//link our program
		glLinkProgram(programID_);

		GLuint isLinked = 0;

		// check to see if the program is linked, if it is not then safely exit 
		glGetProgramiv(programID_, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(programID_, GL_INFO_LOG_LENGTH, &maxLength);
			//the maxLength includes the NULL character
			std::vector<char> errorLog(maxLength);
			glGetProgramInfoLog(programID_, maxLength, &maxLength, &errorLog[0]);

			//exit with failure
			glDeleteProgram(programID_); //dont leak the program
			glDeleteShader(vertexShaderID_); // or the vertex shader
			glDeleteShader(pixelShaderID_); // or the pixel shader

			SDL_Log("%s\n", &(errorLog[0]));
			fatalError("Program failed to link shaders!");
		}

		// always detatch shaders after a successful link
		glDetachShader(programID_, vertexShaderID_);
		glDetachShader(programID_, pixelShaderID_);
		glDeleteShader(vertexShaderID_); // returning resources to opengl
		glDeleteShader(pixelShaderID_);
		vertexShaderID_ = 0;
		pixelShaderID_ = 0;
	}

	void GLSLProgram::addAttribute(const std::string& attribute)
	{
		// adds our attribute and increments our number of attributes variable
		glBindAttribLocation(programID_, numAttributes_++, attribute.c_str());
	}

	void GLSLProgram::CompileShader(const std::string& filePath, GLuint id)
	{
		//read glsl source file
		std::vector<char> glslSource;		
		if(!IOManager::readTextToBuffer(filePath, glslSource))
			fatalError("Failed to open " + filePath);
				
		//load source in the shader
		const char* sourcePtr = &glslSource[0];
		glShaderSource(id, 1, &sourcePtr, NULL);
		
		//compile shader
		glCompileShader(id);

		GLint success = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

			//the maxLength includes the NULL character
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

			//exit with failure
			glDeleteShader(id); //dont leak the shader

			SDL_Log("%s\n", &(errorLog[0]));
			fatalError("Shader " + filePath + " failed to compile");
		}
	}

	void GLSLProgram::use()
	{
		//use our program
		glUseProgram(programID_);
		for (int i = 0; i < numAttributes_; i++)
		{
			glEnableVertexAttribArray(i);
		}
	}

	void GLSLProgram::stopUse()
	{
		// stop using our program
		glUseProgram(0);
		for (int i = 0; i < numAttributes_; i++)
		{
			glDisableVertexAttribArray(i);
		}
	}

	GLint GLSLProgram::getUniformLocation(const std::string& uniformName)
	{
		GLint location = glGetUniformLocation(programID_, uniformName.c_str());
		//if (location == GL_INVALID_INDEX)
		if (location == 0xFFFFFFFF) // GL_INVALID_INDEX is not defined in GLES
		{
			fatalError("Uniform " + uniformName + " not found in shader! (or it is unused)");
		}
		return location;
	}
}