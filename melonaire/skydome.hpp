#ifndef skydome_hpp
#define skydome_hpp

#include "ModelObject3D.hpp"

class Skydome : public ModelObject3D
{
public:
	using ModelObject3D::ModelObject3D;

	virtual bool render(bool _uploadMatrices = true, bool _uploadCamera = true, bool _uploadTexture = true, bool _frustumCulling = true) override
	{
		if (_uploadCamera)
		{
			updateProjMatrixFromCamera();
			updateViewMatrixFromCamera();
		}

		if (_uploadMatrices)
		{
			viewMatrix[3][0] = 0;
			viewMatrix[3][1] = 0;
			viewMatrix[3][2] = 0;

			uploadMVPmatrix();
		}

		if (_uploadTexture)
			uploadTextures();

		glDisable(GL_DEPTH_TEST);

		model->drawModel(shader);

		glEnable(GL_DEPTH_TEST);

		return true;
	}

	void uploadTime(float t)
	{
		uploadUniform(t, "time");
	}

private:

};

#endif