#ifndef shadowMap_hpp
#define shadowMap_hpp

#include <functional>

#include "mfbo.hpp"
#include "Camera.hpp"
#include "shaderUtility.hpp"

#define SHADOW_WIDTH 2048
#define SHADOW_HEIGHT 2048

class ShadowMap
{
public:
	ShadowMap()
		: shadow_width{ SHADOW_WIDTH }
		, shadow_height{ SHADOW_HEIGHT }
		, fbo(shadow_width, shadow_height)
		, orthocamera{ nullptr }
	{}
	~ShadowMap() {}

	void create(Camera *);

	void cleanUp();

	void updateLightSourcePosition(vec3 dir);

	void render(const std::function<void(const Shader&)>& renderFunc, glm::mat4& vpMat, GLuint width, GLuint height);
	
	GLuint getTexture()
	{
		return fbo.getDepthTex();
	}

private:
	GLuint shadow_width, shadow_height;
	mFBO fbo;
	Camera* camera;
	OrthogonolCamera *orthocamera;
	Shader *shader;
};

#endif // !shadowMap_hpp
