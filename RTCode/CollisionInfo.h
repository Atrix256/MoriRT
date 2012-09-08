/*======================================================

CollisionInfo.h

======================================================*/

#ifndef COLLISIONINFO_H
#define COLLISIONINFO_H

#include "Vec3.h"
#include <vector>

//forward declarations
class CPrimitiveBase;

struct SCollisionInfo
{
public:
	SCollisionInfo()
	:m_fCollisionTime(0.0f)
	,m_bFromInside(false)
	,m_pTexturePrimitive(0)
	,m_fTextureU(0.0f)
	,m_fTextureV(0.0f)
	{
	}

	MRTFLOAT m_fCollisionTime;
	Vec3 m_vCollisionPoint;
	bool m_bFromInside;

	CPrimitiveBase *m_pTexturePrimitive; //so we can ask for new texture coordinates if needed
	MRTFLOAT m_fTextureU;
	MRTFLOAT m_fTextureV;

	Vec3 m_vSurfaceNormal;
};

struct SLightingCollisionInfo
{
public:
	SLightingCollisionInfo()
	:m_pLightPointer(0)
	,m_pCollisionPrimitive(0)
	,m_fTimeInside(0.0f)
	{
	}

	void *m_pLightPointer; //could be a primitive, or a directional light
	CPrimitiveBase *m_pCollisionPrimitive;
	MRTFLOAT m_fTimeInside;
};

//typedefs
typedef std::vector<SLightingCollisionInfo> LightingCollisionInfoList;

#endif //COLLISIONINFO_H