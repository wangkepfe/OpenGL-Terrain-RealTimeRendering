
/*
#include "mainRender.hpp"

void MainRender::stencilWaterRender(mObject& waterPlane)
{
	// stencil test
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilOp(GL_REPLACE, GL_ZERO, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	{
		// render the mask
		waterPlane.render();
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glDepthMask(GL_FALSE);
	{
		water.uploadTexture(GL_TEXTURE0, *tex[WATER_DUDV_TGA], "dudvTex", 0);
		water.uploadTexture(GL_TEXTURE1, *tex[WATER_N_TGA], "normalTex", 1);
		water.uploadTexture(GL_TEXTURE2, planeReflectionFBO.getColorTex(), "reflectionTex", 2);
		water.uploadTexture(GL_TEXTURE3, planeRefractionFBO.getColorTex(), "refractionTex", 3);

		// render in mask
		water.render();
	}
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
}*/