#ifndef shadowMappingTest_H
#define shadowMappingTest_H

#include "Renderer.h"
#include "Camera.hpp"

class ShadowMappingTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {}
	void init(Camera* camera);
};

#endif // !shadowMappingTest_H

