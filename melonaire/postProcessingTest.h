#ifndef postProcessingTest_H
#define postProcessingTest_H

#include "Renderer.h"

class PostProcessingTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	void init(const std::function<void(double t)>&);
};

#endif // !postProcessingTest_H