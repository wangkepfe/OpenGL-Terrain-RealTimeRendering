#ifndef skyTest_H
#define skyTest_H

#include "Renderer.h"
#include "Camera.hpp"

class SkyTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	void init(Camera* camera);
	std::function<void()> renderFunc;
};

#endif // !skyTest_H