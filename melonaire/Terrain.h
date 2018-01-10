#ifndef Terrain_h
#define Terrain_h

#include <vector>
#include <array>

#include "ModelObject3D.hpp"

class Terrain : public OpenGLObject
{
public:
	Terrain(unsigned int size, unsigned int chunkSize)
		: size{ size + 1 }
		, chunkSize{ chunkSize }
		, mapSize{ size / chunkSize }

		, data(size + 1, std::vector<float>(size + 1))
		, NormalData(size + 1, std::vector<vec3>(size + 1))

		, geomipmapVertices(mapSize,
			std::vector<std::vector<std::vector<Vertex>>>(mapSize,
				std::vector<std::vector<Vertex>>(5,
					std::vector<Vertex>())))
		, geomipmapIndices(mapSize,
			std::vector<std::vector<std::vector<unsigned int>>>(mapSize,
				std::vector<std::vector<unsigned int>>(5,
					std::vector<unsigned int>())))

		, chunk(mapSize, std::vector< ModelObject3D* >(mapSize))

		, lodMap(mapSize, std::vector <int>(mapSize))

		, chunkTimer{ 0 }
		, chunkUpdatePeriod{ size }

		, resolution{ 0.85f }
		, scaleXZ{ 1 }
		, scaleY{ 1 }
		, offsetPos{ vec3(0, 0, 0) }
		, textureScale{ 1 }

		, mipmapBase{ 20.0f }
		, noiseSeed{ 25 }

		, borderXZMax{ vec2(size, size) }
		, borderXZMin{ vec2(0, 0) }

		, frus{ nullptr }
	{}
	~Terrain(){}

	// 3D object general

	void setCamera(Camera* _camera) {
		camera = _camera;
	}

	void setTexture(Texture* tex, const std::string& nameInShader){
		resourceList[nameInShader] = tex;
	}

	void setNoiseSeed(int seed) { noiseSeed = seed; }

	// terrain general method

	void generate();

	void updateTerrain();
	void render();
	void renderForShadowMap(Shader shader);

	void cleanUp();

	// get infos from terrain

	float getHeight(float x, float z);

	vec3 getNormal(float x, float z);
	vec3 getNormalFlat(float x, float z);

	float getSlopeInDegree(float x, float z);
	float getSlopeInDegreeFlat(float x, float z);

	// params setting

	void setResolution(double res) { resolution = res; }
	void setScaleXZ(float scal) { 
		scaleXZ = scal; 
		borderXZMax *= scal;
		borderXZMin *= scal;
	}
	void setScaleY(float scal) { scaleY = scal; }
	void setPosOffset(const vec3& offset) { 
		offsetPos = offset; 
		borderXZMax += vec2(offset.x, offset.z);
		borderXZMin += vec2(offset.x, offset.z);
	}
	void setTextureScale(float scal) { textureScale = scal; }

	vec3 getPosOffset() { return offsetPos; }
	vec3 getPosOffsetXZ() { 
		auto offsetPosXZ = offsetPos;
		offsetPosXZ.y = 0;
		return offsetPosXZ;
	}

	void centralize(float yOffSet = 0.0f) {
		vec3 v(1.f, 0, 1.f);
		v *=  - scaleXZ * (size / 2);
		v.y = yOffSet;
		setPosOffset(v);
	}


	void setViewFrustum(ViewFrustum* _frus) { frus = _frus; }

	void setMipmapBase(float base) { mipmapBase = base; }

	void setRotation(const vec3& anchorAx, const vec3& rotAx, float rotRd)
	{
		int i, j;

		for (i = 0; i < mapSize; i++)
		{
			for (j = 0; j < mapSize; j++)
			{
				chunk[i][j]->setRotAx(rotAx);
				chunk[i][j]->setRotRd(rotRd);
				chunk[i][j]->setRotAnchorAx(anchorAx);
				//chunk[i][j]->updateModelMatrix();
			}
		}
	}

private:

	// terrain is a size ¡Á size square
	// choose 2 square number + 1 for size! easy for mip mapping
	const unsigned int size;
	const unsigned int chunkSize;
	const unsigned int mapSize;

	// ----- data -----

	// terrain height data
	std::vector<  std::vector<  float  >  > data;

	// normal data
	std::vector<  std::vector<  vec3  >  > NormalData;

	// geomipmap model
	std::vector<std::vector<std::vector<std::vector<Vertex>>>> geomipmapVertices;
	std::vector<std::vector<std::vector<std::vector<unsigned int>>>> geomipmapIndices;

	// model object
	std::vector<std::vector< ModelObject3D* >> chunk;

	std::vector< std::vector <int >> lodMap;

	int chunkTimer;
	unsigned int chunkUpdatePeriod;

	// ----- controlable params -----

	// level of details
	double resolution;

	// scale of terrain, original Y is from -1 to 1
	float scaleXZ;
	float scaleY;

	// position offset, original position is from (0,0) to (size,size)
	vec3 offsetPos;

	// texture scale
	float textureScale;

	// ---------

	float mipmapBase;

	int noiseSeed;

	// ---------

	Camera* camera;
	std::unordered_map<std::string, Resource*> resourceList;

	// height generation
	void generateHeight();

	// normal generation (smooth method)
	void generateNormal();

	// geomipmap generation
	void generateGeomipmap();

	// chunk generation
	void generateChunk();

	//
	void updateChunk(int timer);

	
	

	// ----- utils -----

	// border
	vec2 borderXZMax;
	vec2 borderXZMin;

	vec3 getPoint(int i, int j) { return vec3(i, data[i][j], j); }

	//ViewFrustum
	ViewFrustum* frus;
};

#endif // !Terrain_h





