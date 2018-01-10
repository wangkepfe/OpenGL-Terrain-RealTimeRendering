#ifndef screenQuad_hpp
#define screenQuad_hpp

#include "glObject.hpp"

static const GLfloat fboVertices[] = {
	-1, -1,
	1, -1,
	-1,  1,
	1,  1,
};

class ScreenQuad : public OpenGLObject
{
public:
	ScreenQuad() {}
	~ScreenQuad() {}

	virtual void cleanUp()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	virtual void create()
	{
		shader.use();

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), fboVertices, GL_STATIC_DRAW);
	}

	virtual void render()
	{
		shader.use();

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

private:
	GLuint vao, vbo;
};

#endif // !screenQuad_hpp

