#include "lake.h"

static const GLfloat vertices[] = {
	0.5f, 0.0f, 0.5f,  // Top Right
	0.5f, 0.0f, -0.5f,  // Bottom Right
	-0.5f, 0.0f, -0.5f,  // Bottom Left
	-0.5f, 0.0f, 0.5f   // Top Left
};

static const GLuint indices[] = {  // Note that we start from 0!
	0, 1, 3,   // First Triangle
	1, 2, 3    // Second Triangle
};

static const GLfloat texCoord[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};

void Lake::cleanUp()
{
	cleanFBO();

	plane->getModel()->cleanUp();
	delete plane->getModel();
	delete plane;

	ScreenQuad::cleanUp();
}

void Lake::create(Shader* planeShader, Camera* camera)
{
	createFBO();
	createPlane(planeShader, camera);
	ScreenQuad::create();
}

void Lake::createFBO()
{
	reflectionFBO.create();
	reflectionFBO.bind();
	reflectionFBO.createColorTexAttachment();
	reflectionFBO.createDepthBufferAttachment();
	reflectionFBO.unbind();

	refractionFBO.create();
	refractionFBO.bind();
	refractionFBO.createColorTexAttachment();
	refractionFBO.createDepthTexAttachment();
	refractionFBO.unbind();
}

void Lake::cleanFBO()
{
	reflectionFBO.cleanUp();
	refractionFBO.cleanUp();
}

void Lake::createPlane(Shader* planeShader, Camera* camera)
{
	Vertex vertex;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices{ 0, 1, 3, 1, 2, 3 };

	vertex.position = vec3(1.0f, 0.0f, 1.0f);
	vertex.normal = vec3(0.0f, 1.0f, 0.0f);
	vertex.texCoord = vec2(0.0f, 0.0f);
	vertices.push_back(vertex);

	vertex.position = vec3(1.0f, 0.0f, -1.0f);
	vertex.normal = vec3(0.0f, 1.0f, 0.0f);
	vertex.texCoord = vec2(0.0f, 1.0f);
	vertices.push_back(vertex);

	vertex.position = vec3(-1.0f, 0.0f, -1.0f);
	vertex.normal = vec3(0.0f, 1.0f, 0.0f);
	vertex.texCoord = vec2(1.0f, 1.0f);
	vertices.push_back(vertex);

	vertex.position = vec3(-1.0f, 0.0f, 1.0f);
	vertex.normal = vec3(0.0f, 1.0f, 0.0f);
	vertex.texCoord = vec2(1.0f, 0.0f);
	vertices.push_back(vertex);

	plane = new ModelObject3D(planeShader, new Model(vertices, indices), camera);

	plane->updateAABBfromModel();
}

bool Lake::renderToTexture(Controller* controller, Camera* camera)
{
	// before started: check frustum
	plane->updateModelMatrix();

	auto frus = plane->getViewFrustum();

	if (frus->cullingAABB(plane->getModelMatrix() * plane->getAABB()))
	{
		culled = true;
		return false;
	}

	// ---------- step 1 : reflection fbo ----------
	
	vec4 clippingPlane;

	// bind fbo
	reflectionFBO.bind();

	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// getReflect camera position and lookat
	const float seaLevel = getWaterHeight();

	vec3 camP = controller->getCamPos();
	camP.y = camP.y - 2 * (camP.y - seaLevel);

	vec3 camLA = controller->getCamLookAt();
	camLA.y = camLA.y - 2 * (camLA.y - seaLevel);

	camera->setPos(camP);
	camera->setLookAt(camLA);
	frus->update(camera);

	// enable clip plane

	glEnable(GL_CLIP_DISTANCE0);
	clippingPlane = vec4(0.0f, 1.0f, 0.0f, - seaLevel);

	// render the scene using this camera and frustum
	reflectionRenderFunc(clippingPlane);
	glDisable(GL_CLIP_DISTANCE0);

	// unbind fbo
	reflectionFBO.unbind();


	// ---------- step 2 : refraction fbo ------------

	// bind fbo
	refractionFBO.bind();

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera & frustum
	camera->setPos(controller->getCamPos());
	camera->setLookAt(controller->getCamLookAt());
	frus->update(camera);

	// enable clip plane

	glEnable(GL_CLIP_DISTANCE0);
	clippingPlane = vec4(0.0f, -1.0f, 0.0f, seaLevel);

	// render
	refractionRenderFunc(clippingPlane);
	glDisable(GL_CLIP_DISTANCE0);

	// unbind fbo
	refractionFBO.unbind();

	culled = false;
	return true;
}

void Lake::render(Texture * texDUDV, Texture * texNormal)
{
	if (culled)
		return;

	//  ---------- step 3 : render to screen quad -----------

	// upload the rendered textures
	uploadTexture(*texDUDV, "dudvTex", 0);
	uploadTexture(*texNormal, "normalTex", 1);
	uploadTexture(reflectionFBO.getColorTex(), "reflectionTex", 2);
	uploadTexture(refractionFBO.getColorTex(), "refractionTex", 3);
	uploadTexture(refractionFBO.getDepthTex(), "depthTex", 4);

	// render plane into stencil buffer for 1
	glEnable(GL_STENCIL_TEST);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glStencilOp(GL_REPLACE, GL_ZERO, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	plane->render(true, true, true, false);

	// render the screen quad if only stencil buffer is equal to 1
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_EQUAL, 1, 0xFF);

	//glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ScreenQuad::render();

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);

	glDisable(GL_STENCIL_TEST);
}