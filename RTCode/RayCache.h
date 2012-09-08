/*======================================================

RayCache.h

Holds per ray cache information

======================================================*/

#ifndef RAYCACHE_H
#define RAYCACHE_H

#include <vector>
#include "Vec3.h"
#include "CollisionInfo.h"
#include "ObjectPool.h"

//forward declarations
class CPrimitiveBase;

//ray cache entry
struct SRayCacheItem
{
public:
	SRayCacheItem()
	:m_eCollisionStatus(kCollisionUnknown)
	,m_pCollisionPrimitive(0)
	,m_nReflectionRay(-1)
	,m_nRefractionRay(-1)
	{
	}

	enum ECollisionStatus
	{
		kCollisionUnknown,
		kCollisionHit,
		kCollisionMiss
	};

	ECollisionStatus m_eCollisionStatus;
	SCollisionInfo m_CollisionInfo;
	CPrimitiveBase *m_pCollisionPrimitive;

	LightingCollisionInfoList m_LightCollisionInfo;

	int m_nReflectionRay;
	int m_nRefractionRay;
};

//typedefs
typedef std::vector<SRayCacheItem *> RayCacheList;

class CRayCache
{
public:
	CRayCache(int nCellPixelWidth, int nCellPixelHeight);
	~CRayCache();

	SRayCacheItem *GetRayCacheItem(int nRayIndex);
	int CreateRayCacheItem();

private:
	CObjectPool<SRayCacheItem,1000> m_RayCacheObjectPool;
	RayCacheList m_RayCache;

	int m_nCellPixelWidth;
	int m_nCellPixelHeight;
};

#endif //RAYCACHE_H