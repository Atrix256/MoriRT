/*======================================================

PrimitiveBox.cpp

A box primitive

======================================================*/

#include "PrimitiveBox.h"
#include "Scene.h"
#include <float.h>

CPrimitiveBox::CPrimitiveBox(CScene *pParentScene, Vec3 vPos, Vec3 vScale)
:CPrimitiveBase(pParentScene)
,m_vPos(vPos)
,m_vScale(vScale)
{
	//calculate the AABB
	m_AABB.SetToPoint(vPos - vScale / 2.0f);
	m_AABB.AddPoint(vPos + vScale / 2.0f);
}

bool CPrimitiveBox::InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance /*= -1.0f*/)
{
	MRTFLOAT fRayMinTime = 0.0f;
	MRTFLOAT fRayMaxTime = MRTFLOAT_MAX;
	//int nRayMinAxis = -1;
	//int nRayMaxAxis = -1;

	//enforce a max distance
	if(fMaxDistance >= 0.0f)
	{
		fRayMaxTime = fMaxDistance;
	}

	//for each axis
	for(int nIndex = 0; nIndex < 3; ++nIndex)
	{
		//calculate the min and max of the box on this axis
		MRTFLOAT fAxisMin = m_vPos[nIndex] - m_vScale[nIndex] * 0.5f;
		MRTFLOAT fAxisMax = fAxisMin + m_vScale[nIndex];

		//float fValue = (float)ray.m_vDir[nIndex];

		//if the ray is paralel with this axis
		if(fabsf(ray.m_vDir[nIndex]) < 0.0001f)
		{
			//if the ray isn't in the box, bail out we know there's no intersection
			if(ray.m_vPos[nIndex] < fAxisMin || ray.m_vPos[nIndex] > fAxisMax)
				return false;
		}
		else
		{
			//figure out the intersection times of the ray with the 2 values of this axis
			MRTFLOAT fAxisMinTime = (fAxisMin - ray.m_vPos[nIndex]) / ray.m_vDir[nIndex];
			MRTFLOAT fAxisMaxTime = (fAxisMax - ray.m_vPos[nIndex]) / ray.m_vDir[nIndex];

			//make sure min < max
			if(fAxisMinTime > fAxisMaxTime)
			{
				MRTFLOAT fTemp = fAxisMinTime;
				fAxisMinTime = fAxisMaxTime;
				fAxisMaxTime = fTemp;
			}

			//union this time slice with our running total time slice
			if(fAxisMinTime > fRayMinTime)
			{
				//nRayMinAxis = nIndex;
				fRayMinTime = fAxisMinTime;
			}

			if(fAxisMaxTime < fRayMaxTime)
			{
				//nRayMaxAxis = nIndex;
				fRayMaxTime = fAxisMaxTime;
			}

			//if our time slice shrinks to below zero of a time window, we don't intersect
			if(fRayMinTime > fRayMaxTime)
				return false;
		}
	}

	//if we got here, we do intersect, return our collision info
	CollisionInfo.m_bFromInside = (fRayMinTime == 0.0f);
	if(CollisionInfo.m_bFromInside)
		CollisionInfo.m_fCollisionTime = fRayMaxTime;
	else
		CollisionInfo.m_fCollisionTime = fRayMinTime;
	CollisionInfo.m_vCollisionPoint = ray.m_vPos + ray.m_vDir * CollisionInfo.m_fCollisionTime;
	CollisionInfo.m_bFromInside = (fRayMinTime == 0.0f);

	//fill in the UV and normal info too
	GetUVCoordinates(CollisionInfo.m_vCollisionPoint,CollisionInfo.m_fTextureU,CollisionInfo.m_fTextureV);
	CollisionInfo.m_vSurfaceNormal = GetNormalAtPoint(CollisionInfo.m_vCollisionPoint);
	CollisionInfo.m_pTexturePrimitive = this;

	/*
	CollisionInfo.m_vSurfaceNormal = Vec3(0.0f,0.0f,0.0f);
	int nAxisHit = CollisionInfo.m_bFromInside ? nRayMaxAxis : nRayMinAxis;
	if(CollisionInfo.m_vCollisionPoint[nAxisHit] < m_vPos[nAxisHit])
	{
		CollisionInfo.m_vSurfaceNormal[nAxisHit] = -1.0f;
	}
	else
	{
		CollisionInfo.m_vSurfaceNormal[nAxisHit] = 1.0f;
	}
	*/

	return true;
}

