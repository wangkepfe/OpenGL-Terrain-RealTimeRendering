#include <iostream>

#include <SDL.h>

//#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL_opengl.h>

#include "mainRender.hpp"
#include "controller.h"
#include "main.h"

#include <time.h>

static GLint width = 1080, height = 720;
static const GLint middleX = width / 2, middleY = height / 2;

int main(int argc, char *argv[])
{
	srand(time(NULL));

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Rect r;
	SDL_GetDisplayBounds(0, &r);
	width = r.w;
	height = r.h - 40;

	SDL_Window *window = SDL_CreateWindow("OpenGL",0, 0, width, height, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//// show cursor or not
	SDL_ShowCursor(SDL_DISABLE);
	//SDL_ShowCursor(SDL_ENABLE);

	glClearColor(0.2, 0.3, 0.7, 1.0);
	glViewport(0, 0, width, height);

	SDL_Event windowEvent;

	Controller controller(width, height);
	MainRenderer mainRender(width, height, &controller);

	controller.warpPointerFunc([window](int x, int y) {
		SDL_WarpMouseInWindow(window, x, y);
	});

	SDL_WarpMouseInWindow(window, width / 2, height / 2);
	
	mainRender.init();

	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (SDL_QUIT == windowEvent.type)
				break;
		}

		controller.update();

		mainRender.update(SDL_GetTicks());

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}