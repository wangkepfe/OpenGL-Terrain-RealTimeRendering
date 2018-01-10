#include "Terrain.h"

#include "libnoise/noise.h"

#include "glm/gtx/normal.hpp"

// ----- public functions -----

void Terrain::generate()
{
	generateHeight();
	generateNormal();
	generateGeomipmap();
	generateChunk();

	for (int i{ 0 }; i < mapSize * mapSize; i++)
		updateChunk(i);
}

void Terrain::updateTerrain()
{
	const int eachFrameUpdateChunkNum = 1;

	// from 0 to chunkSize - 1 , update each chunk
	int i, j;

	for (i = 0; i < eachFrameUpdateChunkNum; i++)
	{
		chunkTimer++;
		if (chunkTimer >= chunkUpdatePeriod)
			chunkTimer = 0;

		int chunkTemp = mapSize * mapSize * chunkTimer / chunkUpdatePeriod;

		updateChunk(chunkTemp);
	}
}

void Terrain::render()
{
	int i, j;
	for (i = 0; i < mapSize; i++)
	{
		for (j = 0; j < mapSize; j++)
		{
			chunk[i][j]->render();
		}
	}
}

void Terrain::renderForShadowMap(Shader shader)
{
	int i, j;
	OpenGLObject globj(shader);
	for (i = 0; i < mapSize; i++)
	{
		for (j = 0; j < mapSize; j++)
		{
			chunk[i][j]->shadowMappingRender(shader);
		}
	}
}


float Terrain::getHeight(float x, float z)
{
	if (x > borderXZMax.x || x < borderXZMin.x || z > borderXZMax.y || z < borderXZMin.y)
		return 0.f;

	// step 0: de-transform, de-scaling

	x -= offsetPos.x;
	z -= offsetPos.z;

	x /= scaleXZ;
	z /= scaleXZ;

	// step 1: quads

	int i{ static_cast<int>(x) };
	int j{ static_cast<int>(z) };
	
	// step 2: triangles

	vec3 A, B, C;

	if (x - i < z - j)
	{
		A = getPoint(i, j);
		B = getPoint(i, j + 1);
		C = getPoint(i + 1, j + 1);
	}
	else
	{
		A = getPoint(i, j);
		B = getPoint(i + 1, j + 1);
		C = getPoint(i + 1, j);
	}

	// step 3: plane equation

	vec3 n = glm::triangleNormal(A, B, C);
	float d = -glm::dot(n, A);
	float y = -(n.x * x + n.z * z + d) / n.y;

	// step last: scale up

	return y * scaleY;
}

vec3 Terrain::getNormal(float x, float z)
{
	if (x > borderXZMax.x || x < borderXZMin.x || z > borderXZMax.y || z < borderXZMin.y)
		return vec3(0, 1, 0);

	// step 0: de-transform, de-scaling

	x -= offsetPos.x;
	z -= offsetPos.z;

	x /= scaleXZ;
	z /= scaleXZ;

	// step 1: quads

	int i{ static_cast<int>(x) };
	int j{ static_cast<int>(z) };

	// step 2: triangles

	vec3 A, B, C;
	float lam1, lam2;

	if (x - i < z - j)
	{
		A = NormalData[i][j];
		B = NormalData[i + 1][j + 1];
		C = NormalData[i][j + 1];

		lam1 = j + 1 - z;
		lam2 = x - i;	
	}
	else
	{
		A = NormalData[i][j];
		B = NormalData[i + 1][j + 1];
		C = NormalData[i + 1][j];

		lam1 = i + 1 - x;
		lam2 = z - j;
	}

	// step 3: Barycentric interpolation

	return lam1 * A + lam2 * B + (1 - lam1 - lam2) * C;
}

vec3 Terrain::getNormalFlat(float x, float z)
{
	if (x > borderXZMax.x || x < borderXZMin.x || z > borderXZMax.y || z < borderXZMin.y)
		return vec3(0, 1, 0);

	// step 0: de-transform, de-scaling

	x -= offsetPos.x;
	z -= offsetPos.z;

	x /= scaleXZ;
	z /= scaleXZ;

	// step 1: quads

	int i{ static_cast<int>(x) };
	int j{ static_cast<int>(z) };

	// step 2: triangles

	vec3 A, B, C;

	if (x - i < z - j)
	{
		A = getPoint(i, j);
		B = getPoint(i, j + 1);
		C = getPoint(i + 1, j + 1);
	}
	else
	{
		A = getPoint(i, j);
		B = getPoint(i + 1, j + 1);
		C = getPoint(i + 1, j);
	}

	return glm::triangleNormal(A, B, C);
}

