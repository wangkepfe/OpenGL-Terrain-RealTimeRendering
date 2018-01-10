#include "shadowMap.hpp"

void ShadowMap::create(Camera* _camera)
{
	// fbo
	fbo.create();
	fbo.bind();
	fbo.createDepthTexAttachment();

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	fbo.unbind();

	// camera
	orthocamera = new OrthogonolCamera(100.f, 100.f, 1.f, 100.0f);
	camera = _camera;

	// shader
	shader = new Shader("simpleDepthShader.vert", "simpleDepthShader.frag");
}

void ShadowMap::updateLightSourcePosition(vec3 dir)
{
	// when sun is under the ground, moon arises
	if (dir.y > 0)
		dir = -dir;

	orthocamera->setPos(- dir * 50.0f);
	orthocamera->setLookAt(vec3(0.0f));
}

void ShadowMap::render(const std::function<void(const Shader&)>& renderFunc, glm::mat4& vpMat, GLuint width, GLuint height)
{
	// calculate view matrix
	orthocamera->updateViewMatrix();
	vpMat = orthocamera->getVPmatrix();

	// viewport
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	// bind fbo
	fbo.bind();

	// clear buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	// render
	renderFunc(*shader);
	 
	// unbind fbo
	fbo.unbind();

	// viewport
	glViewport(0, 0, width, height);
}

void ShadowMap::cleanUp()
{
	fbo.cleanUp();

	delete orthocamera;

	shader->cleanUp();
	delete shader;
}
