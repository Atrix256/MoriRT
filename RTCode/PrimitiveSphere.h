/*======================================================

PrimitiveSphere.h

A sphere primitive

======================================================*/

#ifndef PRIMITIVESPHERE_H
#define PRIMITIVESPHERE_H

#include "Vec3.h"
#include "PrimitiveBase.h"

//forward declarations
class CScene;

class CPrimitiveSphere : public CPrimitiveBase
{
public:
	CPrimitiveSphere(CScene *pParentScene, Vec3 vCenter, MRTFLOAT fRadius)
	:CPrimitiveBase(pParentScene)
	,m_vCenter(vCenter)
	,m_fRadius(fRadius)
	,m_bIsLight(false)
	{
	}

	virtual bool InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance = -1.0f);
	virtual Vec3 GetPosition() const {return m_vCenter;}
	virtual void SetPosition(Vec3 vPos);
	virtual Vec3 GetNormalAtPoint(Vec3 &vPoint) const;
	virtual AABB GetAABB() const;
	virtual EPrimitiveType GetPrimitiveType() const {return CPrimitiveBase::kPrimitiveSphere;}

	virtual void SetIsLight(bool bIsLight);
	virtual bool GetIsLight() const {return m_bIsLight;}

	virtual void GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const;

private:
	Vec3 m_vCenter;
	MRTFLOAT m_fRadius;
	bool m_bIsLight;
};

#endif //PRIMITIVESPHERE_H