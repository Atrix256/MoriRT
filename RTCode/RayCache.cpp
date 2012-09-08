/*======================================================

RayCache.cpp

Holds per ray cache information

======================================================*/

#include "RayCache.h"

CRayCache::CRayCache(int nCellPixelWidth, int nCellPixelHeight)
:m_nCellPixelWidth(nCellPixelWidth)
,m_nCellPixelHeight(nCellPixelHeight)
{
	//calculate the number of pixels
	int nNumPixels = nCellPixelWidth*nCellPixelHeight;
	
	//reserve that many objects in the ray cache object pool
	m_RayCacheObjectPool.Reserve(nNumPixels);
	
	//reserve the same amount in our ray cache
	m_RayCache.reserve(nNumPixels);

	//allocate that many ray cache items to start out with
	for(int nIndex = 0; nIndex < nNumPixels; ++nIndex)
		m_RayCache.push_back(m_RayCacheObjectPool.Allocate());
}

CRayCache::~CRayCache()
{
}

SRayCacheItem *CRayCache::GetRayCacheItem(int nRayIndex)
{
	return m_RayCache[nRayIndex];
}

int CRayCache::CreateRayCacheItem()
{
	SRayCacheItem *pRayCacheItem;
	pRayCacheItem = m_RayCacheObjectPool.Allocate();
	m_RayCache.push_back(pRayCacheItem);
	return m_RayCache.size() - 1;
}