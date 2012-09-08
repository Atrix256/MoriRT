/*======================================================

PrimitivePlanes.cpp

A primitive constructed by N number of planes

======================================================*/

#include "PrimitivePlanes.h"
#include "Scene.h"

CPrimitivePlanes::CPrimitivePlanes(CScene *pParentScene)
:CPrimitiveBase(pParentScene)
{
}

void CPrimitivePlanes::AddPlane(Vec3 vA, Vec3 vB, Vec3 vC)
{
	SPlane NewPlane(vA,vB,vC);
	m_Planes.push_back(NewPlane);
}

void CPrimitivePlanes::FinalizeShape(bool bAddToScene /*= true*/)
{
	if(bAddToScene)
	{
		m_pParentScene->AddPrimitive(this,bAddToScene);
	}
}

bool CPrimitivePlanes::InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance /*= -1.0f*/)
{

	for(PlaneList::iterator it = m_Planes.begin(); it != m_Planes.end(); ++it)
	{
	}

	return false;

	/*

	//never on inside
	CollisionInfo.m_bFromInside = false;

	//calculate n
	Vec3 vAB = m_vB - m_vA;
	Vec3 vAC = m_vC - m_vA;
	Vec3 vN = vAB.Cross(vAC);

	//calculate d
	Vec3 vQP = ray.m_vDir * -1;
	MRTFLOAT fD = vQP.Dot(vN);
	if(fD <= 0)
		return false;

	//calculate e
	Vec3 vAP = ray.m_vPos - m_vA;
	Vec3 vE = vQP.Cross(vAP);

	//calculate t
	CollisionInfo.m_fCollisionTime = vAP.Dot(vN) / fD;

	if(CollisionInfo.m_fCollisionTime < 0)
		return false;

	//enforce a max distance if we should
	if(fMaxDistance > 0.0f)
	{
		if(CollisionInfo.m_fCollisionTime > fMaxDistance)
		{
			return false;
		}
	}		

	//calculate v
	MRTFLOAT fV = vAC.Dot(vE) / fD;

	//calculate w
	MRTFLOAT fW = -vAB.Dot(vE) / fD;

	//calculate u
	MRTFLOAT fU = 1.0f - fV - fW;

	//comment this below test out for a plane test
	if(fU < 0 || fV < 0 || fW < 0)
		return false;

	//calculate the intersection point
	CollisionInfo.m_vCollisionPoint = ray.m_vPos + ray.m_vDir * CollisionInfo.m_fCollisionTime;

	//fill in the UV and normal info too
	GetUVCoordinates(CollisionInfo.m_vCollisionPoint,CollisionInfo.m_fTextureU,CollisionInfo.m_fTextureV);
	CollisionInfo.m_vSurfaceNormal = GetNormalAtPoint(CollisionInfo.m_vCollisionPoint);
	CollisionInfo.m_pTexturePrimitive = this;

	//return our data!
	return true;
	*/
}

Vec3 CPrimitivePlanes::GetNormalAtPoint(Vec3 &vPoint) const
{
	return Vec3(0.0f,0.0f,0.0f);
}

AABB CPrimitivePlanes::GetAABB() const
{
	AABB aabb;
	return aabb;
}

void CPrimitivePlanes::GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const
{
	fU = 0.0f;
	fV = 0.0f;
}