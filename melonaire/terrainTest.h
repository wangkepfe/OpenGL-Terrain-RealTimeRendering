#ifndef terrainTest_H
#define terrainTest_H

#include "Renderer.h"

#include "Camera.hpp"
#include "frustum.hpp"
#include "lightingSystem.hpp"

#include "Terrain.h"

class TerrainTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	void init(Camera* camera, ViewFrustum* frus, LightingSystem* lightingSys);
	std::function<void(const vec4&)> renderClipFunc;
	std::function<void(const Shader&)> renderFuncForShadowMaping;
	std::function<Terrain*()> getTerrain;
};

#endif // !terrainTest_H