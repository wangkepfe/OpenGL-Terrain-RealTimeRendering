#ifndef cameraTest_H
#define cameraTest_H

#include "Renderer.h"

#include "Camera.hpp"
#include "frustum.hpp"

class CameraTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override;
	std::function<Camera*()> getCamera;
	std::function<ViewFrustum*()> getViewFrustum;
};

#endif // !cameraTest_H