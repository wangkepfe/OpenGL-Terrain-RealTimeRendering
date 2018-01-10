#include "simpleObjTest.h"

#include "shaderUtility.hpp"
#include "imageUtility.h"
#include "model.hpp"

#include "ModelObject3D.hpp"

void SimpleObjTest::init(Camera* camera)
{
	glEnable(GL_DEPTH_TEST);

	auto shader = new Shader("simple.vert", "simple.frag");

	auto tex1 = new Texture("tex/brickWall/albedo.png");
	auto tex2 = new Texture("tex/woodWall/albedo.png");
	auto tex3 = new Texture("tex/stoneWall/albedo.png");

	auto mdl1 = new Model("model/box.obj");
	auto mdl2 = new Model("model/ball.obj");
	auto mdl3 = new Model("model/plane.obj");

	auto obj1 = new ModelObject3D(shader, mdl1, camera);
	obj1->setTexture(tex1, "tex");
	obj1->setPos(vec3(5, -1, 0));
	obj1->setScal(vec3(1.0f, 1.0f, 1.0f));

	auto obj2 = new ModelObject3D(shader, mdl2, camera);
	obj2->setTexture(tex2, "tex");
	obj2->setPos(vec3(5, -1, 5));
	obj2->setScal(vec3(2.0f, 3.0f, 1.0f));

	auto obj3 = new ModelObject3D(shader, mdl3, camera);
	obj3->setTexture(tex3, "tex");
	obj3->setPos(vec3(0, -5, 0));
	obj3->setScal(vec3(10.0f, 1.0f, 10.0f));

	updateFunc = [=](double t)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		obj1->render(true, true, true, false);
		obj2->render(true, true, true, false);
		obj3->render(true, true, true, false);
	};

	cleanUpFunc = [=]()
	{
		shader->cleanUp();
		tex1->freeImage();
		mdl1->cleanUp();
		tex2->freeImage();
		mdl2->cleanUp();
		tex3->freeImage();
		mdl3->cleanUp();

		delete shader;
		delete tex1;
		delete mdl1;
		delete obj1;
		delete tex2;
		delete mdl2;
		delete obj2;
		delete tex3;
		delete mdl3;
		delete obj3;
	};
}
