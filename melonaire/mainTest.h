#ifndef mainTest_h
#define mainTest_h

#include "Renderer.h"
#include <vector>

class MainTest : public Renderer
{
public:
	using Renderer::Renderer;
	void init() override;
private:
	std::vector<Renderer*> testList;
	std::vector<Renderer*>::iterator currentTestIterator;
};

#endif // mainTest_h