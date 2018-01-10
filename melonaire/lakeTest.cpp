#include "lakeTest.h"

#include "lake.h"

void LakeTest::init(Camera* camera, ViewFrustum* frus, LightingSystem* lightingSys, 
	const std::function<void(const vec4&)>& reflectionRenderFunc,
	const std::function<void(const vec4&)>& refractionRenderFunc,
	float waterLevel)
{
	// shader
	auto shaderSimplest = new Shader("simplest.vert", "simplest.frag");
	auto shaderLakeHQ = new Shader("water.vert", "water.frag");
	lightingSys->bindShader(*shaderLakeHQ);

	// texture
	auto texWaterDUDV = new Texture("tex/lake/waterDUDV.png", false);
	auto texWaterN = new Texture("tex/lake/waterNormal.png", false);

	// lake
	auto lake = new Lake(width, height, shaderLakeHQ);
	lake->create(shaderSimplest, camera);	
	lake->setPosScal(vec3(0.f, waterLevel, 0.f), vec3(1.f, 1.f, 1.f) * 200.f);// position and scale
	lake->setFrustum(frus);

	lake->uploadUniform(waterLevel, "waterLevel");

	// **** inversed matrix ( for ray tracing in shader )
	const mat4 inversedProjMat = glm::inverse(camera->getProjectionMatrix());

	lake->setReflectionRenderFunc(reflectionRenderFunc);
	lake->setRefractionRenderFunc(refractionRenderFunc);

	updateFunc = [=](double t)
	{
		// water
		const mat4 inverseViewMat = glm::inverse(camera->getViewMatrix());

		lake->uploadUniform(inversedProjMat, "inverseProjection");
		lake->uploadUniform(inverseViewMat, "inverseView");
		lake->uploadUniform(camera->getPos(), "viewPos");
		lake->uploadUniform(t * 0.00001, "time");

		lake->renderToTexture(controller, camera);
	};

	renderFunc = [=]()
	{
		lake->render(texWaterDUDV, texWaterN);
	};

	cleanUpFunc = [=]()
	{
		shaderLakeHQ->cleanUp();
		shaderSimplest->cleanUp();
		delete shaderLakeHQ;
		delete shaderSimplest;

		texWaterDUDV->freeImage();
		texWaterN->freeImage();
		delete texWaterDUDV;
		delete texWaterN;

		lake->cleanUp();
		delete lake;
	};
}
