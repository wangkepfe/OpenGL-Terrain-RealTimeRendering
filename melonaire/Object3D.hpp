#ifndef Object3D_hpp
#define Object3D_hpp

#include "glObject.hpp"
#include "Camera.hpp"

#include "AABB.hpp"
using CPM_GLM_AABB_NS::AABB;

#include "frustum.hpp"

#include "glm/gtc/matrix_transform.hpp"

class Object3D : public OpenGLObject
{
public:
	Object3D()
		: pos{ vec3(0.f ,0.f, 0.f) }
		, scal{ vec3(1.f, 1.f, 1.f) }
		, rotAx{ vec3(0.f, 1.f, 0.f) }
		, anchorAx{ vec3(0.f ,0.f, 0.f) }
		, rotRd{ 0.f }
		, projectionMatrix{ mat4() }
		, viewMatrix{ mat4() }
		, modelMatrix{ mat4() }
		, mvpMatrix{ mat4() }
		, camera{ nullptr }
	{}

	Object3D(Camera* camera)
		: pos{ vec3(0.f ,0.f, 0.f) }
		, scal{ vec3(1.f, 1.f, 1.f) }
		, rotAx{ vec3(0.f, 1.f, 0.f) }
		, anchorAx{ vec3(0.f ,0.f, 0.f) }
		, rotRd{ 0.f }
		, projectionMatrix{ mat4() }
		, viewMatrix{ mat4() }
		, modelMatrix{ mat4() }
		, mvpMatrix{ mat4() }
		, camera{ camera }
	{}

	Object3D(Camera* camera, Shader* shader)
		: OpenGLObject(*shader)
		, pos{ vec3(0.f ,0.f, 0.f) }
		, scal{ vec3(1.f, 1.f, 1.f) }
		, rotAx{ vec3(0.f, 1.f, 0.f) }
		, anchorAx{ vec3(0.f ,0.f, 0.f) }
		, rotRd{ 0.f }
		, projectionMatrix{ mat4() }
		, viewMatrix{ mat4() }
		, modelMatrix{ mat4() }
		, mvpMatrix{ mat4() }
		, camera{ camera }
	{}

	~Object3D() {}

	// basic 3D info, for model matrix ( world coords )
	void setPos(const vec3& _pos) { pos = _pos; }
	void setScal(const vec3& _scal) { scal = _scal; }
	void setRotAx(const vec3& _rotAx) { rotAx = _rotAx; }
	void setRotRd(GLfloat _rotRd) { rotRd = _rotRd; }
	void setRotAnchorAx(const vec3& _anchorAx) {
		anchorAx = _anchorAx;
	}

	vec3 getPos() { return pos; }
	vec3 getScal() { return scal; }
	vec3 getRotAx() { return rotAx; }
	GLfloat getRotRd() { return rotRd; }

	void setAABB(const AABB& _aabb) { aabb = _aabb; }
	AABB getAABB() { return aabb; }

	void setViewFrustum(ViewFrustum* _frus) { frus = _frus; }
	ViewFrustum* getViewFrustum() { return frus; }

	// ---- matrices ----
	void updateModelMatrix()
	{
		modelMatrix = mat4();
		if (pos != vec3(0.f, 0.f, 0.f))//translate last
		{
			modelMatrix = glm::translate(modelMatrix, pos);
		}
		if (rotRd != 0.f && rotAx != vec3(0.f, 0.f, 0.f))//rotate second
		{
			if (anchorAx != vec3(0.f, 0.f, 0.f))
			{
				modelMatrix = glm::translate(modelMatrix, anchorAx);
				modelMatrix = glm::rotate(modelMatrix, rotRd, rotAx);
				modelMatrix = glm::translate(modelMatrix, -anchorAx);
			}
			else
			{
				modelMatrix = glm::rotate(modelMatrix, rotRd, rotAx);
			}
		}
		if (scal != vec3(1, 1, 1))//scale first
		{
			modelMatrix = glm::scale(modelMatrix, scal);
		}
	}
	void setModelMatrix(const mat4& _modelMatrix) { modelMatrix = _modelMatrix; }
	void setProjectionMatrix(const mat4& _projectionMatrix) { projectionMatrix = _projectionMatrix; }
	void setViewMatrix(const mat4& _viewMatrix) { viewMatrix = _viewMatrix; }

	mat4 getModelMatrix() const { return modelMatrix; }
	mat4 getViewMatrix() const { return viewMatrix; }
	mat4 getProjectionMatrix() const { return projectionMatrix; }

	// -- camera (recommanded) --
	void setCamera(Camera* _camera)
	{
		camera = _camera;
		projectionMatrix = camera->getProjectionMatrix();
	}
	Camera* getCamera() const { return camera; }

	void updateProjMatrixFromCamera() {
		projectionMatrix = camera->getProjectionMatrix();
	}

	void updateViewMatrixFromCamera() {
		camera->updateViewMatrix();
		viewMatrix = camera->getViewMatrix();
	}

	void uploadViewPositionFromCamera(const std::string& nameInShader = "viewPos") {
		uploadUniform(camera->getPos(), nameInShader);
	}

	// ---- common shader utils func ----

	// for fragment position: 
	// fragPos = vec3(modelMatrix * vec4(inPosition, 1.0));
	void uploadModelMatrix(const std::string& nameInShader = "modelMatrix") const
	{
		uploadUniform(modelMatrix, nameInShader);
	}

	// for normal vector:
	// normal = normalMatrix * inNormal;
	// mTangent = normalMatrix * inTangent;
	// mBitangent = normalMatrix * inBitangent;
	void uploadNormalMatrix(const std::string& nameInShader = "normalMatrix") const
	{
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelMatrix)));
		uploadUniform(normalMatrix, nameInShader);
	}

	// for gl_position:
	// gl_Position = mvp * vec4(inPosition, 1.0);
	void uploadMVPmatrix(const std::string& nameInShader = "mvp")
	{
		mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
		uploadUniform(mvpMatrix, nameInShader);
	}

	void uploadVPmatrix(const std::string& nameInShader = "vp")
	{
		mvpMatrix = projectionMatrix * viewMatrix;
		uploadUniform(mvpMatrix, nameInShader);
	}

protected:
	// pos scal rot
	vec3 pos;
	vec3 scal;
	vec3 rotAx;
	vec3 anchorAx;
	GLfloat rotRd;

	// matrices
	mat4 projectionMatrix, viewMatrix, modelMatrix, mvpMatrix;

	// camera
	Camera* camera;

	// AABB
	AABB aabb;

	//ViewFrustum
	ViewFrustum* frus;
};



#endif // !Object3D_hpp

