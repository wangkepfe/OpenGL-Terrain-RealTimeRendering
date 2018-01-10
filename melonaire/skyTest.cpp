#include "skyTest.h"

#include "skydome.hpp"

void SkyTest::init(Camera* camera)
{
	// shader
	auto shaderSky = new Shader("skydome.vert", "skydome.frag");
	
	// textures
	auto texSkyTint = new Texture("tex/skydome/tint.png", true, true, GL_CLAMP_TO_EDGE);
	auto texSkyTint2 = new Texture("tex/skydome/tint2.png", true, true, GL_CLAMP_TO_EDGE);
	auto texSkySun = new Texture("tex/skydome/sun.png", true, true, GL_CLAMP_TO_EDGE);
	auto texSkyMoon = new Texture("tex/skydome/moon.png", true, true, GL_CLAMP_TO_EDGE);

	// models
	auto mSkydomeSphere = new Model("sphere.obj");
	
	// ----------- skydome ------------

	auto skydome = new Skydome(shaderSky, mSkydomeSphere, camera);
	skydome->setTexture(texSkyTint, "tint");
	skydome->setTexture(texSkyTint2, "tint2");
	skydome->setTexture(texSkyMoon, "moon");
	skydome->setTexture(texSkySun, "sun");

	// ----- --------------- -----

	updateFunc = [=](double t)
	{
		// sun position
		vec3 sunPos = vec3(1, 0, 0);
		const vec3 sunRotAx = vec3(0, 1, 1);
		const vec3 starRotAx = vec3(1, 1, 0);

		mat3 rotMat = glm::rotate(mat4(), static_cast<float>(t * 0.0001), sunRotAx);
		sunPos = rotMat * sunPos;

		skydome->uploadUniform(sunPos, "sun_pos");
		skydome->uploadUniform(mat3(glm::rotate(mat4(), static_cast<float>(t * 0.00001), starRotAx)), "rot_stars");

		skydome->render();
	};

	renderFunc = [=]()
	{
		skydome->render();
	};
		
	cleanUpFunc = [=]()
	{
		shaderSky->cleanUp();
		delete shaderSky;

		texSkyTint->freeImage();
		texSkyTint2->freeImage();
		texSkySun->freeImage();
		texSkyMoon->freeImage();
		delete texSkyTint;
		delete texSkyTint2;
		delete texSkySun;
		delete texSkyMoon;

		mSkydomeSphere->cleanUp();
		delete mSkydomeSphere;

		delete skydome;
	};
}
