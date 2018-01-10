#ifndef EnvironmentMap_hpp
#define EnvironmentMap_hpp

#include <iostream>

//#define GLEW_STATIC
#include <gl/glew.h>

#include "shaderUtility.hpp"
#include "imageUtility.h"
#include "renderedTexture.hpp"

#include "glm/glm.hpp"
using glm::vec3;
using glm::mat4;

#include "glm/gtc/matrix_transform.hpp"

class EnvironmentMap
{
public:
	EnvironmentMap() {}
	~EnvironmentMap() {}

	void init(int _size){
		size = _size;

		//fbo
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size, size);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		//cubemap
		cubeMap.createEmpty(size, size);
	}
	CubeMap getCubemap() { return cubeMap; }

	//fbo
	void begin(){ 
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, size, size);
	}
	void continueBegin(){
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
	void end(){ glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	//texture
	void bindTex(int i){
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap, 0);
	}
	
	//matrix
	void setPosition(vec3 _pos) { pos = _pos; }
	mat4 getProjectionMatrix(){
		return glm::perspective(glm::radians(90.0f), 1.0f, 0.2f, 200.f);
	}
	mat4 switchCameraMatrix(int i){
		mat4 m;
		switch (i)
		{
		case 0:
			m = glm::lookAt(pos, pos + vec3(1, 0, 0), vec3(0, 1, 0));
			break;
		case 1:
			m = glm::lookAt(pos, pos + vec3(-1, 0, 0), vec3(0, 1, 0));
			break;
		case 2:
			m = glm::lookAt(pos, pos + vec3(0, 1, 0), vec3(1, 0, 0));
			break;
		case 3:
			m = glm::lookAt(pos, pos + vec3(0, -1, 0), vec3(1, 0, 0));
			break;
		case 4:
			m = glm::lookAt(pos, pos + vec3(0, 0, 1), vec3(0, 1, 0));
			break;
		case 5:
			m = glm::lookAt(pos, pos + vec3(0, 0, -1), vec3(0, 1, 0));
			break;
		}
		return m;
	}

private:
	int size;
	GLuint fbo;
	CubeMap cubeMap;
	vec3 pos;
};


#endif
