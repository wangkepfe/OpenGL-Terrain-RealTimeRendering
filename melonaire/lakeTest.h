#ifndef lakeTest_H
#define lakeTest_H

#include "Renderer.h"

#include "Camera.hpp"
#include "frustum.hpp"
#include "lightingSystem.hpp"

class LakeTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	void init(Camera * , ViewFrustum * , LightingSystem * , const std::function<void(const vec4&)>& , const std::function<void(const vec4&)>&, float);
	std::function<void()> renderFunc;
};

#endif // !lakeTest_H