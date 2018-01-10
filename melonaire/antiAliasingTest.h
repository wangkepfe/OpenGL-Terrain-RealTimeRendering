#ifndef antiAliasingTest_H
#define antiAliasingTest_H

#include "Renderer.h"

class AntiAliasingTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	void init(const std::function<void(double t)>&);
};

#endif // !antiAliasingTest_H