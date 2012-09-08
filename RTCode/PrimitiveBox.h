/*======================================================

PrimitiveBox.h

A box primitive

======================================================*/

#ifndef PRIMITIVEBOX_H
#define PRIMITIVEBOX_H

#include "Vec3.h"
#include "PrimitiveBase.h"

//forward declarations
class CScene;

class CPrimitiveBox: public CPrimitiveBase
{
public:
	CPrimitiveBox(CScene *pParentScene, Vec3 vPos, Vec3 vScale);

	virtual bool InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance = -1.0f);
	virtual Vec3 GetPosition() const {return m_vPos;}
	virtual void SetPosition(Vec3 vPos);
	virtual Vec3 GetNormalAtPoint(Vec3 &vPoint) const;
	virtual AABB GetAABB() const;
	virtual EPrimitiveType GetPrimitiveType() const {return CPrimitiveBase::kPrimitiveBox;}

	virtual void GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const;

	void SetScale(Vec3 vScale);

private:
	Vec3 m_vPos;
	Vec3 m_vScale;

	AABB m_AABB;
};

#endif //PRIMITIVEBOX_H