Vec3 CPrimitiveBox::GetNormalAtPoint(Vec3 &vPoint) const
{
	MRTFLOAT fClosestDist;
	int nClosestAxis = -1;

	//figure out which side we are closest to
	for(int nIndex = 0; nIndex < 3; ++nIndex)
	{
		MRTFLOAT fDistFromPos= fabsf(m_vPos[nIndex] - vPoint[nIndex]);
		MRTFLOAT fDistFromEdge = fabsf(fDistFromPos - (m_vScale[nIndex] * 0.5f));

		if(nIndex == 0 || fDistFromEdge < fClosestDist)
		{
			fClosestDist = fDistFromEdge;
			nClosestAxis = nIndex;
		}
		/*
		MRTFLOAT fDistFromEdge = fabs((m_vPos[nIndex] - vPoint[nIndex]) / m_vScale[nIndex]);

		if(nIndex == 0 || fDistFromEdge > fClosestDist)
		{
			fClosestDist = fDistFromEdge;
			nClosestAxis = nIndex;
		}
		*/
	}

	//make a vector pointing the right way.  yay boxes
	Vec3 vRet(0.0f,0.0f,0.0f);
	if(vPoint[nClosestAxis] < m_vPos[nClosestAxis])
	{
		vRet[nClosestAxis] = -1.0f;
	}
	else
	{
		vRet[nClosestAxis] =  1.0f;
	}

	return vRet;
}

void CPrimitiveBox::SetScale(Vec3 vScale)
{
	//tell our scene we are going to move
	m_pParentScene->PrimitivePreChangeGeometry(this);

	//move the box
	m_vScale = vScale;

	//calculate the AABB
	m_AABB.SetToPoint(m_vPos - m_vScale / 2.0f);
	m_AABB.AddPoint(m_vPos + m_vScale / 2.0f);

	//tell our scene we are done moving
	m_pParentScene->PrimitivePostChangeGeometry(this);
}

void CPrimitiveBox::SetPosition(Vec3 vPos)
{
	//tell our scene we are going to move
	m_pParentScene->PrimitivePreChangeGeometry(this);

	//move the box
	m_vPos = vPos;

	//calculate the AABB
	m_AABB.SetToPoint(vPos - m_vScale / 2.0f);
	m_AABB.AddPoint(vPos + m_vScale / 2.0f);

	//tell our scene we are done moving
	m_pParentScene->PrimitivePostChangeGeometry(this);
}

AABB CPrimitiveBox::GetAABB() const
{
	return m_AABB;
}

void CPrimitiveBox::GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const
{
	MRTFLOAT fClosestDist;
	int nClosestAxis = -1;

	//figure out which side we are closest to
	for(int nIndex = 0; nIndex < 3; ++nIndex)
	{
		MRTFLOAT fDistFromPos= fabsf(m_vPos[nIndex] - vPoint[nIndex]);
		MRTFLOAT fDistFromEdge = fabsf(fDistFromPos - (m_vScale[nIndex] * 0.5f));

		if(nIndex == 0 || fDistFromEdge < fClosestDist)
		{
			fClosestDist = fDistFromEdge;
			nClosestAxis = nIndex;
		}
	}

	Vec3 vUAxis;
	Vec3 vVAxis;

	switch(nClosestAxis)
	{
		case 0:
		{
			vUAxis = Vec3(0.0f,1.0f,0.0f);
			vVAxis = Vec3(0.0f,0.0f,1.0f);
			break;
		}
		case 1:
		{
			vUAxis = Vec3(1.0f,0.0f,0.0f);
			vVAxis = Vec3(0.0f,0.0f,1.0f);
			break;
		}
		case 2:
		{
			vUAxis = Vec3(1.0f,0.0f,0.0f);
			vVAxis = Vec3(0.0f,1.0f,0.0f);
			break;
		}
	}

	Vec3 vRelPoint = vPoint - m_vPos;

	fU = vRelPoint.Dot(vUAxis) * m_fScaleX + m_fOffsetX;
	fV = vRelPoint.Dot(vVAxis) * m_fScaleY + m_fOffsetY;
}