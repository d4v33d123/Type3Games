#include "GLSLProgram.h"

namespace Type3Engine
{
	GLSLProgram::GLSLProgram() : _numAttributes(0), _programID(0), _vertexShaderID(0), _pixelShaderID(0)
	{

	}

	GLSLProgram::~GLSLProgram()
	{
	}

	void GLSLProgram::compileShaders(const std::string& vertexShaderFilePath, const std::string& pixelShaderFilePath)
	{
		_programID = glCreateProgram();

		_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		if (_vertexShaderID == 0)
		{
			fatalError("Vertex Shader failed to be created!");
		}

		_pixelShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (_pixelShaderID == 0)
		{
			fatalError("Pixel Shader failed to be created!");
		}

		CompileShader(vertexShaderFilePath, _vertexShaderID);
		CompileShader(pixelShaderFilePath, _pixelShaderID);
	}

	void GLSLProgram::linkShaders()
	{
		// attach our shaders to our program

		glAttachShader(_programID, _vertexShaderID);
		glAttachShader(_programID, _pixelShaderID);

		//link our program
		glLinkProgram(_programID);

		GLuint isLinked = 0;

		glGetProgramiv(_programID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &maxLength);
			//the maxLength includes the NULL character
			std::vector<char> errorLog(maxLength);
			glGetProgramInfoLog(_programID, maxLength, &maxLength, &errorLog[0]);

			//exit with failure
			glDeleteProgram(_programID); //dont leak the program
			glDeleteShader(_vertexShaderID); // or the vertex shader
			glDeleteShader(_pixelShaderID); // or the pixel shader

			SDL_Log("%s\n", &(errorLog[0]));
			fatalError("Program failed to link shaders!");

		}

		// always detatch shaders after a successful link
		glDetachShader(_programID, _vertexShaderID);
		glDetachShader(_programID, _pixelShaderID);
		glDeleteShader(_vertexShaderID); // returning resources to opengl
		glDeleteShader(_pixelShaderID);
	}

	void GLSLProgram::addAttribute(const std::string& attribute)
	{
		// adds our attribute and increments our number of attributes variable
		glBindAttribLocation(_programID, _numAttributes++, attribute.c_str());
	}

	void GLSLProgram::CompileShader(const std::string& filePath, GLuint id)
	{
		//read glsl source file
		std::vector<char> glslSource;		
		IOManager::readTextToBuffer(filePath, glslSource);
				
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
		glUseProgram(_programID);
		for (int i = 0; i < _numAttributes; i++)
		{
			glEnableVertexAttribArray(i);
		}
	}

	void GLSLProgram::stopUse()
	{
		// stop using our program
		glUseProgram(0);
		for (int i = 0; i < _numAttributes; i++)
		{
			glDisableVertexAttribArray(i);
		}
	}

	GLint GLSLProgram::getUniformLocation(const std::string& uniformName)
	{
		GLint location = glGetUniformLocation(_programID, uniformName.c_str());
		//if (location == GL_INVALID_INDEX)
		if (location == 0xFFFFFFFF)//GL_INVALID_INDEX is not defined in GLES
		{
			fatalError("Uniform " + uniformName + " not found in shader! (or it is unused)");
		}
		return location;
	}
}