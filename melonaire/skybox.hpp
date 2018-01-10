#ifndef skybox_hpp
#define skybox_hpp

//#define GL_STATIC
#include <gl/glew.h>

#include "imageUtility.h"
#include "shaderUtility.hpp"

static float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f, // -z
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f, // -x
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f, // +x
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f, // +z
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f, // +y
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f, // -y
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

class mSkyBox
{
public:
	mSkyBox() {}
	~mSkyBox() {}

	void setProjectionMatrix(const mat4& _projectionMatrix) { projectionMatrix = _projectionMatrix; }
	void setCameraMatrix(const mat4& _cameraMatrix) { cameraMatrix = _cameraMatrix; }
	void setModelMatrix(const mat4& _modelMatrix) { modelMatrix = _modelMatrix; }

	mat4 getCameraMatrix() { return cameraMatrix; }

	void setCubeMap(CubeMap* _cubeMap) { cubeMap = _cubeMap; }

	void init()
	{
		modelMatrix = mat4();

		// shader
		shader = Shader("shader/skybox.vert", "shader/skybox.frag");

		// model: cube
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), skyboxVertices, GL_STATIC_DRAW);
	}

	void render()
	{
		// depth test
		glDepthMask(GL_FALSE);
		glUseProgram(shader);

		// view matrix, projection matrix
		cameraMatrix[3][0] = 0;
		cameraMatrix[3][1] = 0;
		cameraMatrix[3][2] = 0;
		mat4 vpMatrix = projectionMatrix * cameraMatrix;
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewProjection"), 1, GL_FALSE, &vpMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	
		// vao, vbo
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// texture
		cubeMap->upload(shader, GL_TEXTURE0, 0, "skybox");

		// draw
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ~ depth test
		glDepthMask(GL_TRUE);
	}

private:
	mat4 modelMatrix;
	mat4 cameraMatrix;
	mat4 projectionMatrix;

	Shader shader;
	GLuint skyboxVAO, skyboxVBO;
	CubeMap* cubeMap;
};

#endif // !skybox_hpp

