/*======================================================

PrimitivePlanes.h

A primitive constructed by N number of planes

======================================================*/

#ifndef PRIMITIVEPLANES_H
#define PRIMITIVEPLANES_H

#include <vector>
#include "Vec3.h"
#include "PrimitiveBase.h"

//forward declarations
class CScene;

//a plane defined by 3 points
struct SPlane
{
public:
	SPlane(Vec3 &vA, Vec3 &vB, Vec3 &vC)
	:m_vA(vA)
	,m_vB(vB)
	,m_vC(vC)
	{
		//calculate the normal
		Vec3 vAB = vB - vA;
		Vec3 vAC = vC - vA;
		m_vNormal = vAB.Cross(vAC);
	}

	//the 3 points defining the plane
	Vec3 m_vA;
	Vec3 m_vB;
	Vec3 m_vC;

	//the normal, cached off
	Vec3 m_vNormal;
};

//typedefs
typedef std::vector<SPlane> PlaneList;

class CPrimitivePlanes : public CPrimitiveBase
{
public:
	CPrimitivePlanes(CScene *pParentScene);

	void AddPlane(Vec3 vA, Vec3 vB, Vec3 vC);
	void FinalizeShape(bool bAddToScene = true);

	virtual bool InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance = -1.0f);
	virtual Vec3 GetPosition() const {return Vec3(0.0f,0.0f,0.0f);}
	virtual void SetPosition(Vec3 vPos) {}
	virtual Vec3 GetNormalAtPoint(Vec3 &vPoint) const;
	virtual AABB GetAABB() const;
	virtual EPrimitiveType GetPrimitiveType() const {return CPrimitiveBase::kPrimitivePlanes;}

	virtual void GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const;

private:
	PlaneList m_Planes;
};

#endif //PRIMITIVEPLANES_H