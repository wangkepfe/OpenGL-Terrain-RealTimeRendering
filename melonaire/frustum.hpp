#ifndef frustum_hpp
#define frustum_hpp

#include "glm/glm.hpp"
using glm::vec3;
using glm::vec4;
using glm::mat4;

#include "AABB.hpp"
using CPM_GLM_AABB_NS::AABB;

#include "Camera.hpp"

class ViewFrustum
{
public:
	ViewFrustum() {}
	ViewFrustum(float fovyInDegrees, float aspectRatio, float _znear, float _zfar) {
		setParam(fovyInDegrees, aspectRatio, _znear, _zfar);
	}
	~ViewFrustum() {}

	void setParam(float fovyInDegrees, float aspectRatio, float _znear, float _zfar)
	{
		znear = _znear;
		zfar = _zfar;

		up = znear * tanf(fovyInDegrees * M_PI / 360.0);
		right = up * aspectRatio;

		/*
		if (aspectRatio < 1.0)
		{
			up = znear * tanf(fovyInDegrees * M_PI / 360.0);
			right = up * aspectRatio;
		}
		else
		{
			right = znear * tanf(fovyInDegrees * M_PI / 360.0);
			up = right / aspectRatio;
		}
		*/
	}

	void update(Camera* camera)
	{
		
		// method 1 : calculate the plane by geometry

		// step1: get ¡°real¡± front, righ, up vector
		vec3 pos = camera->getPos();
		vec3 lookat = camera->getLookAt();
		vec3 upVec = camera->getUp();

		vec3 frontVec = lookat - pos;
		vec3 rightVec = glm::normalize(glm::cross(frontVec, upVec));
		upVec = glm::normalize(glm::cross(rightVec, frontVec));

		// step2: get plane equations
		vec3 normal;

		// far
		normal = glm::normalize(frontVec);
		m_planes[FrustumPlane_Far] = vec4(normal,  -glm::dot(normal, pos) - zfar);

		// right
		normal = glm::normalize(glm::cross(frontVec * znear + rightVec * right, upVec));
		m_planes[FrustumPlane_Right] = vec4(normal, -glm::dot(normal, pos));

		// left
		normal = - glm::normalize(glm::cross(frontVec * znear - rightVec * right, upVec));
		m_planes[FrustumPlane_Left] = vec4(normal, -glm::dot(normal, pos));

		// up
		normal = - glm::normalize(glm::cross(frontVec * znear + upVec * up, rightVec));
		m_planes[FrustumPlane_Top] = vec4(normal, -glm::dot(normal, pos));

		// down
		normal = glm::normalize(glm::cross(frontVec * znear - upVec * up, rightVec));
		m_planes[FrustumPlane_Bottom] = vec4(normal, -glm::dot(normal, pos));

		/*
		// method 2 : extract place equations from combo matrix

		mat4 m = camera->getProjectionMatrix() * camera->getViewMatrixLH();
	
		m_planes[FrustumPlane_Right].x = m[0][3] - m[0][0];
		m_planes[FrustumPlane_Right].y = m[1][3] - m[1][0];
		m_planes[FrustumPlane_Right].z = m[2][3] - m[2][0];
		m_planes[FrustumPlane_Right].w = m[3][3] - m[3][0];

		m_planes[FrustumPlane_Left].x = m[0][3] + m[0][0];
		m_planes[FrustumPlane_Left].y = m[1][3] + m[1][0];
		m_planes[FrustumPlane_Left].z = m[2][3] + m[2][0];
		m_planes[FrustumPlane_Left].w = m[3][3] + m[3][0];

		m_planes[FrustumPlane_Top].x = m[0][3] + m[0][1];
		m_planes[FrustumPlane_Top].y = m[1][3] + m[1][1];
		m_planes[FrustumPlane_Top].z = m[2][3] + m[2][1];
		m_planes[FrustumPlane_Top].w = m[3][3] + m[3][1];

		m_planes[FrustumPlane_Bottom].x = m[0][3] - m[0][1];
		m_planes[FrustumPlane_Bottom].y = m[1][3] - m[1][1];
		m_planes[FrustumPlane_Bottom].z = m[2][3] - m[2][1];
		m_planes[FrustumPlane_Bottom].w = m[3][3] - m[3][1];

		m_planes[FrustumPlane_Far].x = m[0][3] - m[0][2];
		m_planes[FrustumPlane_Far].y = m[1][3] - m[1][2];
		m_planes[FrustumPlane_Far].z = m[2][3] - m[2][2];
		m_planes[FrustumPlane_Far].w = m[3][3] - m[3][2];

		// normalize place equation

		normalizePlane(m_planes[FrustumPlane_Right]);
		normalizePlane(m_planes[FrustumPlane_Left]);
		normalizePlane(m_planes[FrustumPlane_Top]);
		normalizePlane(m_planes[FrustumPlane_Bottom]);
		normalizePlane(m_planes[FrustumPlane_Far]);	

		*/
	}

	bool cullingAABB(const AABB& aabb)
	{
		vec3 vertex[8];

		vec3 min = aabb.getMin();
		vec3 max = aabb.getMax();

		vertex[0] = vec3(min.x, min.y, min.z);
		vertex[1] = vec3(min.x, min.y, max.z);
		vertex[2] = vec3(min.x, max.y, min.z);
		vertex[3] = vec3(min.x, max.y, max.z);
		vertex[4] = vec3(max.x, min.y, min.z);
		vertex[5] = vec3(max.x, min.y, max.z);
		vertex[6] = vec3(max.x, max.y, min.z);
		vertex[7] = vec3(max.x, max.y, max.z);

		int i, j;
		bool flag;
		for (i = 0; i < 5; i++)
		{
			flag = false;

			for (j = 0; j < 8; j++)
			{
				if (pointToPlane(m_planes[i], vertex[j]) < 0)
				{
					flag = true;
					break;
				}
			}

			if (flag == false)
				return true;
		}

	return false;
	}

private:

	void normalizePlane(vec4& plane)
	{
		plane /= glm::length(vec3(plane));
	}

	float pointToPlane(const vec4& plane, const vec3& point)
	{
		return plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w;
	}

	enum FrustumPlane
	{
		FrustumPlane_Right,
		FrustumPlane_Left,
		FrustumPlane_Top,
		FrustumPlane_Bottom,
		FrustumPlane_Far,
	};

	float right, up, znear, zfar;
	vec4 m_planes[5];
};

#endif // !frustum_hpp

