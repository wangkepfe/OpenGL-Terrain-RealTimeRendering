#include "lightingTest.h"

#include "shaderUtility.hpp"
#include "imageUtility.h"
#include "model.hpp"

#include "ModelObject3D.hpp"

#include "lightingSystem.hpp"

#include "shadowMap.hpp"

LightingSystem* LightingTest::init(Camera* camera)
{
	auto lightingSys = new LightingSystem();

	auto ambientLight = new AmbientLight(vec3(0.5, 0.5, 0.5));
	auto sunLight = new DirectionalLight(vec3(0.5, 0.5, 0.5), vec3(1, -1, 0));
	auto moonLight = new DirectionalLight(vec3(0.2, 0.2, 0.2), vec3(1, -1, 0));

	lightingSys->addLight(ambientLight, "ambient");

	lightingSys->addLight(sunLight, "sunLight");
	lightingSys->addLight(moonLight, "moonLight");

	//lightingSys->bindShader(*shaderCloudsLQ);

	// shadow map test
	auto shadowMap = new ShadowMap();
	shadowMap->create(camera);

	updateFunc = [=](double t)
	{
		lightingSys->update();
		lightingSys->upload();

		// ---------- sky ----------
		const vec3 sunFull(1.0f, 1.0f, 0.9f);
		const vec3 sunDawn(1.0f, 0.5f, 0.15f);
		const vec3 dark(0.0f, 0.0f, 0.0f);
		const vec3 moon(0.05f, 0.15f, 0.3f);
		const float sunLightRamp[4] = { 0.4f, 0.1f, 0.0f, -0.2f };
		const float moonLightRamp[2] = { 0.1f, -0.1f };

		// sun position
		vec3 sunPos = vec3(1, 0, 0);
		const vec3 sunRotAx = vec3(0, 1, 1);
		const vec3 starRotAx = vec3(1, 1, 0);

		mat3 rotMat = glm::rotate(mat4(), static_cast<float>(t * 0.0001), sunRotAx);
		sunPos = rotMat * sunPos;

		// update sun position to shadow map
		shadowMap->updateLightSourcePosition(sunPos);

		// shadow map rendering
		mat4 vpMat;
		shadowMap->render(shadowMapRenderFunc, vpMat, width, height);

		// upload to shaders
		lightingSys->uploadShadowMapping(shadowMap->getTexture(), "texShadowMap", 29, vpMat, "lightingSpace");

		// sun light & ambient light
		sunLight->setDirec(-sunPos);

		if (sunPos.y > sunLightRamp[0])
		{
			sunLight->setColor(sunFull);
			ambientLight->setColor(sunFull);
		}
		else if (sunPos.y > sunLightRamp[1])
		{
			float mixFac = (sunPos.y - sunLightRamp[1]) / (sunLightRamp[0] - sunLightRamp[1]);
			sunLight->setColor(glm::mix(sunDawn, sunFull, mixFac));
			ambientLight->setColor(glm::mix(sunDawn, sunFull, mixFac));
		}
		else if (sunPos.y > sunLightRamp[2])
		{
			sunLight->setColor(sunDawn);
			ambientLight->setColor(sunDawn);
		}
		else if (sunPos.y > sunLightRamp[3])
		{
			float mixFac = (sunPos.y - sunLightRamp[3]) / (sunLightRamp[2] - sunLightRamp[3]);
			sunLight->setColor(glm::mix(dark, sunDawn, mixFac));
			ambientLight->setColor(glm::mix(moon, sunDawn, mixFac));
		}
		else
		{
			sunLight->setColor(dark);
			ambientLight->setColor(moon);
		}

		// moon light
		moonLight->setDirec(sunPos);

		if (sunPos.y > moonLightRamp[0])
		{
			moonLight->setColor(dark);
		}
		else if (sunPos.y > moonLightRamp[1])
		{
			moonLight->setColor(glm::mix(moon, dark, (sunPos.y - moonLightRamp[1]) / (moonLightRamp[0] - moonLightRamp[1])));
		}
		else
		{
			moonLight->setColor(moon);
		}
	};

	cleanUpFunc = [=]()
	{
		delete ambientLight;
		delete sunLight;
		delete moonLight;

		delete lightingSys;

		shadowMap->cleanUp();
		delete shadowMap;
	};

	/////
	getShadowMapTex = [=]()->GLuint
	{
		return shadowMap->getTexture();
	};
	/////

	return lightingSys;
}

void LightingTest::setShadowMapRenderFunc(const std::function<void(const Shader&)>& _shadowMapRenderFunc)
{
	shadowMapRenderFunc = _shadowMapRenderFunc;
}
