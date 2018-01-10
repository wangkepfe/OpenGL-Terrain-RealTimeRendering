#ifndef Resource_hpp
#define Resource_hpp

#include "glm/glm.hpp"
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

class Resource
{
public:
	Resource() {}
	virtual ~Resource() {}
	virtual void upload(GLuint shader, const std::string& nameInShader, const GLuint number) = 0;
};

#endif
