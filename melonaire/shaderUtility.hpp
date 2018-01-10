#ifndef shaderUtility_hpp
#define shaderUtility_hpp

#include <SDL.h>

//#define GL_STATIC
#include <gl/glew.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Resource.hpp"

class Shader
{
public:
	Shader() {}
	Shader(unsigned int program) :program{ program } {}
	Shader(const std::string& shaderName)
	{
		loadShader("shader/" + shaderName + ".vert", "shader/" + shaderName + ".frag");
	}
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) 
	{
		loadShader("shader/" + vertexShaderPath, "shader/" + fragmentShaderPath);
	}
	~Shader() {}

	GLuint loadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void cleanUp() { glDeleteProgram(program); }
	void use() const { glUseProgram(program); }

	operator GLuint() const { return program; }

	void uploadUniform(const vec3& v, const std::string& nameInShader) const
	{
		use();
		glUniform3f(glGetUniformLocation(program, nameInShader.c_str()), v.x, v.y, v.z);
	}

	void uploadUniform(const vec4& v, const std::string& nameInShader) const
	{
		use();
		glUniform4f(glGetUniformLocation(program, nameInShader.c_str()), v.x, v.y, v.z, v.w);
	}

	void uploadUniform(const mat3& m, const std::string& nameInShader) const
	{
		use();
		glUniformMatrix3fv(glGetUniformLocation(program, nameInShader.c_str()), 1, GL_FALSE, &m[0][0]);
	}

	void uploadUniform(const mat4& m, const std::string& nameInShader) const
	{
		use();
		glUniformMatrix4fv(glGetUniformLocation(program, nameInShader.c_str()), 1, GL_FALSE, &m[0][0]);
	}

	void uploadUniform(float n, const std::string& nameInShader) const
	{
		use();
		glUniform1f(glGetUniformLocation(program, nameInShader.c_str()), n);
	}

private:
	GLuint program;

	const std::string readFile(const std::string& filePath);
	void compileShader(GLuint& id, const GLenum& type, const std::string& filePath);
};

// readFile: from filename to char* buffer
inline const std::string Shader::readFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.good())
	{
		std::cout << "Failed to open file: " << filePath << std::endl;
		exit(-1);
	}
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();
	return  stream.str();
}

// compileShader: do everything about a shader
inline void Shader::compileShader(GLuint& id, const GLenum& type, const std::string& filePath)
{
	// create, load, and complie
	id = glCreateShader(type);
	const std::string codeString = readFile(filePath);
	const GLchar* codeChar = codeString.c_str();
	glShaderSource(id, 1, &codeChar, NULL);
	glCompileShader(id);

	// get type string
	std::string typeString;
	if (type == GL_VERTEX_SHADER)
		typeString = "Vertex";
	else if (type == GL_FRAGMENT_SHADER)
		typeString = "Fragment";

	// Check the compilation status and report any errors
	GLint shaderStatus;
	glGetShaderiv(id, GL_COMPILE_STATUS, &shaderStatus);

	// If the shader failed to compile, display the info log and quit out
	if (shaderStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog);

		std::cout << typeString << " shader compilation failed: " << std::endl << strInfoLog << std::endl;
		delete[] strInfoLog;
		SDL_Quit();
	}
	else
	{
		std::cout << typeString << " shader compilation OK" << std::endl;
	}
}

inline GLuint Shader::loadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	GLuint vertexShaderID, fragmentShaderID;
	compileShader(vertexShaderID, GL_VERTEX_SHADER, vertexShaderPath);
	compileShader(fragmentShaderID, GL_FRAGMENT_SHADER, fragmentShaderPath);

	program = glCreateProgram();
	glAttachShader(program, vertexShaderID);
	glAttachShader(program, fragmentShaderID);
	glLinkProgram(program);

	// Check the status
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);

		std::cout << "Shader program linking failed:" << std::endl << strInfoLog << std::endl;
		delete[] strInfoLog;
		SDL_Quit();
	}
	else
	{
		std::cout << "Shader program linking OK." << std::endl;
	}

	glUseProgram(program);
	return program;
}


#endif // ! shaderUtility_hpps