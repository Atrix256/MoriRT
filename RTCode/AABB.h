/*======================================================

AABB.h

axis aligned bounding box

======================================================*/

#ifndef AABB_H
#define AABB_H

#include "Vec3.h"

class AABB
{
public:
	AABB() {}

	AABB(Vec3 vMin, Vec3 vMax)
	:m_vMin(vMin)
	,m_vMax(vMax)
	{
	}

	void SetToPoint(Vec3 vP);
	void AddPoint(Vec3 vP);
	void AddAABB(AABB aabb);
	void GrowToMultipleOf(MRTFLOAT fMultiple);

	bool ContainsPoint(Vec3 P) const;

	Vec3 m_vMin;
	Vec3 m_vMax;
};

#endif //AABB_H
