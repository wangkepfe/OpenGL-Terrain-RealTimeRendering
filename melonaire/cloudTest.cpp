#include "cloudTest.h"

#include "ModelObject3D.hpp"

void CloudTest::init(Camera* camera, ViewFrustum* frus, LightingSystem* lightingSys)
{
	auto shaderCloudsLQ = new Shader("cloudsLQ.vert", "cloudsLQ.frag");
	lightingSys->bindShader(*shaderCloudsLQ);

	auto texClouds = new Texture("tex/cloud/cloud3.png", false);

	auto mPlane = new Model("plane.obj");

	/***************************************/

	auto cloudHigh = new ModelObject3D(shaderCloudsLQ, mPlane, camera);

	cloudHigh->setTexture(texClouds, "tex");

	cloudHigh->setPos(vec3(-1000, 100, -1000));
	cloudHigh->setRotAx(vec3(1, 0, 0));
	cloudHigh->setRotRd(glm::radians(90.f));
	cloudHigh->setScal(vec3(1.f, 1.f, 1.f) * 2000.f);

	cloudHigh->setViewFrustum(frus);
	cloudHigh->updateAABBfromModel();

	/***************************************/

	updateFunc = [=](double t)
	{
		cloudHigh->uploadUniform(static_cast<float>(t * 0.00001), "unitime");

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		cloudHigh->render();

		glDisable(GL_BLEND);
	};

	renderFunc = [=]()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		cloudHigh->render();

		glDisable(GL_BLEND);
	};

	cleanUpFunc = [=]()
	{
		shaderCloudsLQ->cleanUp();
		delete shaderCloudsLQ;

		texClouds->freeImage();
		delete texClouds;

		mPlane->cleanUp();
		delete mPlane;

		delete cloudHigh;
	};
}
