#ifndef Renderer_h
#define Renderer_h

// necessary

//#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include <functional>

// ~necessary

#include "controller.h"

class Renderer
{
public:
	Renderer()
		: width{ 0 }
		, height{ 0 }
		, updateFunc{ [](double) {} }
		, cleanUpFunc{ []() {} }
		, controller{ nullptr }
	{}
	Renderer(const GLint width, const GLint height)
		: width{ width }
		, height{ height }
		, updateFunc{ [](double) {} }
		, cleanUpFunc{ []() {} }
		, controller{ nullptr }
	{}
	Renderer(Controller* _controller)
		: width{ 0 }
		, height{ 0 }
		, updateFunc{ [](double) {} }
		, cleanUpFunc{ []() {} }
		, controller{ _controller }
	{}
	Renderer(const GLint width, const GLint height, Controller* _controller)
		: width{ width }
		, height{ height }
		, updateFunc{ [](double) {} }
		, cleanUpFunc{ []() {} }
		, controller{ _controller }
	{}
	~Renderer() 
	{ 
		cleanUpFunc(); 
	}
	void update(double t)
	{
		updateFunc(t);
	}

	virtual void init() = 0;

protected:
	const GLint width;
	const GLint height;
	std::function<void(double)> updateFunc;
	std::function<void()> cleanUpFunc;
	Controller* controller;
};

#endif // !Renderer_h

