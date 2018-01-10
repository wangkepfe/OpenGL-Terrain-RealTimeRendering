#ifndef plantTest_H
#define plantTest_H

#include "Renderer.h"

#include "Camera.hpp"
#include "frustum.hpp"
#include "lightingSystem.hpp"

#include "Terrain.h"

class PlantTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	void init(Camera* camera, ViewFrustum* frus, LightingSystem* lightingSys, Terrain*, float);
	std::function<void(const vec4&)> renderClipFunc;
};

#endif // !plantTest_H