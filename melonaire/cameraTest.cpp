#include "cameraTest.h"

void CameraTest::init()
{
	auto camera = new PerspectiveCamera(60.f, width / (float)height, 0.2f, 2000.f);
	camera->setUp(vec3(0, 1, 0));

	auto frus = new ViewFrustum(60.f, width / (float)height, 0.2f, 2000.f);

	updateFunc = [=](double t)
	{
		frus->update(camera);
	};

	cleanUpFunc = [=]()
	{
		delete camera;
	};

	getCamera = [=]()->Camera*
	{
		return camera;
	};

	getViewFrustum = [=]()->ViewFrustum*
	{
		return frus;
	};
}
