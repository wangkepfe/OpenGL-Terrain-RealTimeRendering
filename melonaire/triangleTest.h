#ifndef triangleTest_H
#define triangleTest_H

#include "Renderer.h"

class TriangleTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override;
};

#endif // !triangleTest_H

