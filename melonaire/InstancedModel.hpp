#ifndef InstancedModel_hpp
#define InstancedModel_hpp

#include "model.hpp"

using glm::mat4;
using glm::vec4;

class InstancedModel : public Model
{
public:
	using Model::Model;

	void drawInstancedModel(int shader, mat4* modelMatrices, int amount)
	{
		glUseProgram(shader);

		GLint loc;

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		loc = glGetAttribLocation(shader, "inPosition");
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);

		glBindBuffer(GL_ARRAY_BUFFER, nbo);
		loc = glGetAttribLocation(shader, "inNormal");
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);

		glBindBuffer(GL_ARRAY_BUFFER, tvbo);
		loc = glGetAttribLocation(shader, "inTangent");
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);

		glBindBuffer(GL_ARRAY_BUFFER, bvbo);
		loc = glGetAttribLocation(shader, "inBitangent");
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);

		glBindBuffer(GL_ARRAY_BUFFER, tbo);
		loc = glGetAttribLocation(shader, "inTexCoord");
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);

		// instance vbo

		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(mat4), &modelMatrices[0], GL_STATIC_DRAW);
		loc = glGetAttribLocation(shader, "instanceModelMatrix");
		for (int i = 0; i < 4; i++)
		{
			// Set up the vertex attribute
			glVertexAttribPointer(loc + i,              // Location
				4, GL_FLOAT, GL_FALSE,       // vec4
				sizeof(mat4),                // Stride
				(void *)(sizeof(vec4) * i)); // Start offset
											 // Enable it
			glEnableVertexAttribArray(loc + i);
			// Make it instanced
			glVertexAttribDivisor(loc + i, 1);
		}

		// draw elements instanced

		glDrawElementsInstanced(GL_TRIANGLES, int(indices.size()), GL_UNSIGNED_INT, 0, amount);
	}

	void drawInstancedModelOnlyPosition(int shader, mat4* modelMatrices, int amount)
	{
		glUseProgram(shader);

		GLint loc;

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		loc = glGetAttribLocation(shader, "inPosition");
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);

		// instance vbo

		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(mat4), &modelMatrices[0], GL_STATIC_DRAW);
		loc = glGetAttribLocation(shader, "instanceModelMatrix");
		for (int i = 0; i < 4; i++)
		{
			// Set up the vertex attribute
			glVertexAttribPointer(loc + i,              // Location
				4, GL_FLOAT, GL_FALSE,       // vec4
				sizeof(mat4),                // Stride
				(void *)(sizeof(vec4) * i)); // Start offset
											 // Enable it
			glEnableVertexAttribArray(loc + i);
			// Make it instanced
			glVertexAttribDivisor(loc + i, 1);
		}

		// draw elements instanced

		glDrawElementsInstanced(GL_TRIANGLES, int(indices.size()), GL_UNSIGNED_INT, 0, amount);
	}

private:
	unsigned int instanceVBO;
};

#endif // !InstancedModel_hpp

