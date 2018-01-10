//
//  model.cpp
//  melonaire
//
//  Created by Wang Ke on 2017-05-23.
//  Copyright © 2017 Wang Ke. All rights reserved.
//

#include "model.hpp"

using glm::vec3;
using glm::vec2;

namespace algorithm
{
	inline float angleBetween(const vec3& a, const vec3& b)
	{
		float angle = glm::dot(a, b);
		angle /= glm::length(a) * glm::length(b);
		return angle = acosf(angle);
	}

	inline bool inTriangle(const vec3& D, const vec3 &A, const vec3 &B, const vec3 &C)
	{
		// Starting vars
		vec3 u = B - A;
		vec3 v = C - A;
		vec3 w = D - A;
		vec3 n = glm::cross(u, v);

		float y = glm::dot(glm::cross(u, w), n) / glm::dot(n, n);
		float b = glm::dot(glm::cross(w, v), n) / glm::dot(n, n);
		float a = 1 - y - b;

		// Projected point
		// vec3  p = (a * A) + (y * B) + (b * C);

		if (a >= 0 && a <= 1
			&& b >= 0 && b <= 1
			&& y >= 0 && y <= 1)
			return true;
		else
			return false;
	}

	// Split a String into a string array at a given token
	inline void split(const std::string &in,
		std::vector<std::string> &out,
		std::string token)
	{
		out.clear();

		std::string temp;

		for (int i = 0; i < int(in.size()); i++)
		{
			std::string test = in.substr(i, token.size());

			if (test == token)
			{
				if (!temp.empty())
				{
					out.push_back(temp);
					temp.clear();
					i += (int)token.size() - 1;
				}
				else
				{
					out.push_back("");
				}
			}
			else if (i + token.size() >= in.size())
			{
				temp += in.substr(i, token.size());
				out.push_back(temp);
				break;
			}
			else
			{
				temp += in[i];
			}
		}
	}

	// Get tail of string after first token and possibly following spaces
	inline std::string tail(const std::string &in)
	{
		size_t token_start = in.find_first_not_of(" \t");
		size_t space_start = in.find_first_of(" \t", token_start);
		size_t tail_start = in.find_first_not_of(" \t", space_start);
		size_t tail_end = in.find_last_not_of(" \t");
		if (tail_start != std::string::npos && tail_end != std::string::npos)
		{
			return in.substr(tail_start, tail_end - tail_start + 1);
		}
		else if (tail_start != std::string::npos)
		{
			return in.substr(tail_start);
		}
		return "";
	}

	// Get first token of string
	inline std::string firstToken(const std::string &in)
	{
		if (!in.empty())
		{
			size_t token_start = in.find_first_not_of(" \t");
			size_t token_end = in.find_first_of(" \t", token_start);
			if (token_start != std::string::npos && token_end != std::string::npos)
			{
				return in.substr(token_start, token_end - token_start);
			}
			else if (token_start != std::string::npos)
			{
				return in.substr(token_start);
			}
		}
		return "";
	}

	// Get element at given index position
	template <class T>
	inline const T & getElement(const std::vector<T> &elements, std::string &index)
	{
		int idx = std::stoi(index);
		if (idx < 0)
			idx = int(elements.size()) + idx;
		else
			idx--;
		return elements[idx];
	}
}

void Model::generateVertex(std::vector<Vertex>& oVerts,
                            const std::vector<vec3>& iPositions,
                            const std::vector<vec2>& iTCoords,
                            const std::vector<vec3>& iNormals,
                            std::string icurline)
{
    std::vector<std::string> sface, svert;
    Vertex vVert;
    algorithm::split(algorithm::tail(icurline), sface, " ");
        
    // For every given vertex do this
    for (int i = 0; i < int(sface.size()); i++)
    {
        algorithm::split(sface[i], svert, "/");
            
        if(svert.size() == 1)
        {
            if(svert[0] == "\r")
                continue;
            vVert.position = algorithm::getElement(iPositions, svert[0]);
            oVerts.push_back(vVert);
        }
        else
        {
            if(svert[0] == "\r")
                continue;
            // Calculate and store the vertex
            vVert.position = algorithm::getElement(iPositions, svert[0]);
            vVert.texCoord = algorithm::getElement(iTCoords, svert[1]);
            vVert.normal = algorithm::getElement(iNormals, svert[2]);
            oVerts.push_back(vVert);
        }
    }
}
    
