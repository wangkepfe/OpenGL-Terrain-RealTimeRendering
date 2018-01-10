#include "controller.h"

#include <SDL.h>

#include "glm/glm.hpp"
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

#include "glm/gtc/matrix_transform.hpp"

void Controller::update()
{
	// keyboard state
	const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

	// ------- step 1: [w s a d] horizontal movement -------

	vec3 moveDir = vec3(0.f, 0.f, 0.f);
	vec3 moveVelocity = vec3(0.f, 0.f, 0.f);

	// viewDir no y component
	vec3 viewDir = camLookat - camPos;
	viewDir.y = 0;
	viewDir = glm::normalize(viewDir);

	// independent w s a d control, summing up, normalize
	if (keyboardState[SDL_SCANCODE_W])
	{
		moveDir += viewDir;
	}
	if (keyboardState[SDL_SCANCODE_S])
	{
		moveDir -= viewDir;
	}
	if (keyboardState[SDL_SCANCODE_A])
	{
		moveDir -= glm::cross(viewDir, vec3(0, 1, 0));
	}
	if (keyboardState[SDL_SCANCODE_D])
	{
		moveDir += glm::cross(viewDir, vec3(0, 1, 0));
	}

	if (moveDir != vec3(0, 0, 0))
	{
		moveDir = glm::normalize(moveDir);
		moveVelocity += moveDir * moveSpeed;
	}

	// ------- step 2: [space lshift] vertical movement -------

	moveDir = vec3(0.f, 0.f, 0.f);

	// move rules: free move
	if (keyboardState[SDL_SCANCODE_LSHIFT])
	{
		moveDir += vec3(0, -1, 0);
	}
	if (keyboardState[SDL_SCANCODE_SPACE])
	{
		moveDir += vec3(0, 1, 0);
	}

	if (moveDir != vec3(0, 0, 0))
	{
		moveVelocity += moveDir * moveSpeed;
	}

	camPos += moveVelocity;
	camLookat += moveVelocity;

	// ------- step 3: [mouse moving] head spinning -------

	// mouse button state
	int x, y;
	auto mouseState = SDL_GetMouseState(&x, &y);
	bool leftButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
	mouseMoving(x, y);

	// ------- step 4: [mouse picking] callback -------
	
	if (leftButtonState && lastLeftButtonState != leftButtonState)
	{
		onClickLButton();
	}

	lastLeftButtonState = leftButtonState;
}

void Controller::mouseMoving(int x, int y)
{
	if (oldx == x && oldy == y)
		return;

	if (x > width - 5 || x < 5 || y > height - 5 || y < 5)
	{
		warpPointer(width / 2, height / 2);
		oldx = width / 2;
		oldy = height / 2;
		return;
	}

	yawAngle += -(y - oldy) * 0.004f;
	pitchAngle += -(x - oldx) * 0.003f;

	oldx = x;
	oldy = y;

	if (yawAngle > 1.5f)
		yawAngle = 1.5f;
	if (yawAngle < -1.5f)
		yawAngle = -1.5f;

	const vec3 lookat = vec3(1, 0, 0);
	const vec3 right = vec3(0, 0, 1);
	const vec3 up = vec3(0, 1, 0);

	mat4 rotMat = mat4();
	rotMat = glm::rotate(rotMat, pitchAngle, up);
	rotMat = glm::rotate(rotMat, yawAngle, right);

	camLookat = camPos + vec3(rotMat * vec4(lookat, 0));
}