float Terrain::getSlopeInDegree(float x, float z)
{
	vec3 n = getNormal(x, z);
	float rad = glm::dot(n, vec3(0, 1, 0));
	return 90.f - glm::degrees(rad);
}

float Terrain::getSlopeInDegreeFlat(float x, float z)
{
	vec3 n = getNormalFlat(x, z);
	float rad = glm::dot(n, vec3(0, 1, 0));
	return 90.f - glm::degrees(rad);
}

void Terrain::cleanUp()
{
	int i, j, lod;
	for (i = 0; i < mapSize; i++)
	{
		for (j = 0; j < mapSize; j++)
		{
			// delete old model
			auto oldModel = chunk[i][j]->getModel();
			oldModel->cleanUp();
			delete oldModel;

			delete chunk[i][j];
		}
	}
}

// ----- private functions -----

void Terrain::generateHeight()
{
	noise::module::Perlin noiseModule;
	double x, y, z;
	int i, j;

	noiseModule.SetSeed(noiseSeed);
	noiseModule.SetFrequency(1.0);

	// fill data[][] with perlin noise
	z = 1;
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			x = i * resolution;
			y = j * resolution;

			data[i][j] = noiseModule.GetValue(x, y, z);
		}
	}
}

void Terrain::generateNormal()
{
	int i, j; 

	std::vector<  std::vector<  std::array< vec3, 2 >  >  > 
		triangleNormal(size, std::vector<  std::array< vec3, 2 >  >(size));

	// each triangle's normal
	for (i = 0; i < size - 1; i++)
	{
		for (j = 0; j < size - 1; j++)
		{
			const vec3 p1 = getPoint(i, j);
			const vec3 p2 = getPoint(i, j + 1);
			const vec3 p3 = getPoint(i + 1, j + 1);
			const vec3 p4 = getPoint(i + 1, j);

			triangleNormal[i][j][0] = glm::triangleNormal(p1, p2, p3);
			triangleNormal[i][j][1] = glm::triangleNormal(p1, p3, p4);
		}
	}

	// each point's normal
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			vec3 normalSum = vec3(0, 0, 0);

			if (i > 0 && j > 0)
			{
				normalSum += triangleNormal[i-1][j-1][0] + triangleNormal[i-1][j-1][1];
			}

			if (i < size - 1 && j < size - 1)
			{
				normalSum += triangleNormal[i][j][0] + triangleNormal[i][j][1];
			}

			if (i > 0 && j < size - 1)
			{
				normalSum += triangleNormal[i - 1][j][0] * 2.f;
			}

			if (i < size - 1 && j > 0)
			{
				normalSum += triangleNormal[i][j - 1][0] * 2.f;
			}

			NormalData[i][j] = glm::normalize(normalSum);
		}
	}
}

void Terrain::generateGeomipmap()
{
	int i, j, lod, m, n;

	Vertex vertex;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// mapSize * mapSize of chunks, each chunk could have different LOD
	for (i = 0; i < mapSize; i++)
	{
		for (j = 0; j < mapSize; j++)
		{

			// create 5 different LOD chunks for each chunk position
			for (lod = 0; lod < 5; lod++)
			{
				vertices.clear();
				indices.clear();

				// for LOD 0, 1, 2, 3, 4 ; the step in the chunk are 1, 2, 4, 8, 16
				int step = glm::pow(2, lod);

				// for inside each chunk, which is 17 * 17 vertices (16 * 16 quads)
				//    create the vertiex array
				for (m = 0; m < chunkSize + 1; m += step)
				{
					for (n = 0; n < chunkSize + 1; n += step)
					{
						// vertex
						int x = i * chunkSize + m;
						int y = j * chunkSize + n;

						vertex.position = getPoint(x, y);
						vertex.normal = NormalData[x][y];
						vertex.texCoord = vec2(x, y) * textureScale / (float)size;

						vertices.push_back(vertex);

						// index
						if (m < chunkSize && n < chunkSize)
						{
							int rowLen = chunkSize / step + 1;
							int mm = m / step;
							int nn = n / step;
							int currentIndex = mm * rowLen + nn;
							
							indices.push_back(currentIndex);
							indices.push_back(currentIndex + 1);
							indices.push_back(currentIndex + rowLen + 1);
							indices.push_back(currentIndex);
							indices.push_back(currentIndex + rowLen + 1);
							indices.push_back(currentIndex + rowLen);
						}
					}
				}

				geomipmapVertices[i][j][lod] = vertices;
				geomipmapIndices[i][j][lod] = indices;

			}
		}
	}
}

