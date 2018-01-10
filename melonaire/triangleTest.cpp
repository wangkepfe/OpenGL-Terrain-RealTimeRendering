#include "triangleTest.h"

#include "shaderUtility.hpp"

void TriangleTest::init()
{
	glDisable(GL_DEPTH_TEST);

	auto shaderNaive = new Shader("naive.vert", "naive.frag");

	GLfloat vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f,  0.5f, 0.0f
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	updateFunc = [=](double t)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderNaive->use();
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		GLint loc = glGetAttribLocation(*shaderNaive, "inPosition");
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	};

	cleanUpFunc = [=]()
	{
		shaderNaive->cleanUp();
		delete shaderNaive;
	};
}
