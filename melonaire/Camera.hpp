#ifndef  Camera_hpp
#define  Camera_hpp 

//#define GLEW_STATIC
#include <gl/glew.h>

#include "glm/glm.hpp"
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

#include "glm/gtc/matrix_transform.hpp"

#include "glObject.hpp"

#include <vector>
#include <algorithm>

class Camera
{
public:
	Camera()
		: pos{ vec3() }
		, lookat{ vec3(1, 0 ,0) }
		, up{ vec3(0, 1, 0) }
		, viewMat{ glm::lookAt(pos, lookat, up) }
	{}

	Camera(const vec3& _pos, const vec3& _lookat, const vec3& _up)
		: pos{ _pos }
		, lookat{ _lookat }
		, up{ _up }
		, viewMat{ glm::lookAt(pos, lookat, up) }
	{}

	~Camera() {}

	void setPos(const vec3& _pos) { pos = _pos; }
	void setLookAt(const vec3& _lookat) { lookat = _lookat; }
	void setUp(const vec3& _up) { up = _up; }

	vec3 getPos() const { return pos; }
	vec3 getLookAt() const  { return lookat; }
	vec3 getUp() const { return up; }

	void setCamera(const vec3& _pos, const vec3& _lookat, const vec3& _up) {
		pos = _pos; 
		lookat = _lookat;
		up = _up;
		updateViewMatrix();
	}

	void updateViewMatrix() {
		viewMat = glm::lookAt(pos, lookat, up);
	}

	void setViewMatrix(const mat4& _viewMatrix) { viewMat = _viewMatrix; }
	mat4 getViewMatrix() const { return viewMat; }

	mat4 getViewMatrixLH() const { return glm::lookAtLH(pos, lookat, up); }

	virtual void setProjectionMatrix(const mat4&) {}
	virtual mat4 getProjectionMatrix() { return mat4(); }
	virtual mat4 getVPmatrix() { return mat4(); }

protected:
	vec3 pos, lookat, up;
	mat4 viewMat;
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera() 
		: Camera()
		, projMat{ mat4() }
	{}

	PerspectiveCamera(float fovyDegree, float aspect, float znear, float zfar)
		: Camera()
		, projMat{ glm::perspective(glm::radians(fovyDegree), aspect, znear, zfar) }
	{}

	~PerspectiveCamera() {}

	void setPerspective(float fovyDegree, float aspect, float znear, float zfar) {
		projMat = glm::perspective(glm::radians(fovyDegree), aspect, znear, zfar);
	}

	virtual void setProjectionMatrix(const mat4& _projectionMatrix) override { projMat = _projectionMatrix; }
	virtual mat4 getProjectionMatrix() override { return projMat; }
	virtual mat4 getVPmatrix() override { return projMat * viewMat; }

protected:
	mat4 projMat;
};

class OrthogonolCamera : public Camera
{
public:
	OrthogonolCamera()
	: Camera()
	, projMat{ mat4() }
	{}
	OrthogonolCamera(float width, float height, float znear, float zfar)
		: Camera()
		, projMat{ glm::ortho(-width / 2, width / 2, -height / 2, height / 2, znear, zfar) }
	{}
	~OrthogonolCamera() {}

	void setOrthogonol(float width, float height, float znear, float zfar) {
		projMat = glm::ortho(- width / 2, width / 2, - height / 2, height / 2, znear, zfar);
	}

	virtual void setProjectionMatrix(const mat4& _projectionMatrix) override { projMat = _projectionMatrix; }
	virtual mat4 getProjectionMatrix() override { return projMat; }
	virtual mat4 getVPmatrix() override { return projMat * viewMat; }

protected:
	mat4 projMat;
};

#endif // ! Camera_hpp