void Terrain::generateChunk()
{
	int i, j;

	for (i = 0; i < mapSize; i++)
	{
		for (j = 0; j < mapSize; j++)
		{
			chunk[i][j] = new ModelObject3D(new Model());

			chunk[i][j]->setShader(shader);
			chunk[i][j]->setCamera(camera);
			for (const auto& tex : resourceList)
			{
				chunk[i][j]->setTexture(tex.second, tex.first);
			}

			chunk[i][j]->setPos(offsetPos);
			chunk[i][j]->setScal(vec3(scaleXZ, scaleY, scaleXZ));
			

			chunk[i][j]->updateModelMatrix();

			chunk[i][j]->setViewFrustum(frus);

			// AABB
			vec3 aabbMin = getPoint(i * chunkSize, j * chunkSize);
			vec3 aabbMax = getPoint((i + 1) * chunkSize, (j + 1) * chunkSize);

			aabbMin.y = -1;
			aabbMax.y = 1;

			chunk[i][j]->setAABB(AABB(aabbMin, aabbMax));
		}
	}
}

void Terrain::updateChunk(int timer)
{
	int i, j, m;

	// ----- determine LOD map -----

	const float mipmapBaseDistance = mipmapBase * scaleXZ;

	for (i = 0; i < mapSize; i++)
	{
		for (j = 0; j < mapSize; j++)
		{
			// each chunk's center point's position to camPos 's distance
			vec3 chunkCenterPoint = chunk[i][j]->getModelMatrix() * vec4(getPoint(i * chunkSize + chunkSize / 2, j * chunkSize + chunkSize / 2), 1);

			float distance = glm::distance(camera->getPos(), chunkCenterPoint);

			int lod = 0;
			while (lod < 4 && distance > glm::pow(2, lod) * mipmapBaseDistance)
				lod++;

			lodMap[i][j] = lod;
		}
	}

	// ----- build up chunks , stitch borders -----

	i = timer / mapSize;
	j = timer % mapSize;

	//  fetch verts and indexs

	auto vertices = geomipmapVertices[i][j][lodMap[i][j]];
	auto indices = geomipmapIndices[i][j][lodMap[i][j]];


	// morph the border
	int step = glm::pow(2, lodMap[i][j]);
	int blockSize = chunkSize / step;
	int offSet;

	if (i > 0 && lodMap[i - 1][j] > lodMap[i][j])
	{
		// up border
		offSet = 0;
		for (m = 0; m < blockSize; m += 2)
		{
			indices[offSet + m * 6 + 1] += 1;

			indices[offSet + m * 6 + 6] = 0;
			indices[offSet + m * 6 + 7] = 0;
			indices[offSet + m * 6 + 8] = 0;

			indices[offSet + m * 6 + 9] += 1;
		}
	}

	if (i < mapSize - 1 && lodMap[i + 1][j] > lodMap[i][j])
	{
		// down border
		offSet = blockSize * (blockSize - 1) * 6;
		for (m = 0; m < blockSize; m += 2)
		{
			indices[offSet + m * 6 + 2] -= 1;

			indices[offSet + m * 6 + 3] = 0;
			indices[offSet + m * 6 + 4] = 0;
			indices[offSet + m * 6 + 5] = 0;

			indices[offSet + m * 6 + 11] -= 1;
		}
	}

	if (j > 0 && lodMap[i][j - 1] > lodMap[i][j])
	{
		// left border
		offSet = 0;
		for (m = 0; m < blockSize; m += 2)
		{
			indices[offSet + m * 6 * blockSize + 5] += blockSize + 1;
			indices[offSet + (m + 1) * 6 * blockSize] += blockSize + 1;

			indices[offSet + (m + 1) * 6 * blockSize + 3] = 0;
			indices[offSet + (m + 1) * 6 * blockSize + 4] = 0;
			indices[offSet + (m + 1) * 6 * blockSize + 5] = 0;
		}
	}

	if (j < mapSize - 1 && lodMap[i][j + 1] > lodMap[i][j])
	{
		// right border
		offSet = 6 * (blockSize - 1);
		for (m = 0; m < blockSize; m += 2)
		{
			indices[offSet + m * 6 * blockSize + 0] = 0;
			indices[offSet + m * 6 * blockSize + 1] = 0;
			indices[offSet + m * 6 * blockSize + 2] = 0;

			indices[offSet + m * 6 * blockSize + 4] -= blockSize + 1;
			indices[offSet + (m + 1) * 6 * blockSize + 1] -= blockSize + 1;
		}
	}

	// delete old model

	auto oldModel = chunk[i][j]->getModel();
	oldModel->cleanUp();
	delete oldModel;


	// use new model

	chunk[i][j]->setModel(new Model(vertices, indices));
}
