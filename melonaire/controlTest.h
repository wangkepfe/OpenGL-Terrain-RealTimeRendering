#ifndef controlTest_H
#define controlTest_H

#include "Renderer.h"
#include "Camera.hpp"

class ControlTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	void init(Camera* camera);
};

#endif // !controlTest_H