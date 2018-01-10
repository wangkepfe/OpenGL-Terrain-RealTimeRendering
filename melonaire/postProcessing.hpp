#ifndef postProcessing_hpp
#define postProcessing_hpp

#include "renderedTexture.hpp"
#include "shaderUtility.hpp"

static const GLfloat fboVertices[] = {
	-1, -1,
	1, -1,
	-1,  1,
	1,  1,
};

class PostProcessing
{
public:
	PostProcessing(GLuint width, GLuint height)
		: width{ width }
		, height{ height }
		, fbo(width, height)
	{}
	~PostProcessing(){}

	void setShader(Shader _shader)
	{
		shader = _shader;
	}

	void createQuad()
	{
		shader.use();

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), fboVertices, GL_STATIC_DRAW);
	}

	void createFBO(int type = 0)
	{
		fbo.create();
		fbo.bind();
		fbo.createColorTexAttachment();

		if (type == 0)
			fbo.createDepthStencilBufferAttachment();
		else if(type == 1)
			fbo.createDepthBufferAttachment();
		else if(type == 2)
			fbo.createDepthStencilTexAttachment();
		else if (type == 3)
			fbo.createDepthTexAttachment();

		fbo.unbind();
	}

	void begin()
	{
		fbo.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, width, height);
	}

	void continueBegin()
	{
		fbo.bind();
		glViewport(0, 0, width, height);
	}

	void end()
	{
		fbo.unbind();	
	}

	GLuint getColorTex() { return fbo.getColorTex(); }
	GLuint getDepthTex() { return fbo.getDepthTex(); }

	void uploadUniform(vec3 v, const std::string nameInShader)
	{
		shader.use();
		glUniform3f(glGetUniformLocation(shader, nameInShader.c_str()), v.x, v.y, v.z);
	}

	void uploadUniform(mat4 m, const std::string nameInShader)
	{
		shader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader, nameInShader.c_str()), 1, GL_FALSE, &m[0][0]);
	}

	void uploadUniform(float n, const std::string nameInShader)
	{
		shader.use();
		glUniform1f(glGetUniformLocation(shader, nameInShader.c_str()), n);
	}

	void uploadTexture(const GLenum texEnum, GLuint tex, const std::string nameInShader, GLuint loc)
	{
		shader.use();

		glActiveTexture(texEnum);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(glGetUniformLocation(shader, nameInShader.c_str()), loc);
	}

	void render()
	{
		shader.use();

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

private:
	GLuint width, height;

	mFBO fbo;

	Shader shader;

private:
	GLuint vao, vbo;
};

#endif
