#include "shader.h"

/*
 * File Name: shader.cpp
 *
 * This file contains methods which help load in shaders
 * and set them up so the program is linked to them properly.
 *
 * @author Part of 169 starter code
 */
enum ShaderType { vertex, fragment };

/*
 * Creates a single shader ID given a file path and type. 
 *
 * @param shaderFilePath The file path to the shader
 * @param type Specifies the type of shader (vertex, fragment, etc.)
 * @return Returns the new shader ID
 * @author Part of 169 starter code
 */
GLuint LoadSingleShader(const char * shaderFilePath, ShaderType type) 
{
	// Create a shader id.
	GLuint shaderID = 0;
	if (type == vertex) 
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	else if (type == fragment) 
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Try to read shader codes from the shader file.
	std::string shaderCode;
	std::ifstream shaderStream(shaderFilePath, std::ios::in);
	if (shaderStream.is_open()) 
	{
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else 
	{
		std::cerr << "Impossible to open " << shaderFilePath << ". "
			<< "Check to make sure the file exists and you passed in the "
			<< "right filepath!"
			<< std::endl;
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader.
	std::cerr << "Compiling shader: " << shaderFilePath << std::endl;
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);
	
	// Check Shader.
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage.data());
		std::string msg(shaderErrorMessage.begin(), shaderErrorMessage.end());
		std::cerr << msg << std::endl;
		return 0;
	}
	else 
	{
		if (type == vertex)
			printf("Successfully compiled vertex shader!\n");
		else if (type == fragment) 
			printf("Successfully compiled fragment shader!\n");
	}

	return shaderID;
}

/*
 * Loads in vertex and fragment shaders for the program.
 *
 * @param vertexFilePath The file path to the vertex shader
 * @param fragmentFilePath The file path to the fragment shader
 * @return Returns the program ID associated with the shaders, will be
 * used to execute program in Window.cpp
 * @author Part of 169 starter code
 */
GLuint LoadShaders(const char * vertexFilePath, const char * fragmentFilePath) 
{
	// Create the vertex shader and fragment shader.
	GLuint vertexShaderID = LoadSingleShader(vertexFilePath, vertex);
	GLuint fragmentShaderID = LoadSingleShader(fragmentFilePath, fragment);

	// Check both shaders.
	if (vertexShaderID == 0 || fragmentShaderID == 0) return 0;

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Link the program.
	printf("Linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check the program.
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage.data());
		std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
		std::cerr << msg << std::endl;
		glDeleteProgram(programID);
		return 0;
	}
	else
	{
		printf("Successfully linked program!\n");
	}

	// Detach and delete the shaders as they are no longer needed.
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}
