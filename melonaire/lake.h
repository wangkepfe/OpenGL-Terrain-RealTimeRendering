#ifndef lake_h
#define lake_h

#include <functional>

#include "screenQuad.hpp"
#include "mfbo.hpp"
#include "ModelObject3D.hpp"
#include "controller.h"

class Lake : public ScreenQuad
{
public:
	Lake(GLuint waterSamplewidth, GLuint waterSampleheight, Shader* shader)
		: reflectionFBO(waterSamplewidth, waterSampleheight)
		, refractionFBO(waterSamplewidth, waterSampleheight)
	{
		setShader(*shader);
	}
	~Lake(){}

	void cleanUp() override;

	// creation

	void create(Shader* planeShader, Camera* camera);

	void createFBO();
	void cleanFBO();
	void createPlane(Shader* planeShader, Camera* camera);

	// render

	bool renderToTexture(Controller* controller, Camera* camera);
	void render(Texture* texDUDV, Texture* texNormal);

	// access to member

	ModelObject3D* getWaterPlane()
	{
		return plane;
	}

	mFBO& getReflectionFBO()
	{
		return reflectionFBO;
	}

	mFBO& getRefractionFBO()
	{
		return refractionFBO;
	}

	void setReflectionRenderFunc(const std::function<void(const vec4&)>& _reflectionRenderFunc)
	{
		reflectionRenderFunc = _reflectionRenderFunc;
	}

	void setRefractionRenderFunc(const std::function<void(const vec4&)>& _refractionRenderFunc)
	{
		refractionRenderFunc = _refractionRenderFunc;
	}

	float getWaterHeight()
	{
		return plane->getPos().y;
	}

	// modify member

	void setPosScal(const vec3& pos, const vec3& scal)
	{
		plane->setPos(pos);
		plane->setScal(scal);
		plane->updateModelMatrix();
	}

	void setFrustum(ViewFrustum* frus)
	{
		plane->setViewFrustum(frus);
	}

private:
	mFBO reflectionFBO;
	mFBO refractionFBO;

	std::function<void(const vec4&)> reflectionRenderFunc;
	std::function<void(const vec4&)> refractionRenderFunc;

	ModelObject3D* plane;

	bool culled;
};


#endif // !lake_h

