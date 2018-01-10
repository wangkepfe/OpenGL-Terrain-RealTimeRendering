#include "mainSceneTest.h"

#include "ModelObject3D.hpp"
#include "lightingSystem.hpp"

#include "cameraTest.h"
#include "controlTest.h"
#include "skyTest.h"
#include "lightingTest.h"
#include "cloudTest.h"
#include "terrainTest.h"
#include "lakeTest.h"
#include "plantTest.h"
#include "antiAliasingTest.h"

#include "screenQuad.hpp"

void MainSceneTest::init()
{
	/*

	What do i want in this?

	sky, (done)

	clouds, (done, still in doubt, good texture in need?)

	terrainLand, (done, not satisfied of geomipmapping)

	lake, (done, weird edge looking)

	plants, (done, without frustum culling)

	houses,

	humam, animals,

	...

	with

	Advanced OpenGL:

	Depth, Stencil testing, (in implemetation, really not easy)

	Blending,

	Face Culling, (not sure if in need)

	FrameBufferObjects, (done)

	Instancing (done in plants rendering) or particle sys,

	Texture filter and AntiAliasing

	and

	Advanced lighting:

	Normal mapping, (done)

	shadow mapping,

	HDR,

	Gamma,

	Bloom,

	SSAO,

	dynamic enviromental mapping, (tried and failed)

	*/


	/*

	1, If we are going to render a whole natural scenery, we need to render sky first, then terrain, then other stuffs

	2, Does clouds need to be well-shaped? Probably not.

	3, Terrain has different LOD. Implemented by dividing it into chunks.

	4, Lighting System. Everyone has a reference of lighting sys. or can anyone change lighting? like a moving color-changable torch

	5, Controller controls camera by user input. It takes camera's pointer, and maybe terrain and other obstacles' pointers.

	6, Rendering a lake is tricky. We need to render everything again (with low quality or LOD).
	This may redefine how we render the scene.
	So the render func of everything had better be a lambda?

	*/

	/*******************************************************************************************/

	glEnable(GL_DEPTH_TEST);

	// environment setting
	float waterLevel = -1.0f;

	// camera
	auto cameraTest = new CameraTest(width, height);
	cameraTest->init();
	auto camera = cameraTest->getCamera();
	auto frus = cameraTest->getViewFrustum();

	// controller
	auto controlTest = new ControlTest(controller);
	controlTest->init(camera);

	// sky
	auto skyTest = new SkyTest();
	skyTest->init(camera);

	// lighting system
	auto lightingTest = new LightingTest(width, height, controller);
	auto lightingSys = lightingTest->init(camera);

	// cloud
	auto cloudTest = new CloudTest();
	cloudTest->init(camera, frus, lightingSys);

	// terrain
	auto terrainTest = new TerrainTest(width, height, controller);
	terrainTest->init(camera, frus, lightingSys);
	auto terrain = terrainTest->getTerrain();

	// plant
	auto plantTest = new PlantTest();
	plantTest->init(camera, frus, lightingSys, terrain, waterLevel);

	// lake
	auto lakeTest = new LakeTest(width, height, controller);
	auto reflectionRenderFunc = [=](const vec4& clippingPlane) {
		skyTest->renderFunc();
		cloudTest->renderFunc();
		plantTest->renderClipFunc(clippingPlane);
		terrainTest->renderClipFunc(clippingPlane);
	};
	auto refractionRenderFunc = [=](const vec4& clippingPlane) {
		skyTest->renderFunc();
		plantTest->renderClipFunc(clippingPlane);
		terrainTest->renderClipFunc(clippingPlane);
	};
	lakeTest->init(camera, frus, lightingSys, reflectionRenderFunc, refractionRenderFunc, waterLevel);

	auto renderFunc = [=](double t) {
		skyTest->update(t);
		cloudTest->update(t);
		terrainTest->update(t);

		lakeTest->renderFunc();

		plantTest->update(t);
	};

	// multi-sampled anti-aliasing (post processing included)
	auto antiAliasingTest = new AntiAliasingTest(width, height, controller);
	antiAliasingTest->init(renderFunc);

	// shadow map render function
	auto shadowRenderFunc = [=](const Shader& shader) {
		terrainTest->renderFuncForShadowMaping(shader);
	};

	lightingTest->setShadowMapRenderFunc(shadowRenderFunc);

	//
	auto quad = new ScreenQuad();
	quad->setShader(Shader("postprocess.vert", "depthMap.frag"));
	quad->create();

	updateFunc = [=](double t)
	{
		controlTest->update(t);
		cameraTest->update(t);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		lightingTest->update(t);// shadow map render
		lakeTest->update(t);// pre render

		antiAliasingTest->update(t);
		//quad->uploadTexture(lightingTest->getShadowMapTex(),"depthMap",0);
		//quad->render();
	};

	cleanUpFunc = [=]()
	{
		delete cameraTest;
		delete controlTest;
		delete skyTest;
		delete lightingTest;
		delete cloudTest;
		delete terrainTest;
		delete plantTest;
		delete lakeTest;
		delete antiAliasingTest;
	};
}
