#ifndef postprocess_hpp
#define postprocess_hpp

#include <functional>

#include "screenQuad.hpp"
#include "mfbo.hpp"

class PostProcess : public ScreenQuad
{
public:
	PostProcess(GLuint width, GLuint height, Shader* shader)
		: postProcessFBO(width, height)
		, width{width}
		, height{height}
	{
		setShader(*shader);
	}
	~PostProcess(){}

	void create() override
	{
		postProcessFBO.create();
		postProcessFBO.bind();
		postProcessFBO.createColorTexAttachment();
		postProcessFBO.createDepthStencilBufferAttachment();
		postProcessFBO.unbind();

		ScreenQuad::create();
	}

	void cleanUp() override
	{
		postProcessFBO.cleanUp();
		ScreenQuad::cleanUp();
	}

	void render(double t)
	{
		// bind fbo
		postProcessFBO.bind();

		// clear buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// render the scene
		renderFunc(t);

		// unbind fbo
		postProcessFBO.unbind();
		
		// upload texture
		uploadTexture(postProcessFBO.getColorTex(), "screenTexture", 0);

		// render
		ScreenQuad::render();
	}

	void setRenderFunc(const std::function<void(double)>& _renderFunc)
	{
		renderFunc = _renderFunc;
	}


private:
	mFBO postProcessFBO;

	GLuint width, height;

	std::function<void(double)> renderFunc;
};

#endif // !postprocess_hpp

