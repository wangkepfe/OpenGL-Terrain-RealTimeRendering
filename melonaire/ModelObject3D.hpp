#ifndef ModelObject3D_hpp
#define ModelObject3D_hpp

#include <unordered_map>
#include <limits>

#include "Object3D.hpp"
#include "model.hpp"

class ModelObject3D : public Object3D
{
public:
	ModelObject3D()
		: Object3D()
		, model{ nullptr }
	{}

	ModelObject3D(Model* _model)
		: Object3D()
		, model{ _model }
	{}

	ModelObject3D(Model* _model, Camera* camera)
		: Object3D(camera)
		, model{ _model }
	{}

	ModelObject3D(Shader* shader, Model* _model, Camera* camera)
		: Object3D(camera, shader)
		, model{ _model }
	{}

	~ModelObject3D(){}

	// render
	virtual bool render(bool _uploadMatrices = true, bool _uploadCamera = true, bool _uploadTexture = true, bool _frustumCulling = true)
	{
		if (_frustumCulling)
		{
			updateModelMatrix();

			if (frus->cullingAABB(modelMatrix * aabb))
			{
				return false;
			}
		}
		else
		{
			updateModelMatrix();
		}

		if (_uploadCamera)
		{
			uploadViewPositionFromCamera();

			updateProjMatrixFromCamera();
			updateViewMatrixFromCamera();
		}

		if (_uploadMatrices)
		{
			uploadModelMatrix();
			uploadNormalMatrix();
			uploadMVPmatrix();
		}

		if (_uploadTexture)
			uploadTextures();

		model->drawModel(shader);

		return true;
	}

	void shadowMappingRender(Shader shadowMappingDepthShader)
	{
		auto oldShader = getShader();
		setShader(shadowMappingDepthShader);

		updateModelMatrix();
		uploadModelMatrix();

		//model->drawModelOnlyPosition(shadowMappingDepthShader);
		model->drawModel(shadowMappingDepthShader);

		setShader(oldShader);
	}

	// model
	void setModel(Model*_model)
	{
		model = _model;
	}
	Model* getModel() const
	{
		return model;
	}

	void updateAABBfromModel()
	{
		const float floatMin{ std::numeric_limits<float>::min() };
		const float floatMax{ std::numeric_limits<float>::max() };
		float xmin{ floatMax }, xmax{ floatMax }, ymin{ floatMax }, ymax{ floatMin }, zmin{ floatMin }, zmax{ floatMin };
		for (const auto& vertex : model->vertices)
		{
			if (vertex.position.x < xmin)
				xmin = vertex.position.x;

			if (vertex.position.y < ymin)
				ymin = vertex.position.y;

			if (vertex.position.z < zmin)
				zmin = vertex.position.z;

			if (vertex.position.x > xmax)
				xmax = vertex.position.x;

			if (vertex.position.y > ymax)
				ymax = vertex.position.y;

			if (vertex.position.z > zmax)
				zmax = vertex.position.z;
		}

		aabb = AABB(vec3(xmin, ymin, zmin), vec3(xmax, ymax, zmax));
	}

	// texture
	void setTexture(Resource* resource, const std::string& nameInShader)
	{
		resourceList[nameInShader] = resource;
	}

	void uploadTextures() const
	{
		int i{ 0 };
		for (const auto &obj : resourceList)
		{
			obj.second->upload(shader, obj.first, i++);
		}
	}

protected:
	Model* model;

	// texture utility function
	std::unordered_map<std::string, Resource*> resourceList;
};

#endif // !ModelObject3D_hpp
