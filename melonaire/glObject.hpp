#ifndef glObject_hpp
#define glObject_hpp

#include <iostream>
#include <string>
 
//#define GLEW_STATIC
#include <gl/glew.h>

#include "shaderUtility.hpp"
#include "imageUtility.h"

#include "utilityFunc.h"

class OpenGLObject
{
public:
	OpenGLObject(){}
	OpenGLObject(const Shader& _shader)
		:shader{ _shader }
	{}
	~OpenGLObject(){}

	void setShader(const Shader& _shader) {
		shader = _shader;
	}

	Shader getShader() const {
		return shader;
	}

	void uploadUniform(const vec3& v, const std::string& nameInShader) const
	{
		shader.use();
		glUniform3f(glGetUniformLocation(shader, nameInShader.c_str()), v.x, v.y, v.z);
	}

	void uploadUniform(const vec4& v, const std::string& nameInShader) const
	{
		shader.use();
		glUniform4f(glGetUniformLocation(shader, nameInShader.c_str()), v.x, v.y, v.z, v.w);
	}

	void uploadUniform(const mat3& m, const std::string& nameInShader) const
	{
		shader.use();
		glUniformMatrix3fv(glGetUniformLocation(shader, nameInShader.c_str()), 1, GL_FALSE, &m[0][0]);
	}

	void uploadUniform(const mat4& m, const std::string& nameInShader) const
	{
		shader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader, nameInShader.c_str()), 1, GL_FALSE, &m[0][0]);
	}

	void uploadUniform(float n, const std::string& nameInShader) const
	{
		shader.use();
		glUniform1f(glGetUniformLocation(shader, nameInShader.c_str()), n);
	}

	void uploadTexture(const Texture& tex, const std::string& nameInShader, const GLuint number) const
	{
		shader.use();
		glActiveTexture(GL_TEXTURE0 + number);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(glGetUniformLocation(shader, nameInShader.c_str()), number);
	}

	void uploadTexture(const CubeMap& tex, const std::string& nameInShader, const GLuint number) const
	{
		shader.use();
		glActiveTexture(GL_TEXTURE0 + number);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
		glUniform1i(glGetUniformLocation(shader, nameInShader.c_str()), number);
	}

protected:
	Shader shader;
};

#endif // !glObject_hpp

