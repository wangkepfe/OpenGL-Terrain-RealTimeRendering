#include "controlTest.h"

void ControlTest::init(Camera* camera)
{
	controller->init(vec3(0, 0, 0));
	controller->setMovingSpeed(0.1f);

	updateFunc = [=](double t)
	{
		camera->setPos(controller->getCamPos());
		camera->setLookAt(controller->getCamLookAt());
	};

	cleanUpFunc = [=]()
	{

	};
}
