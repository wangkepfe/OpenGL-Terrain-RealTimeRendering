#include "terrainTest.h"

void TerrainTest::init(Camera* camera, ViewFrustum* frus, LightingSystem* lightingSys)
{
	// shader
	auto shaderTerrain = new Shader("terrainHighQuality.vert", "terrainHighQuality.frag");
	lightingSys->bindShader(*shaderTerrain);

	// textures & models

	auto texDirt = new Texture("tex/rockDirt/albedo.png");
	auto texDirtN = new Texture("tex/rockDirt/normal.png", false);
	auto texDirtS = new Texture("tex/rockDirt/roughness.png", false);

	auto texSand = new Texture("tex/sand/albedo.png");
	auto texSandN = new Texture("tex/sand/normal.png", false);
	auto texSandS = new Texture("tex/sand/rondughness.png", false);

	auto texRock = new Texture("tex/rock/albedo.png");
	auto texRockN = new Texture("tex/rock/normal.png", false);
	auto texRockS = new Texture("tex/rock/roughness.png", false);

	auto texGrass = new Texture("tex/grass.png");
	auto texNoise = new Texture("tex/cloud/cloud3.png", false);

	// ----------- terrain ------------
	auto terrain = new Terrain(256, 16);

	// shader, camera
	terrain->setShader(*shaderTerrain);
	terrain->setCamera(camera);

	// textures
	terrain->setTexture(texRock, "texRock");
	terrain->setTexture(texRockN, "texRockN");
	terrain->setTexture(texRockS, "texRockS");

	terrain->setTexture(texSand, "texSand");
	terrain->setTexture(texSandN, "texSandN");
	terrain->setTexture(texSandS, "texSandS");

	terrain->setTexture(texDirt, "texDirt");
	terrain->setTexture(texDirtN, "texDirtN");
	terrain->setTexture(texDirtS, "texDirtS");

	terrain->setTexture(texNoise, "texNoise");
	terrain->setTexture(texGrass, "texGrass");

	// texture scale
	terrain->setTextureScale(100.0f);

	// noise resolution
	terrain->setResolution(0.019f);
	terrain->setNoiseSeed(244);

	// scale
	terrain->setScaleXZ(0.5f);
	terrain->setScaleY(5.0f);

	// pos offset
	terrain->centralize();

	terrain->setMipmapBase(20.f);

	// view frus
	terrain->setViewFrustum(frus);

	// generate
	terrain->generate();

	// ----- --------------- -----

	updateFunc = [=](double t)
	{
		terrain->updateTerrain();
		terrain->render();
	};

	renderClipFunc = [=](const vec4& clippingPlane)
	{
		terrain->uploadUniform(clippingPlane, "clippingPlane");
		terrain->render();
	};

	renderFuncForShadowMaping = [=](const Shader& shader)
	{
		terrain->renderForShadowMap(shader);
	};

	getTerrain = [=]()->Terrain*
	{
		return terrain;
	};

	cleanUpFunc = [=]()
	{
		shaderTerrain->cleanUp();
		delete shaderTerrain;

		texDirt->freeImage();
		texDirtN->freeImage();
		texDirtS->freeImage();
		delete texDirt;
		delete texDirtN;
		delete texDirtS;

		texSand->freeImage();
		texSandN->freeImage();
		texSandS->freeImage();
		delete texSand;
		delete texSandN;
		delete texSandS;

		texRock->freeImage();
		texRockN->freeImage();
		texRockS->freeImage();
		delete texRock;
		delete texRockN;
		delete texRockS;

		texGrass->freeImage();
		delete texGrass;

		texNoise->freeImage();
		delete texNoise;

		terrain->cleanUp();
		delete terrain;
	};
}