// Triangulate a list of vertices into a face by printing
//	inducies corresponding with triangles within it
void Model::vertexTriangluation(std::vector<unsigned int>& oIndices,
                                const std::vector<Vertex>& iVerts)
{
    // If there are 2 or less verts,
    // no triangle can be created,
    // so exit
    if (iVerts.size() < 3)
    {
        return;
    }
    // If it is a triangle no need to calculate it
    if (iVerts.size() == 3)
    {
        oIndices.push_back(0);
        oIndices.push_back(1);
        oIndices.push_back(2);
        return;
    }
        
    // Create a list of vertices
    std::vector<Vertex> tVerts = iVerts;
        
    while (true)
    {
        // For every vertex
        for (int i = 0; i < int(tVerts.size()); i++)
        {
            // pPrev = the previous vertex in the list
            Vertex pPrev;
            if (i == 0)
            {
                pPrev = tVerts[tVerts.size() - 1];
            }
            else
            {
                pPrev = tVerts[i - 1];
            }
                
            // pCur = the current vertex;
            Vertex pCur = tVerts[i];
                
            // pNext = the next vertex in the list
            Vertex pNext;
            if (i == tVerts.size() - 1)
            {
                pNext = tVerts[0];
            }
            else
            {
                pNext = tVerts[i + 1];
            }
                
            // Check to see if there are only 3 verts left
            // if so this is the last triangle
            if (tVerts.size() == 3)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(tVerts.size()); j++)
                {
                    if (iVerts[j].position == pCur.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pPrev.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pNext.position)
                        oIndices.push_back(j);
                }
                    
                tVerts.clear();
                break;
            }
            if (tVerts.size() == 4)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(iVerts.size()); j++)
                {
                    if (iVerts[j].position == pCur.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pPrev.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pNext.position)
                        oIndices.push_back(j);
                }
                    
                vec3 tempVec;
                for (int j = 0; j < int(tVerts.size()); j++)
                {
                    if (tVerts[j].position != pCur.position
                        && tVerts[j].position != pPrev.position
                        && tVerts[j].position != pNext.position)
                    {
                        tempVec = tVerts[j].position;
                        break;
                    }
                }
                    
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(iVerts.size()); j++)
                {
                    if (iVerts[j].position == pPrev.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pNext.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == tempVec)
                        oIndices.push_back(j);
                }
                    
                tVerts.clear();
                break;
            }
                
            // If Vertex is not an interior vertex
            float angle = algorithm::angleBetween(pPrev.position - pCur.position, pNext.position - pCur.position) * (180 / 3.1415926f);
            if (angle <= 0 && angle >= 180)
                continue;
                
            // If any vertices are within this triangle
            bool inTri = false;
            for (int j = 0; j < int(iVerts.size()); j++)
            {
                if (algorithm::inTriangle(iVerts[j].position, pPrev.position, pCur.position, pNext.position)
                    && iVerts[j].position != pPrev.position
                    && iVerts[j].position != pCur.position
                    && iVerts[j].position != pNext.position)
                {
                    inTri = true;
                    break;
                }
            }
            if (inTri)
                continue;
                
            // Create a triangle from pCur, pPrev, pNext
            for (int j = 0; j < int(iVerts.size()); j++)
            {
                if (iVerts[j].position == pCur.position)
                    oIndices.push_back(j);
                if (iVerts[j].position == pPrev.position)
                    oIndices.push_back(j);
                if (iVerts[j].position == pNext.position)
                    oIndices.push_back(j);
            }
                
            // Delete pCur from the list
            for (int j = 0; j < int(tVerts.size()); j++)
            {
                if (tVerts[j].position == pCur.position)
                {
                    tVerts.erase(tVerts.begin() + j);
                    break;
                }
            }
                
            // reset i to the start
            // -1 since loop will add 1 to it
            i = -1;
        }
            
        // if no triangles were created
        if (oIndices.size() == 0)
            break;
            
        // if no more vertices
        if (tVerts.size() == 0)
            break;
    }
}
    
