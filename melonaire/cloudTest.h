#ifndef cloudTest_H
#define cloudTest_H

#include "Renderer.h"

#include "Camera.hpp"
#include "frustum.hpp"
#include "lightingSystem.hpp"

class CloudTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	void init(Camera* camera, ViewFrustum* frus, LightingSystem* lightingSys);
	std::function<void()> renderFunc;
};

#endif // !cloudTest_H