//
//  imageUtility.cpp
//  lemonaire
//
//  Created by Wang Ke on 2017-06-24.
//  Copyright © 2017 Wang Ke. All rights reserved.
//

#include "imageUtility.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture::loadImage(const std::string& path)
{
	 stbi_set_flip_vertically_on_load(true);
	 data = stbi_load(path.c_str(), &width, &height, &bpp, 0);
}

void Texture::freeImage()
{
	if(data)
		stbi_image_free(data);

	//glDeleteTextures(1, &textureID);
}

void Texture::createOpenGLTexture(const GLenum filterType, bool srgb)
{
	glGenTextures(1, &textureID);			// Generate OpenGL texture IDs
	glBindTexture(GL_TEXTURE_2D, textureID);		// Bind Our Texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered

	if(srgb)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, filterType);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, filterType);

	glGenerateMipmap(GL_TEXTURE_2D); // mip map
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtered	

	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
}

void CubeMap::loadImages(std::vector<std::string> names)
{
	int i{ 0 };
	for (const auto& name : names)
	{
		int width, height, bpp;
		unsigned char* data = stbi_load(name.c_str(), &width, &height, &bpp, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		i++;
		if (data) stbi_image_free(data);
	}
}