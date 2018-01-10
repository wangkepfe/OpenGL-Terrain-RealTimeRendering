#ifndef mfbo_hpp
#define mfbo_hpp

#include <iostream>

//#define GLEW_STATIC
#include <gl/glew.h>

class mFBO
{
public:
	mFBO(GLuint width, GLuint height)
		: width{ width }
		, height{ height }
		, fbo{ 0 }
		, colorTex{ 0 }
		, depthTex{ 0 }
	{}
	~mFBO() 
	{
	}

	void cleanUp()
	{
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &colorTex);
		glDeleteTextures(1, &depthTex);
	}

	void create()
	{
		glGenFramebuffers(1, &fbo);
	}

	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void createColorTexAttachment()
	{
		glGenTextures(1, &colorTex);
		glBindTexture(GL_TEXTURE_2D, colorTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
		}
	}

	void createDepthTexAttachment()
	{
		glGenTextures(1, &depthTex);
		glBindTexture(GL_TEXTURE_2D, depthTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
		}
	}

	void createDepthStencilTexAttachment()
	{
		glGenTextures(1, &depthTex);
		glBindTexture(GL_TEXTURE_2D, depthTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
		}
	}

	void createDepthBufferAttachment()
	{
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
		}
	}

	void createDepthStencilBufferAttachment()
	{
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
		}
	}

	// multisample antialiasing MSAA
	
	void createMultisampledAttachments()
	{
		// color
		unsigned int textureColorBufferMultiSampled;

		glGenTextures(1, &textureColorBufferMultiSampled);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

		// depth and stencil
		unsigned int rbo;

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		
		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
		}
	}
	
	operator GLuint() const {
		return fbo;
	}

	GLuint getColorTex() { return colorTex; }
	GLuint getDepthTex() { return depthTex; }

private:
	GLuint width, height;

	GLuint fbo, colorTex, depthTex;
};

#endif // !renderedTexture_hpp

