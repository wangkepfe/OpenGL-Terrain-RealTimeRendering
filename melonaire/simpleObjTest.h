#ifndef simpleObjTest_H
#define simpleObjTest_H

#include "Camera.hpp"
#include "Renderer.h"

class SimpleObjTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override {} 
	void init(Camera* camera);
};

#endif // !simpleObjTest_H