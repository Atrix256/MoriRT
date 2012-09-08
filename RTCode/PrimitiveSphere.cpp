/*======================================================

PrimitiveSphere.cpp

A sphere primitive

======================================================*/

#include "PrimitiveSphere.h"
#include "Scene.h"

//adapted from "Real Time Collision Detection" 5.3.2
bool CPrimitiveSphere::InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance /*= -1.0f*/)
{
	//not inside til proven otherwise
	CollisionInfo.m_bFromInside = false;

	//get the vector from the center of this circle to where the ray begins.
	Vec3 vM = ray.m_vPos - m_vCenter;

    //get the dot product of the above vector and the ray's vector
	MRTFLOAT fB = vM.Dot(ray.m_vDir);

	MRTFLOAT fC = vM.Dot(vM) - m_fRadius * m_fRadius;

	//exit if r's origin outside s (c > 0) and r pointing away from s (b > 0)
	if(fC > 0.0f && fB > 0.0f)
	{
		return false;
	}

	//calculate discriminant
	MRTFLOAT fDiscr = fB * fB - fC;

	//a negative discriminant corresponds to ray missing sphere
	if(fDiscr < 0.0f)
	{
		return false;
	}

	//ray now found to intersect sphere, compute smallest t value of intersection
	CollisionInfo.m_fCollisionTime = -fB - sqrtf(fDiscr);

	//if t is negative, ray started inside sphere so clamp t to zero and remember that we hit from the inside
	if(CollisionInfo.m_fCollisionTime < 0.0f)
	{
	  CollisionInfo.m_fCollisionTime = -fB + sqrtf(fDiscr);
	  CollisionInfo.m_bFromInside = true;
	}

	//enforce a max distance if we should
	if(fMaxDistance >= 0.0f)
	{
		if(CollisionInfo.m_fCollisionTime > fMaxDistance)
		{
			return false;
		}
	}

    //compute the point of intersection
	CollisionInfo.m_vCollisionPoint = ray.m_vPos + ray.m_vDir * CollisionInfo.m_fCollisionTime;

	//fill in the UV and normal info too
	GetUVCoordinates(CollisionInfo.m_vCollisionPoint,CollisionInfo.m_fTextureU,CollisionInfo.m_fTextureV);
	CollisionInfo.m_vSurfaceNormal = GetNormalAtPoint(CollisionInfo.m_vCollisionPoint);
	CollisionInfo.m_pTexturePrimitive = this;

	return true;
}

Vec3 CPrimitiveSphere::GetNormalAtPoint(Vec3 &vPoint) const
{
	Vec3 vDir = vPoint - m_vCenter;
	vDir.Normalize();
	return vDir;
}

AABB CPrimitiveSphere::GetAABB() const
{
	Vec3 vRadius(m_fRadius, m_fRadius, m_fRadius);
	AABB ret(m_vCenter - vRadius, m_vCenter + vRadius);
	return ret;
}

void CPrimitiveSphere::SetPosition(Vec3 vPos)
{
	//tell our scene we are going to move
	m_pParentScene->PrimitivePreChangeGeometry(this);

	//move to the new location
	m_vCenter = vPos;

	//tell our scene we are done moving
	m_pParentScene->PrimitivePostChangeGeometry(this);
}

void CPrimitiveSphere::SetIsLight(bool bIsLight)
{
	m_pParentScene->PrimitivePreChangeGeometry(this);

	if(m_bIsLight == bIsLight)
		return;

	m_bIsLight = bIsLight;

	m_pParentScene->OnPrimitiveSetIsLight(this,m_bIsLight);

	m_pParentScene->PrimitivePostChangeGeometry(this);
}

void CPrimitiveSphere::GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const
{
	/*
	//just get the polar coordinates of the point on the sphere
	Vec3 vRel = vPoint - m_vCenter;
	fU = atan2(vRel.m_fZ,sqrtf(vRel.m_fX * vRel.m_fX + vRel.m_fY * vRel.m_fY)) * m_fScaleX + m_fOffsetX;
	fV = atan2(vRel.m_fY,vRel.m_fX) * m_fScaleY + m_fOffsetY;
	*/

	Vec3 vRel = vPoint - m_vCenter;
	vRel.Normalize();

	fU = vRel.m_fX * m_fScaleX + m_fOffsetX;
	fV = vRel.m_fY * m_fScaleY + m_fOffsetY;
}