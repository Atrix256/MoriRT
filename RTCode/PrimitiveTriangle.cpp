/*======================================================

PrimitiveTriangle.cpp

A triangle primitive

======================================================*/

#include "PrimitiveTriangle.h"
#include "Scene.h"

CPrimitiveTriangle::CPrimitiveTriangle(CScene *pParentScene, Vec3 vA, Vec3 vB, Vec3 vC)
:CPrimitiveBase(pParentScene)
,m_vA(vA)
,m_vB(vB)
,m_vC(vC)
{
	//calculate the normal
	Vec3 vAB = vB - vA;
	Vec3 vAC = vC - vA;
	m_vNormal = vAB.Cross(vAC);
	m_vNormal.Normalize();

	//calculate the AABB
	m_AABB.SetToPoint(m_vA);
	m_AABB.AddPoint(m_vB);
	m_AABB.AddPoint(m_vC);

	m_vUAxis = Vec3(m_vNormal.m_fY, m_vNormal.m_fZ, -m_vNormal.m_fX );
	m_vVAxis = m_vUAxis.Cross( m_vNormal );
}

bool CPrimitiveTriangle::InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance /*= -1.0f*/)
{
	//never on inside
	CollisionInfo.m_bFromInside = false;

	//calculate n
	Vec3 vAB = m_vB - m_vA;
	Vec3 vAC = m_vC - m_vA;
	Vec3 vN = vAB.Cross(vAC);

	//calculate d
	Vec3 vQP = ray.m_vDir * -1;
	MRTFLOAT fD = vQP.Dot(vN);
	if(fD <= 0.0f)
		return false;

	//calculate e
	Vec3 vAP = ray.m_vPos - m_vA;
	Vec3 vE = vQP.Cross(vAP);

	//calculate t
	CollisionInfo.m_fCollisionTime = vAP.Dot(vN) / fD;

	if(CollisionInfo.m_fCollisionTime < 0.0f)
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
	if(fU < 0.0f || fV < 0.0f || fW < 0.0f)
		return false;

	//calculate the intersection point
	CollisionInfo.m_vCollisionPoint = ray.m_vPos + ray.m_vDir * CollisionInfo.m_fCollisionTime;

	//fill in the UV and normal info too
	GetUVCoordinates(CollisionInfo.m_vCollisionPoint,CollisionInfo.m_fTextureU,CollisionInfo.m_fTextureV);
	CollisionInfo.m_vSurfaceNormal = GetNormalAtPoint(CollisionInfo.m_vCollisionPoint);
	CollisionInfo.m_pTexturePrimitive = this;

	//return our data!
	return true;
}

Vec3 CPrimitiveTriangle::GetNormalAtPoint(Vec3 &vPoint) const
{
	return m_vNormal;
}

void CPrimitiveTriangle::SetPosition(Vec3 vPos)
{
	//tell our scene we are going to move
	m_pParentScene->PrimitivePreChangeGeometry(this);

	//move the 3 points
	Vec3 vMovement = vPos - GetPosition();
	m_vA += vMovement;
	m_vB += vMovement;
	m_vC += vMovement;

	//calculate the AABB
	m_AABB.SetToPoint(m_vA);
	m_AABB.AddPoint(m_vB);
	m_AABB.AddPoint(m_vC);

	//tell our scene we are done moving
	m_pParentScene->PrimitivePostChangeGeometry(this);
}

AABB CPrimitiveTriangle::GetAABB() const
{
	return m_AABB;
}

void CPrimitiveTriangle::GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const
{
	fU = vPoint.Dot(m_vUAxis) * m_fScaleX + m_fOffsetX;
	fV = vPoint.Dot(m_vVAxis) * m_fScaleY + m_fOffsetY;
}