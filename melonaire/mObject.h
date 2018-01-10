#ifndef mObject_h
#define mObject_h

#include <vector>
#include <utility>

#include "model.hpp"
#include "imageUtility.h"
#include "shaderUtility.hpp"

#include "glm/glm.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

class mObject
{
public:
	mObject()
		: pos{ vec3() }
		, scal{ vec3(1.f, 1.f, 1.f) }
		, rotAx{ vec3(0.f, 1.f, 0.f) }
		, rotRd{ 0.f }

		, projectionMatrix{ mat4() }
		, cameraMatrix{ mat4() }
		, modelMatrix{ mat4() }
		, mvpMatrix{ mat4() }

		, cubeMap{ nullptr }
	{

	}
	~mObject()
	{

	}

	// set essentials
	void setShaderProgram(const Shader& _program) { program = _program; }
	void setModel(Model* _model) { model = _model; }
	void setTexture(Texture* texture, const std::string& nameInShader);
	void setCubeMap(CubeMap* cubeMap, const std::string& nameInShader);

	// render
	virtual void render();

	// pos scal rot
	void setPosScalRot(const vec3& _pos, const vec3& _scal, const vec3& _rotAx, GLfloat _rotRd)
	{
		pos = _pos;
		scal = _scal;
		rotAx = _rotAx;
		rotRd = _rotRd;
	}
	void setPos(const vec3& _pos) { pos = _pos; }
	void setScal(const vec3& _scal) { scal = _scal; }
	void setRot(const vec3& _rotAx, GLfloat _rotRd) { rotAx = _rotAx; rotRd = _rotRd; }
	vec3 getPos() { return pos; }
	vec3 getScal() { return scal; }
	vec3 getRotAx() { return rotAx; }
	GLfloat getRotRd() { return rotRd; }

	// matrices
	void setProjectionMatrix(const mat4& _projectionMatrix) { projectionMatrix = _projectionMatrix; }
	void setCameraMatrix(const mat4& _cameraMatrix) { cameraMatrix = _cameraMatrix; }
	mat4 getCameraMatrix() { return cameraMatrix; }

protected:
	// pos scal rot
	vec3 pos, scal, rotAx;
	GLfloat rotRd;

	// matrices
	mat4 projectionMatrix, cameraMatrix, modelMatrix, mvpMatrix;

	// cube map
	CubeMap* cubeMap;
	std::string cubeMapName;

	// essential
	Shader program;
	Model* model;
	Texture* tex;

	// texture utility function
	std::vector<std::pair<Texture*, std::string>> textures;
};

class mGlowObject : public mObject
{
public:
	using mObject::mObject;
	void setGlowColor(const vec3& _color) { color = _color; }
	virtual void render() override
	{
		//program.uploadUniform(color, "glowColor");
		mObject::render();
	}
private:
	vec3 color;
};

#endif // !mObject_h

