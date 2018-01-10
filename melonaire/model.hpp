//
//  model.hpp
//  melonaire
//
//  Created by Wang Ke on 2017-05-23.
//  Copyright © 2017 Wang Ke. All rights reserved.
//

#ifndef model_hpp
#define model_hpp

//#define GLEW_STATIC
#include <gl/glew.h>

#include <vector>
#include <string>
#include <fstream>

#include "glm/glm.hpp"

#include "Resource.hpp"

using glm::vec3;
using glm::vec2;

//the attributes per vertex
struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;

	vec3 tangent;
	vec3 bitangent;
};
    
class Model
{
public:
	Model() {}
	Model(const std::string filename) { loadObjFile("resource/" + filename); }
	Model(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices)
		:vertices{ vertices }
		, indices{ indices }
	{
		computeTangentSpace();
		conversionToPointerArray();
		bindToGL();
	}
	~Model()
	{}

	//model attributes
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	//interface: load & draw
	void loadObjFile(const std::string filename);
	void drawModel(int shader);
	void drawModelOnlyPosition(int shader);
	void cleanUp()
	{
		if (vertexPositionArray != nullptr) delete[] vertexPositionArray;
		if (vertexNormalArray != nullptr)  delete[] vertexNormalArray;
		if (vertexTexCoordArray != nullptr) delete[] vertexTexCoordArray;
		if (vertexTangentArray != nullptr) delete[] vertexTangentArray;
		if (vertexBitangentArray != nullptr) delete[] vertexBitangentArray;
		if (vertexIndexArray != nullptr) delete[] vertexIndexArray;

		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &nbo);
		glDeleteBuffers(1, &tbo);
		glDeleteBuffers(1, &tvbo);
		glDeleteBuffers(1, &bvbo);
		glDeleteBuffers(1, &ebo);
	}

protected:
	void generateVertex(std::vector<Vertex>& oVerts,
		const std::vector<vec3>& iPositions,
		const std::vector<vec2>& iTCoords,
		const std::vector<vec3>& iNormals,
		std::string icurline);

	void vertexTriangluation(std::vector<unsigned int>& oIndices,
		const std::vector<Vertex>& iVerts);

	void computeTangentSpace();

	//opengl objects
	void conversionToPointerArray();
	void bindToGL();

	unsigned int vao, vbo, nbo, tbo, tvbo, bvbo, ebo;

	GLfloat* vertexPositionArray{ nullptr };
	GLfloat* vertexNormalArray{ nullptr };
	GLfloat* vertexTexCoordArray{ nullptr };
	GLfloat* vertexTangentArray{ nullptr };
	GLfloat* vertexBitangentArray{ nullptr };
	GLuint* vertexIndexArray{ nullptr };
};

#endif /* model_hpp */
