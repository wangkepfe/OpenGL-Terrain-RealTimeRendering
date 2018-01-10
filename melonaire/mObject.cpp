#include "mObject.h"
#include "glm/gtc/matrix_transform.hpp"

void mObject::setTexture(Texture * texture, const std::string& nameInShader)
{
	textures.push_back(std::pair<Texture *, std::string>(texture, nameInShader));
}

void mObject::setCubeMap(CubeMap* _cubeMap, const std::string& nameInShader)
{
	cubeMap = _cubeMap;
	cubeMapName = nameInShader;
}

void mObject::render()
{
	// shader
	glUseProgram(program);

	// mvp
	modelMatrix = mat4();
	if (pos != vec3())//translate last
	{
		modelMatrix = glm::translate(modelMatrix, pos);
	}
	if (rotRd != 0.f)//rotate second
	{
		modelMatrix = glm::rotate(modelMatrix, rotRd, rotAx);
	}
	if (scal != vec3(1.f, 1.f, 1.f))//scale first
	{
		modelMatrix = glm::scale(modelMatrix, scal);
	}
	
	using glm::mat3;

	mvpMatrix = projectionMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelMatrix)));
	glUniformMatrix3fv(glGetUniformLocation(program, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	// ** texs **
	int i{ 0 };

	// cube map
	if (cubeMap != nullptr)
	{
		//cubeMap->upload(program, GL_TEXTURE0, 0, cubeMapName);
		i++;
	}
	
	for (const auto& obj : textures)
	{
		auto tex = obj.first;
		const std::string name = obj.second;
		GLenum texEnum;

		switch (i)
		{
		case 0: texEnum = GL_TEXTURE0; break;
		case 1: texEnum = GL_TEXTURE1; break;
		case 2: texEnum = GL_TEXTURE2; break;
		case 3: texEnum = GL_TEXTURE3; break;
		case 4: texEnum = GL_TEXTURE4; break;
		}

		//tex->upload(program, texEnum, i, name);

		i++;
	}
	
	// draw model
	model->drawModel(program);
}
