#ifndef lightingSystem_hpp
#define lightingSystem_hpp

#include <string>
#include <vector>
#include <unordered_map>

//#define GL_STATIC
#include <gl/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
using glm::vec3;

#define MAX_LIGHT_NUM 10

#define AMBIENT_LIGHT_TYPE 0
#define POINT_LIGHT_TYPE 1
#define DIRECTIONAL_LIGHT_TYPE 2
#define SPOT_LIGHT_TYPE 3

class LightingSystem;

class Light {
	friend class LightingSystem;
protected:
	Light() {}
	virtual ~Light() {}

	virtual vec3 getColor() const { return vec3(1, 0, 0); }
	virtual vec3 getPos() const { return vec3(0, 0, 0); }
	virtual vec3 getDirec() const { return vec3(1, 0, 0); }
	virtual float getAngle() const { return 60.f; }
	virtual int getType() const = 0;
};

class AmbientLight : public Light {
public:
	AmbientLight(){}
	AmbientLight(const vec3& color) :color{ color } {}
	~AmbientLight(){}

	void setColor(const vec3& _color) { color = _color; }
	vec3 getColor() const override { return color; }

	int getType() const override { return AMBIENT_LIGHT_TYPE; }
protected:
	vec3 color;
};

class PointLight : public AmbientLight {
public:
	PointLight() {}
	PointLight(const vec3& color, const vec3& pos)
		: AmbientLight(color)
		, pos{ pos } {}
	~PointLight() {}

	void setPos(const vec3& _pos) { pos = _pos; }
	vec3 getPos() const override { return pos; }

	int getType() const override { return POINT_LIGHT_TYPE; }
protected:
	vec3 pos;
};

class DirectionalLight : public AmbientLight {
public:
	DirectionalLight() {}
	DirectionalLight(const vec3& color, const vec3& direc)
		: AmbientLight(color)
		, direc{ direc } {}
	~DirectionalLight() {}

	void setDirec(const vec3& _direc) { direc = _direc; }
	vec3 getDirec() const override { return direc; }

	int getType() const override { return DIRECTIONAL_LIGHT_TYPE; }
protected:
	vec3 direc;
};

class SpotLight : public PointLight {
public:
	SpotLight() {}
	SpotLight(const vec3& color, const vec3& pos, const vec3& direc, float angle)
		: PointLight(color, pos)
		, direc{ direc } 
		, angle{ angle }
	{}
	~SpotLight() {}

	void setDirec(const vec3& _direc) { direc = _direc; }
	vec3 getDirec() const override { return direc; }

	void setAngle(float _angle) { angle = _angle; }
	float getAngle() const override { return angle; }

	int getType() const override { return SPOT_LIGHT_TYPE; }
protected:
	vec3 direc;

	// half angle in degree
	float angle;
};

class LightingSystem
{
public:
	LightingSystem() {}
	~LightingSystem() {}

	// shader upload utils

	void bindShader(const GLuint shader)
	{
		shaderList.push_back(shader);
	}

	void unbindShader(const GLuint shader)
	{
		auto it = find(begin(shaderList), end(shaderList), shader);
		if (it != end(shaderList))
			shaderList.erase(it);
	}

	void upload() const
	{
		for (auto shader : shaderList)
		{
			glUseProgram(shader);
			glUniform1i(glGetUniformLocation(shader, "lightNum"), lightNum);
			glUniform1iv(glGetUniformLocation(shader, "lightType"), lightNum, lightType);
			glUniform3fv(glGetUniformLocation(shader, "lightPos"), lightNum, glm::value_ptr(lightPos[0]));
			glUniform3fv(glGetUniformLocation(shader, "lightDir"), lightNum, glm::value_ptr(lightDir[0]));
			glUniform3fv(glGetUniformLocation(shader, "lightColor"), lightNum, glm::value_ptr(lightColor[0]));
		}
	}

	void uploadShadowMapping(GLuint tex, const std::string& texName, const GLuint number, const glm::mat4& m, const std::string& matrixName) const
	{
		for (auto shader : shaderList)
		{
			glUseProgram(shader);

			glActiveTexture(GL_TEXTURE0 + number);
			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(glGetUniformLocation(shader, texName.c_str()), number);

			glUniformMatrix4fv(glGetUniformLocation(shader, matrixName.c_str()), 1, GL_FALSE, &m[0][0]);
		}
	}

	// lights managment

	void addLight(Light* light, const std::string& name)
	{
		lightList[name] = light;
		update();
	}

	void removeLight(const std::string& name)
	{
		lightList.erase(name);
		update();
	}

	Light* getLight(const std::string& name) const
	{
		return lightList.at(name);
	}

	void update()
	{
		lightNum = lightList.size();

		int i{ 0 };

		for (auto obj : lightList)
		{
			auto light = obj.second;
			lightType[i] = light->getType();
			lightPos[i] = light->getPos();
			lightDir[i] = light->getDirec();
			lightColor[i] = light->getColor();
			i++;
		}
	}

private:
	

	std::vector<GLuint> shaderList;
	std::unordered_map<std::string, Light*> lightList;

	int lightNum;
	int lightType[MAX_LIGHT_NUM];
	vec3 lightPos[MAX_LIGHT_NUM];
	vec3 lightDir[MAX_LIGHT_NUM];
	vec3 lightColor[MAX_LIGHT_NUM];
};

#endif // !lightingSystem_hpp
