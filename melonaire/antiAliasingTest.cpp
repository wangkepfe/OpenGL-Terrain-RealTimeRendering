#include "antiAliasingTest.h"

#include "msaa.hpp"

void AntiAliasingTest::init(const std::function<void(double t)>& renderFunc)
{
	// shader
	auto shaderPostProcessing = new Shader("postprocess.vert", "postprocessSimple.frag");
	//auto shaderPostProcessing = new Shader("postprocess.vert", "postprocessContrast.frag");

	// msaa with pp quad
	auto msaa = new MultisampledAntialiasing(width, height, shaderPostProcessing);
	msaa->create();
	msaa->setRenderFunc(renderFunc);

	updateFunc = [=](double t)
	{
		msaa->render(t);
	};

	cleanUpFunc = [=]()
	{
		shaderPostProcessing->cleanUp();
		delete shaderPostProcessing;

		msaa->cleanUp();
		delete msaa;
	};
}
