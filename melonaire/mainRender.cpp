#include "mainRender.hpp"

#include "mainSceneTest.h"
#include "mainTest.h"

#define TEST_MODE 0

void MainRenderer::init()
{
#if !TEST_MODE
	auto mainSceneTest = new MainSceneTest(width, height, controller);

	mainSceneTest->init();

	updateFunc = [=](double t)
	{
		mainSceneTest->update(t);
	};

	cleanUpFunc = [=]()
	{
		delete mainSceneTest;
	};
#else
	auto mainTest = new MainTest(width, height, controller);

	mainTest->init();

	updateFunc = [=](double t)
	{
		mainTest->update(t);
	};

	cleanUpFunc = [=]()
	{
		delete mainTest;
	};
#endif
}