void Model::computeTangentSpace()
{
    std::vector<vec3> tangents, bitangents;
    for(int i = 0; i < indices.size(); i+=3)
    {
        // Shortcuts for vertices
        const vec3 & v0 = vertices[indices[i+0]].position;
        const vec3 & v1 = vertices[indices[i+1]].position;
        const vec3 & v2 = vertices[indices[i+2]].position;
            
        // Shortcuts for UVs
        const vec2 & uv0 = vertices[indices[i+0]].texCoord;
        const vec2 & uv1 = vertices[indices[i+1]].texCoord;
        const vec2 & uv2 = vertices[indices[i+2]].texCoord;
            
        // Edges of the triangle : postion delta
        vec3 deltaPos1 = v1-v0;
        vec3 deltaPos2 = v2-v0;
            
        // UV delta
        vec2 deltaUV1 = uv1-uv0;
        vec2 deltaUV2 = uv2-uv0;
            
        // calculation
        float r = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
		if (r > 0.01f || r < -0.01f)
			r = 1.0f / r;
		else
			continue;

        vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
        vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;
            
        //add
        vertices[indices[i+0]].tangent += tangent;
        vertices[indices[i+1]].tangent += tangent;
        vertices[indices[i+2]].tangent += tangent;
            
        vertices[indices[i+0]].bitangent += bitangent;
        vertices[indices[i+1]].bitangent += bitangent;
        vertices[indices[i+2]].bitangent += bitangent;
    }
        
	/*
    for(int i = 0; i < indices.size(); i++)
    {
		vertices[indices[i]].tangent = glm::normalize(vertices[indices[i]].tangent);
		vertices[indices[i]].bitangent = glm::normalize(vertices[indices[i]].tangent);
    }
	*/
}
    
void Model::loadObjFile(const std::string Path)
{
    std::vector<vec3> Positions;
    std::vector<vec2> TCoords;
    std::vector<vec3> Normals;
        
    // *** open file ***
    if (Path.substr(Path.size() - 4, 4) != ".obj")
        return;
        
    std::ifstream file(Path);
        
    if (!file.is_open())
        return;
        
    // *** read file ***
    std::string curline;
    while (std::getline(file, curline))
    {
        // Generate a Vertex Position
        if (algorithm::firstToken(curline) == "v")
        {
            std::vector<std::string> spos;
            vec3 vpos;
            algorithm::split(algorithm::tail(curline), spos, " ");
                
            vpos.x = std::stof(spos[0]);
            vpos.y = std::stof(spos[1]);
            vpos.z = std::stof(spos[2]);
                
            Positions.push_back(vpos);
        }
        // Generate a Vertex Texture Coordinate
        if (algorithm::firstToken(curline) == "vt")
        {
            std::vector<std::string> stex;
            vec2 vtex;
            algorithm::split(algorithm::tail(curline), stex, " ");
                
            vtex.x = std::stof(stex[0]);
            vtex.y = std::stof(stex[1]);
                
            TCoords.push_back(vtex);
        }
        // Generate a Vertex Normal;
        if (algorithm::firstToken(curline) == "vn")
        {
            std::vector<std::string> snor;
            vec3 vnor;
            algorithm::split(algorithm::tail(curline), snor, " ");
                
            vnor.x = std::stof(snor[0]);
            vnor.y = std::stof(snor[1]);
            vnor.z = std::stof(snor[2]);
                
            Normals.push_back(vnor);
        }
        // Generate a Face (vertices & indices)
        if (algorithm::firstToken(curline) == "f")
        {
            std::vector<Vertex> vVerts;
            generateVertex(vVerts, Positions, TCoords, Normals, curline);
                
            for (int i = 0; i < int(vVerts.size()); i++)
            {
                vertices.push_back(vVerts[i]);
            }
                
            std::vector<unsigned int> iIndices;
            vertexTriangluation(iIndices, vVerts);
                
            for (int i = 0; i < int(iIndices.size()); i++)
            {
                unsigned int indnum = (unsigned int)((vertices.size()) - vVerts.size()) + iIndices[i];
                indices.push_back(indnum);
            }
        }
    }
    file.close();
        
    // *** tangent & bitangent ***
    computeTangentSpace();
        
    // *** openGL buffer objects ***
    conversionToPointerArray();
    bindToGL();
}
    
