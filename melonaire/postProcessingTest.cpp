#include "postProcessingTest.h"
#include "postprocess.hpp"

void PostProcessingTest::init(const std::function<void(double t)>& renderFunc)
{
	// shader
	auto shaderPostProcessing = new Shader("postprocess.vert", "postprocessSimple.frag");
	//auto shaderPostProcessing = new Shader("postprocess.vert", "postprocessContrast.frag");

	// postprocessquad
	auto ppquad = new PostProcess(width, height, shaderPostProcessing);
	ppquad->create();
	ppquad->setRenderFunc(renderFunc);
	
	updateFunc = [=](double t)
	{
		ppquad->render(t);
	};

	cleanUpFunc = [=]()
	{
		shaderPostProcessing->cleanUp();
		delete shaderPostProcessing;

		ppquad->cleanUp();
		delete ppquad;
	};
}
