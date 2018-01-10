#ifndef controller_h
#define controller_h

//#define GLEW_STATIC
#include <GL/glew.h>

#include <functional>

#include "glm/glm.hpp"

class Controller
{
public:
	Controller(GLint width, GLint height)
		: width{ width }
		, height{ height } 
		, oldx{ width / 2 }
		, oldy{ height / 2 }
		, yawAngle{ 0.0f }
		, pitchAngle{ 0.0f }
		, moveSpeed{ 0.1f }
		, onClickLButton{ [](){} }
	{}
	~Controller() {}

	// only used in main loop
	void update();

	// init camera(controlled object)
	void init(glm::vec3 _camPos) {
		camPos = _camPos;
	}

	// update camera
	glm::vec3 getCamPos() const{
		return camPos;
	}

	glm::vec3 getCamLookAt() const{
		return camLookat;
	}

	// moving speed
	void setMovingSpeed(float speed){
		moveSpeed = speed;
	}
	float getMovingSpeed() {
		return moveSpeed;
	}

	// set callback
	void setOnClickLeftButton(const std::function<void()>& _onClickLButton){
		onClickLButton = _onClickLButton;
	}

	void warpPointerFunc(const std::function<void(int, int)>& func){
		warpPointer = func;
	}

private:

	void mouseMoving(int x, int y);

	const GLint width;
	const GLint height;

	// for mouse controlled eye direction
	int oldx, oldy;
	float yawAngle, pitchAngle;

	// moving
	float moveSpeed;

	// mouse button callback
	std::function<void()> onClickLButton;

	// controlled object
	glm::vec3 camPos;
	glm::vec3 camLookat;	

	// warp pointer
	std::function<void(int, int)> warpPointer;

	// last Lbutton state
	bool lastLeftButtonState;
};

#endif // !controller_hpp

