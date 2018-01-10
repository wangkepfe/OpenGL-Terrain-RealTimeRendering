#ifndef lightingTest_H
#define lightingTest_H

#include "Renderer.h"
#include "Camera.hpp"

#include "lightingSystem.hpp"

class LightingTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {};
	LightingSystem* init(Camera* camera);
	void setShadowMapRenderFunc(const std::function<void(const Shader&)>&);
	/////
	std::function<GLuint()> getShadowMapTex;
	/////
private:
	std::function<void(const Shader&)> shadowMapRenderFunc;
};

#endif // !lightingTest_H