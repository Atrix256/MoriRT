/*======================================================

PrimitiveComposite.h

A primitive which is defined by 2 child primitives and an operation of how to combine them.

A child primitive can also be another composite primitive

======================================================*/

#ifndef PRIMITIVECOMPOSITE_H
#define PRIMITIVECOMPOSITE_H

#include <vector>
#include "Vec3.h"
#include "PrimitiveBase.h"
#include "CollisionInfo.h"

//forward declarations
class CScene;

class CPrimitiveComposite : public CPrimitiveBase
{
public:
	enum EOperation
	{
		kAND = 0,     //only the area where the 2 shapes overlap
		kXOR = 1,     //only the area where there is exactly one shape claiming the space
		kOR = 2,      //the combination of both shapes (add the shapes together)
		kAdd = kOR,   //add the shapes together (the combination of both shapes)
		kSubtract = 3 //subtract the right shape from the left shape
	};

	CPrimitiveComposite(CScene *pParentScene, CPrimitiveBase *pLeft, CPrimitiveBase *pRight, EOperation eOperation)
	:CPrimitiveBase(pParentScene)
	,m_pLeftChild(pLeft)
	,m_pRightChild(pRight)
	,m_eOperation(eOperation)
	{
	}

	virtual bool InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance = -1.0f);
	virtual AABB GetAABB() const;
	virtual EPrimitiveType GetPrimitiveType() const {return CPrimitiveBase::kPrimitiveComposite;}

	virtual Vec3 GetPosition() const
	{
		return Vec3(0.0f,0.0f,0.0f);
	}
	virtual void SetPosition(Vec3 vPos)
	{
	}
	virtual Vec3 GetNormalAtPoint(Vec3 &vPoint) const
	{
		return Vec3(0.0f,0.0f,0.0f);
	}
	virtual void GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const
	{
		fU = fV = 0.0f;
	}


private:

	CPrimitiveBase *m_pLeftChild;
	CPrimitiveBase *m_pRightChild;
	EOperation		m_eOperation;
};

#endif //PRIMITIVECOMPOSITE_H