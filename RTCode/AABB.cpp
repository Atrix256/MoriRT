/*======================================================

AABB.cpp

axis aligned bounding box

======================================================*/

#include "AABB.h"
#include "RTMath.h"

void AABB::SetToPoint(Vec3 vP)
{
	m_vMin = vP;
	m_vMax = vP;
}

void AABB::AddPoint(Vec3 vP)
{
	if(vP.m_fX < m_vMin.m_fX)
		m_vMin.m_fX = vP.m_fX;

	if(vP.m_fX > m_vMax.m_fX)
		m_vMax.m_fX = vP.m_fX;

	if(vP.m_fY < m_vMin.m_fY)
		m_vMin.m_fY = vP.m_fY;

	if(vP.m_fY > m_vMax.m_fY)
		m_vMax.m_fY = vP.m_fY;
	
	if(vP.m_fZ < m_vMin.m_fZ)
		m_vMin.m_fZ = vP.m_fZ;

	if(vP.m_fZ > m_vMax.m_fZ)
		m_vMax.m_fZ = vP.m_fZ;
}

void AABB::AddAABB(AABB aabb)
{
	AddPoint(aabb.m_vMin);
	AddPoint(aabb.m_vMax);
}

void AABB::GrowToMultipleOf(MRTFLOAT fMultiple)
{
	m_vMin.m_fX = RoundNumberDownToMultipleOf(m_vMin.m_fX,fMultiple);
	m_vMin.m_fY = RoundNumberDownToMultipleOf(m_vMin.m_fY,fMultiple);
	m_vMin.m_fZ = RoundNumberDownToMultipleOf(m_vMin.m_fZ,fMultiple);

	m_vMax.m_fX = RoundNumberUpToMultipleOf(m_vMax.m_fX,fMultiple);
	m_vMax.m_fY = RoundNumberUpToMultipleOf(m_vMax.m_fY,fMultiple);
	m_vMax.m_fZ = RoundNumberUpToMultipleOf(m_vMax.m_fZ,fMultiple);
}

bool AABB::ContainsPoint(Vec3 P) const
{
	for(int nIndex = 0; nIndex < 3; ++nIndex)
	{
		if(P[nIndex] < m_vMin[nIndex] ||
		   P[nIndex] > m_vMax[nIndex])
		{
			return false;
		}
	}

	return true;
}