void Model::bindToGL()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &nbo);
    glGenBuffers(1, &tbo);
    glGenBuffers(1, &tvbo);
    glGenBuffers(1, &bvbo);
    glGenBuffers(1, &ebo);
        
    glBindVertexArray(vao);
        
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*sizeof(GLfloat), vertexPositionArray, GL_STATIC_DRAW);
        
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*sizeof(GLfloat), vertexNormalArray, GL_STATIC_DRAW);
        
    glBindBuffer(GL_ARRAY_BUFFER, tvbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*sizeof(GLfloat), vertexTangentArray, GL_STATIC_DRAW);
        
    glBindBuffer(GL_ARRAY_BUFFER, bvbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*sizeof(GLfloat), vertexBitangentArray, GL_STATIC_DRAW);
        
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*2*sizeof(GLfloat), vertexTexCoordArray, GL_STATIC_DRAW);
        
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), vertexIndexArray, GL_STATIC_DRAW);
}
    
void Model::drawModel(int shader)
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
        
    glDrawElements(GL_TRIANGLES, int(indices.size()), GL_UNSIGNED_INT, 0L);
}

void Model::drawModelOnlyPosition(int shader)
{
	glUseProgram(shader);

	GLint loc;

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	loc = glGetAttribLocation(shader, "inPosition");
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc);

	glDrawElements(GL_TRIANGLES, int(indices.size()), GL_UNSIGNED_INT, 0L);
}
    
void Model::conversionToPointerArray()
{
	auto verticeNumber = vertices.size();
	vertexPositionArray = new GLfloat[verticeNumber * 3];
	vertexNormalArray = new GLfloat[verticeNumber * 3];
	vertexTangentArray = new GLfloat[verticeNumber * 3];
	vertexBitangentArray = new GLfloat[verticeNumber * 3];
	vertexTexCoordArray = new GLfloat[verticeNumber * 2];
	vertexIndexArray = new GLuint[indices.size()];

	int i{ 0 }, j{ 0 };
	for (const auto& obj : vertices)
	{
		vertexPositionArray[i + 0] = obj.position.x;
		vertexPositionArray[i + 1] = obj.position.y;
		vertexPositionArray[i + 2] = obj.position.z;

		vertexNormalArray[i + 0] = obj.normal.x;
		vertexNormalArray[i + 1] = obj.normal.y;
		vertexNormalArray[i + 2] = obj.normal.z;

		vertexTangentArray[i + 0] = obj.tangent.x;
		vertexTangentArray[i + 1] = obj.tangent.y;
		vertexTangentArray[i + 2] = obj.tangent.z;

		vertexBitangentArray[i + 0] = obj.bitangent.x;
		vertexBitangentArray[i + 1] = obj.bitangent.y;
		vertexBitangentArray[i + 2] = obj.bitangent.z;

		vertexTexCoordArray[j + 0] = obj.texCoord.x;
		vertexTexCoordArray[j + 1] = obj.texCoord.y;

		i += 3;
		j += 2;
	}

	i = 0;
	for (const auto& obj : indices)
	{
		vertexIndexArray[i++] = obj;
	}
}

















