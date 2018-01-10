#ifndef ModelObject3DInstanced_hpp
#define ModelObject3DInstanced_hpp

#include "ModelObject3D.hpp"
#include "InstancedModel.hpp"

class ModelObject3DInstanced : public ModelObject3D
{
public:
	using ModelObject3D::ModelObject3D;

	void setModelMatrices(mat4* _modelMatrices, int _amount) {
		modelMatrices = _modelMatrices;
		amount = _amount;
	}

	// render
	virtual bool render(bool _uploadMatrices = true, bool _uploadCamera = true, bool _uploadTexture = true, bool _frustumCulling = true)
	{
		if (_frustumCulling)
		{

		}

		if (_uploadCamera)
		{
			uploadViewPositionFromCamera();

			updateProjMatrixFromCamera();
			updateViewMatrixFromCamera();
		}

		if (_uploadMatrices)
		{
			uploadVPmatrix();
		}

		if (_uploadTexture)
			uploadTextures();

		auto modelInstanced = static_cast<InstancedModel*>(model);

		if (modelInstanced == nullptr)
		{
			std::cerr << "Model instanced static cast failed.\n";
		}

		modelInstanced->drawInstancedModel(shader, modelMatrices, amount);

		return true;
	}

private:
	mat4* modelMatrices;
	int amount;
};

#endif // !ModelObject3DInstanced_hpp

