#include "plantTest.h"

#include "ModelObject3DInstanced.hpp"

void PlantTest::init(Camera* camera, ViewFrustum* frus, LightingSystem* lightingSys, Terrain* terrain, float waterLevel)
{
	// shader
	//auto shaderPlant = new Shader("plantHQ.vert", "plantHQ.frag");
	auto shaderPlant = new Shader("plantMQ.vert", "plantMQ.frag");
	lightingSys->bindShader(*shaderPlant);

	const unsigned int speciesNum = 7;
	int i, j;

	// texture
	Texture* texPlant[speciesNum];
	texPlant[0] = new Texture("tex/plant/bigBranch1.png");
	texPlant[1] = new Texture("tex/plant/bigBranch2.png");

	texPlant[2] = new Texture("tex/plant/plant01.png");
	texPlant[3] = new Texture("tex/plant/plant02.png");
	texPlant[4] = new Texture("tex/plant/plant03.png");
	texPlant[5] = new Texture("tex/plant/plant04.png");
	texPlant[6] = new Texture("tex/plant/plant05.png");

	// model
	auto modelPlant = new Model("model/plant/plant.obj");

	// instanced model matrices
	const unsigned int amount[speciesNum] = {100, 100, 400, 300, 100, 200, 50};
	mat4 **modelMatrices = new mat4*[speciesNum];
	for (i = 0; i < speciesNum; i++)
	{
		modelMatrices[i] = new mat4[amount[i]];
		for (j = 0; j < amount[i]; j++)
		{
			mat4 m;
			// 1 translation
			float x = randm1to1() * 50;
			float z = randm1to1() * 50;

			float y = terrain->getHeight(x, z);

			while (y < -0.2f || y > 0.8f)
			{
				x = randm1to1() * 50;
				z = randm1to1() * 50;

				y = terrain->getHeight(x, z);
			}

			m = glm::translate(m, vec3(x, y, z));

			// 2 rotate
			m = glm::rotate(m, rand0to1() * 360, vec3(0, 1, 0));

			// 3 scale
			float scale;
			if(i == 0 || i == 1)
				scale = 1.0f + rand0to1() * 0.2f;
			else
				scale = 0.3f + rand0to1() * 0.1f;
			m = glm::scale(m, vec3(scale));

			modelMatrices[i][j] = m;
		}
	}

	// modelObject3D instanced
	ModelObject3DInstanced* plants[speciesNum];

	for (i = 0; i < speciesNum; i++)
	{
		plants[i] = new ModelObject3DInstanced(shaderPlant, modelPlant, camera);
		plants[i]->setTexture(texPlant[i], "tex");
		plants[i]->setModelMatrices(modelMatrices[i], amount[i]);
	}
	
	
	updateFunc = [=](double t)
	{
		/*
		for (int i = 0; i < n; i++)
		{
			plant->setPos(plantPos[i]);
			plant->render();
		}
		*/
		for (int i = 0; i < speciesNum; i++)
			plants[i]->render();
	};

	renderClipFunc = [=](const vec4& clippingPlane)
	{
		/*
		plant->uploadUniform(clippingPlane, "clippingPlane");
		for (int i = 0; i < n; i++)
		{
			plant->setPos(plantPos[i]);
			plant->render();
		}
		*/
		for (int i = 0; i < speciesNum; i++)
		{
			plants[i]->uploadUniform(clippingPlane, "clippingPlane");
			plants[i]->render();
		}
	};

	cleanUpFunc = [=]()
	{
		for (int i = 0; i < speciesNum; i++)
			delete modelMatrices[i];

		delete modelMatrices;

		shaderPlant->cleanUp();
		delete shaderPlant;

		for (int i = 0; i < speciesNum; i++)
		{
			texPlant[i]->freeImage();
			delete texPlant[i];
		}

		modelPlant->cleanUp();
		delete modelPlant;

		delete plants;
	};
}
