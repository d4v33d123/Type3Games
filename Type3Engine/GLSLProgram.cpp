#include "GLSLProgram.h"
#include "errors.h"
#include <fstream>
#include <vector>
namespace T3E
{

	GLSLProgram::GLSLProgram() : numAttributes_(0), programID_(0), vertexShaderID_(0), pixelShaderID_(0)
	{

	}


	GLSLProgram::~GLSLProgram()
	{
	}


	void GLSLProgram::compileShaders(const std::string& vertexShaderFilePath, const std::string& pixelShaderFilePath)
	{
		// create our program ID
		programID_ = glCreateProgram();

		vertexShaderID_ = glCreateShader(GL_VERTEX_SHADER);
		if (vertexShaderID_ == 0)
		{
			fatalError("Vertex Shader failed to be created!");
		}

		pixelShaderID_ = glCreateShader(GL_FRAGMENT_SHADER);
		if (pixelShaderID_ == 0)
		{
			fatalError("Pixel Shader failed to be created!");
		}

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

			std::printf("%s\n", &(errorLog[0]));
			fatalError("Program failed to link shaders!");

		}

		// always detatch shaders after a successful link
		glDetachShader(programID_, vertexShaderID_);
		glDetachShader(programID_, pixelShaderID_);
		glDeleteShader(vertexShaderID_); // returning resources to opengl
		glDeleteShader(pixelShaderID_);


	}

	void GLSLProgram::addAttribute(const std::string& attribute)
	{
		// adds our attribute and increments our number of attributes variable
		glBindAttribLocation(programID_, numAttributes_++, attribute.c_str());
	}

	void GLSLProgram::CompileShader(const std::string& filePath, GLuint id)
	{


		std::ifstream file(filePath);
		if (file.fail())
		{
			perror(filePath.c_str());
			fatalError("Failed to open " + filePath);
		}

		std::string fileContents = "";
		std::string line;

		while (std::getline(file, line))
		{
			fileContents += line + "\n";
		}

		file.close();

		const char* contentsPtr = fileContents.c_str();
		glShaderSource(id, 1, &contentsPtr, nullptr);

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

			std::printf("%s\n", &(errorLog[0]));
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
		if (location == GL_INVALID_INDEX)
		{
			fatalError("Uniform " + uniformName + " not found in shader!");
		}
		return location;
	}
}