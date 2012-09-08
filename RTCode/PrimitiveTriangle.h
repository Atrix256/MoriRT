/*======================================================

PrimitiveTriangle.h

A triangle primitive

======================================================*/

#ifndef PRIMITIVETRIANGLE_H
#define PRIMITIVETRIANGLE_H

#include "Vec3.h"
#include "PrimitiveBase.h"

//forward declarations
class CScene;

class CPrimitiveTriangle : public CPrimitiveBase
{
public:
	CPrimitiveTriangle(CScene *pParentScene, Vec3 vA, Vec3 vB, Vec3 vC);

	virtual bool InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance = -1.0f);
	virtual Vec3 GetPosition() const {return (m_vA + m_vB + m_vC) / 3;}
	virtual void SetPosition(Vec3 vPos);
	virtual Vec3 GetNormalAtPoint(Vec3 &vPoint) const;
	virtual AABB GetAABB() const;
	virtual EPrimitiveType GetPrimitiveType() const {return CPrimitiveBase::kPrimitiveTriangle;}

	virtual void GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const;

private:
	Vec3 m_vA;
	Vec3 m_vB;
	Vec3 m_vC;
	Vec3 m_vNormal;
	AABB m_AABB;

	//used for calculating UV texture coordinates
	Vec3 m_vUAxis;
	Vec3 m_vVAxis;
};

#endif //PRIMITIVETRIANGLE_H