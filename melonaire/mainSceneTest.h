#ifndef mainSceneTest_h
#define mainSceneTest_h

#include "Renderer.h"

class MainSceneTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override;
};

#endif // mainSceneTest_h