/*======================================================

Ray.h

A ray for math purposes

======================================================*/

#ifndef RAY_H
#define RAY_H

#include "Vec3.h"

class Ray
{
public:
	Ray() {}

	Ray(Vec3 vPos, Vec3 vDir)
	:m_vPos(vPos)
	,m_vDir(vDir)
	{
	}

	Vec3 m_vPos;
	Vec3 m_vDir;
};

#endif //RAY_H
