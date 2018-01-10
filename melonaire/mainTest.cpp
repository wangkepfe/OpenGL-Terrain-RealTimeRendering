#include "mainTest.h"

#include "glm/glm.hpp"
using glm::vec3;

#include "triangleTest.h"
#include "simpleObjTest.h"
#include "ShadowMappingTest.h"

void MainTest::init()
{
	// new tests
	auto triangleTest = new TriangleTest(width, height, controller);
	auto simpleObjTest = new SimpleObjTest(width, height, controller);
	auto shadowMappingTest = new ShadowMappingTest(width, height, controller);

	// camera
	auto camera = new PerspectiveCamera(60.f, width / (float)height, 0.2f, 2000.f);
	camera->setUp(vec3(0, 1, 0));

	// init
	triangleTest->init();
	simpleObjTest->init(camera);
	shadowMappingTest->init(camera);

	// controller
	controller->init(vec3(0, 0, 0));
	controller->setMovingSpeed(0.01f);

	// test list
	//testList.push_back(triangleTest);
	//testList.push_back(simpleObjTest);
	testList.push_back(shadowMappingTest);
	currentTestIterator = testList.begin();

	// controller click callback: switch test
	controller->setOnClickLeftButton([=]() {
		if(++currentTestIterator == testList.end())
			currentTestIterator = testList.begin();
	});

	updateFunc = [=](double t)
	{
		camera->setPos(controller->getCamPos());
		camera->setLookAt(controller->getCamLookAt());

		auto currentTest = *currentTestIterator;
		currentTest->update(t);
	};

	cleanUpFunc = [=]()
	{
		delete triangleTest;
		delete simpleObjTest;
		delete shadowMappingTest;
	};
}
