//
//  imageUtility.h
//  lemonaire
//
//  Created by Wang Ke on 2017-06-24.
//  Copyright © 2017 Wang Ke. All rights reserved.
//

#ifndef imageUtility_h
#define imageUtility_h

#include <string>
#include <vector>

//#define GLEW_STATIC
#include <gl/glew.h>

#include "Resource.hpp"

class Texture : public Resource
{
public:
	Texture() {}
	Texture(GLuint id) :textureID{ id } {}
	Texture(const std::string& path, bool srgb = true, bool createGlObject = true, const GLenum filterType = GL_REPEAT)
	{
		loadImage("resource/" + path);
		if (createGlObject)
			createOpenGLTexture(filterType, srgb);
	}
	~Texture() {
	}

	void loadImage(const std::string& path);
	void freeImage();
	void createOpenGLTexture(const GLenum filterType, bool srgb);

	operator GLuint() const {
		return textureID;
	}

	int width, height, bpp;
	unsigned char *data;

	virtual void upload(GLuint shader, const std::string& nameInShader, const GLuint number) override
	{
		glUseProgram(shader);
		glActiveTexture(GL_TEXTURE0 + number);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(glGetUniformLocation(shader, nameInShader.c_str()), number);
	}

private:
	GLuint textureID;
};

class CubeMap : public Resource
{
public:
	CubeMap() {}
	~CubeMap() {}

	operator GLuint() const {
		return textureID;
	}

	void create()
	{
		// texture object
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		// filter
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void createEmpty(int width, int height)
	{
		create();
		for (int i{ 0 }; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			i++;
		}
	}

	void loadImages(std::vector<std::string> names);

	virtual void upload(GLuint shader, const std::string& nameInShader, const GLuint number) override
	{
		glUseProgram(shader);
		glActiveTexture(GL_TEXTURE0 + number);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		glUniform1i(glGetUniformLocation(shader, nameInShader.c_str()), number);
	}
private:
	GLuint textureID;
};
	
#endif /* imageUtility_h */






















