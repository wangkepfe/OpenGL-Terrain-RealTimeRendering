#include "ShadowMappingTest.h"

#include "ModelObject3D.hpp"
#include "screenQuad.hpp"

namespace {
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	unsigned int depthMap;
}

void depthMapfunc()
{
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMappingTest::init(Camera* camera)
{
	glEnable(GL_DEPTH_TEST);

	auto shader = new Shader("shadowMappingTest.vert", "shadowMappingTest.frag");
	auto shaderDepth = new Shader("simpleDepthShader.vert", "simpleDepthShader.frag");
	auto shaderDepthMapPreview = new Shader("postprocess.vert", "depthMap.frag");

	auto tex1 = new Texture("tex/brickWall/albedo.png");
	auto tex2 = new Texture("tex/woodWall/albedo.png");
	auto tex3 = new Texture("tex/stoneWall/albedo.png");

	auto mdl1 = new Model("model/box.obj");
	auto mdl2 = new Model("model/ball.obj");
	auto mdl3 = new Model("model/plane.obj");

	auto obj1 = new ModelObject3D(shader, mdl1, camera);
	obj1->setTexture(tex1, "tex");
	obj1->setPos(vec3(5, -1, 0));
	obj1->setScal(vec3(1.0f, 1.0f, 1.0f));

	auto obj2 = new ModelObject3D(shader, mdl2, camera);
	obj2->setTexture(tex2, "tex");
	obj2->setPos(vec3(5, 2, 5));
	obj2->setScal(vec3(2.0f, 2.0f, 2.0f));

	auto obj3 = new ModelObject3D(shader, mdl3, camera);
	obj3->setTexture(tex3, "tex");
	obj3->setPos(vec3(0, -2, 0));
	obj3->setScal(vec3(20.0f, 1.0f, 20.0f));

	// lighting
	vec3 dir = vec3(0.5, -1, 0.5);
	obj1->uploadUniform(dir, "lightDir");

	auto orthoCamera = new OrthogonolCamera(50,50,1,100);
	orthoCamera->setPos(-dir*40.f);
	orthoCamera->setLookAt(vec3());
	orthoCamera->updateViewMatrix();

	mat4 lightSpace = orthoCamera->getVPmatrix();

	shader->uploadUniform(lightSpace, "lightingSpace");
	shaderDepth->uploadUniform(lightSpace, "lightingSpace");

	//fbo
	depthMapfunc();

	//screen quad
	auto quad = new ScreenQuad();
	quad->setShader(*shaderDepthMapPreview);
	quad->create();

	updateFunc = [=](double t)
	{
		//
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);

		obj1->shadowMappingRender(*shaderDepth);
		obj2->shadowMappingRender(*shaderDepth);
		obj3->shadowMappingRender(*shaderDepth);

		//glDisable(GL_CULL_FACE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//
		/*glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		quad->uploadTexture(depthMap, "depthMap", 0);
		quad->render();*/

		//
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto tex = Texture(depthMap);
		tex.upload(*shader, "texShadowMap", 1);

		obj1->uploadUniform(1, "textureScale");
		obj1->render(true, true, true, false);

		obj2->render(true, true, true, false);

		obj3->uploadUniform(50, "textureScale");
		obj3->render(true, true, true, false);

		/*glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		obj1->uploadUniform(1, "textureScale");
		obj1->render(true, true, true, false);

		obj2->render(true, true, true, false);

		obj3->uploadUniform(50, "textureScale");
		obj3->render(true, true, true, false);*/
	};

	cleanUpFunc = [=]()
	{
		shader->cleanUp();
		tex1->freeImage();
		mdl1->cleanUp();
		tex2->freeImage();
		mdl2->cleanUp();
		tex3->freeImage();
		mdl3->cleanUp();

		delete shader;
		delete tex1;
		delete mdl1;
		delete obj1;
		delete tex2;
		delete mdl2;
		delete obj2;
		delete tex3;
		delete mdl3;
		delete obj3;

		delete orthoCamera;
	};
}
