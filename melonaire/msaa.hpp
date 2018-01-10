#ifndef msaa_hpp
#define msaa_hpp

#include <functional>

#include "screenQuad.hpp"
#include "mfbo.hpp"

class MultisampledAntialiasing : public ScreenQuad
{
public:
	MultisampledAntialiasing(GLuint width, GLuint height, Shader* shader)
		: msFBO(width, height)
		, postProcessFBO(width, height)
		, width{ width }
		, height{ height }
	{
		setShader(*shader);
	}
	~MultisampledAntialiasing() {}

	void create() override
	{
		msFBO.create();
		msFBO.bind();
		msFBO.createMultisampledAttachments();
		msFBO.unbind();

		postProcessFBO.create();
		postProcessFBO.bind();
		postProcessFBO.createColorTexAttachment();
		postProcessFBO.createDepthStencilBufferAttachment();
		postProcessFBO.unbind();

		ScreenQuad::create();
	}

	void cleanUp() override
	{
		msFBO.cleanUp();
		postProcessFBO.cleanUp();
		ScreenQuad::cleanUp();
	}

	void render(double t)
	{
		//*************************************
		// bind ms fbo
		msFBO.bind();

		// clear buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// render the scene
		renderFunc(t);

		//*************************************
		// resolve multisampled buffer(s) into postprocessing FBO
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessFBO);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// unbind ms fbo
		msFBO.unbind();

		//*************************************
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
	mFBO msFBO;
	mFBO postProcessFBO;

	GLuint width, height;

	std::function<void(double)> renderFunc;
};

#endif // !msaa_hpp

