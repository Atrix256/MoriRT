/*======================================================

PrimitiveBase.h

The base class for primtives

======================================================*/

#ifndef PRIMITIVEBASE_H
#define PRIMITIVEBASE_H

#include <math.h>
#include "Vec3.h"
#include "Ray.h"
#include "AABB.h"
#include "CollisionInfo.h"

//a struct to help with the problem of grid traversal intersecting the same ray against the same primitive multiple times in the same frame
struct SCollisionInfoCache
{
public:
	SCollisionInfoCache()
	:m_nRayIndex(-1)
	,m_nFrameIndex(-1)
	,m_fMaxDistance(-1.0f)
	,m_bIntersected(false)
	{
	}

	//these are to make sure the cached data is still good
	int m_nRayIndex;
	int m_nFrameIndex;
	MRTFLOAT m_fMaxDistance;

	//this is the cached collision info
	bool m_bIntersected;
	SCollisionInfo m_CollisionInfo;
};

struct SCompositeRayCollisionItem
{
public:
	SCompositeRayCollisionItem()
	:m_pPrimitive(0)
	{
	}

	CPrimitiveBase *m_pPrimitive;
	SCollisionInfo m_CollisionInfo;

	bool operator < (const SCompositeRayCollisionItem &OtherItem) const
	{
		return m_CollisionInfo.m_fCollisionTime > OtherItem.m_CollisionInfo.m_fCollisionTime;
	}
};

//forward declarations
class CScene;
class CTexture;

//typedefs
typedef Vec3(*FnTextureFunction)(const SCollisionInfo &CollisionInfo, const CPrimitiveBase *pPrimitive, const Vec3 &vDiffuseColor);
typedef std::vector<SCompositeRayCollisionItem> CompositeRayCollisionList;

class CPrimitiveBase
{
public:
	CPrimitiveBase(CScene *pParentScene);
	~CPrimitiveBase();

	enum EPrimitiveType
	{
		kPrimitiveSphere,
		kPrimitiveTriangle,
		kPrimitiveBox,
		kPrimitivePlanes,
		kPrimitiveComposite
	};

	//interface
	virtual Vec3 GetPosition() const = 0;
	virtual void SetPosition(Vec3 vPos) = 0;
	virtual Vec3 GetNormalAtPoint(Vec3 &vPoint) const = 0;
	virtual AABB GetAABB() const = 0;
	virtual EPrimitiveType GetPrimitiveType() const = 0;
	virtual void GetUVCoordinates(const Vec3 &vPoint, MRTFLOAT &fU, MRTFLOAT &fV) const = 0;

	//base functionality
	bool RayIntersects(int nRayIndex, const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance = -1.0f);
	const char *GetPrimitiveTypeString();
	void SetReflection(MRTFLOAT fReflection);
	MRTFLOAT GetReflection() const {return m_fReflection;}
	void SetDiffuse(MRTFLOAT fDiffuse);
	MRTFLOAT GetDiffuse() const {return m_fDiffuse;}
	void SetColor(Vec3 NewColor)
	{
		SetDiffuseColor(NewColor);
		SetSpecularColor(NewColor);
	}

	void SetTexture(const char *szFileName);
	void SetTextureFunction(FnTextureFunction pTextureFunction);
	void SetTextureScale(MRTFLOAT fScaleX,MRTFLOAT fScaleY);
	void SetTextureOffset(MRTFLOAT fOffsetX,MRTFLOAT fOffsetY);

	MRTFLOAT GetTextureScaleX() const {return m_fScaleX;}
	MRTFLOAT GetTextureScaleY() const {return m_fScaleY;}

	MRTFLOAT GetTextureOffsetX() const {return m_fOffsetX;}
	MRTFLOAT GetTextureOffsetY() const {return m_fOffsetY;}

	void SetDiffuseColor(Vec3 NewColor);
	Vec3 GetDiffuseColor(const SCollisionInfo &CollisionInfo) const;

	void SetSpecularColor(Vec3 NewColor);
	Vec3 GetSpecularColor() const {return m_SpecularColor;}

	void SetEmissiveColor(Vec3 NewColor);
	Vec3 GetEmissiveColor() const {return m_EmissiveColor;}

	void SetLightAbsorption(Vec3 vLightAbsorption, MRTFLOAT fLightAbsorptionScalar);
	Vec3 GetLightAbsorption() const {return m_vLightAbsorption;}
	MRTFLOAT GetLightAbsorptionScalar() const {return m_fLightAbsorptionScalar;}
	Vec3 CalculateLightAbsorbance(MRTFLOAT fTimeInShape) const;

	void SetRefraction(MRTFLOAT fRefraction);
	MRTFLOAT GetRefraction() const {return m_fRefraction;}

	void SetRefractionIndex(MRTFLOAT fRefractionIndex);
	MRTFLOAT GetRefractionIndex() const {return m_fRefractionIndex;}

	virtual void SetIsLight(bool bIsLight) {}
	virtual bool GetIsLight() const {return false;}

	int GetID() const {return m_nPrimitiveID;}

	void GetRayCollisionList(const Ray &ray, CompositeRayCollisionList &RayCollisionList);

protected:

	//private interface
	virtual bool InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance = -1.0f) = 0;

	//protected members
	MRTFLOAT m_fReflection;
	MRTFLOAT m_fDiffuse;
	Vec3 m_DiffuseColor;
	Vec3 m_SpecularColor;
	Vec3 m_EmissiveColor;

	Vec3 m_vLightAbsorption;
	MRTFLOAT m_fLightAbsorptionScalar;

	MRTFLOAT m_fRefraction;
	MRTFLOAT m_fRefractionIndex;

	CScene *m_pParentScene;

	//cached collision info to help with the problem of grid traversal intersecting the same ray against the same primitive multiple times in the same frame
	SCollisionInfoCache m_CachedCollisionInfo;

	//our texture
	CTexture *m_pTexture;
	FnTextureFunction m_pTextureFunction;

	//our texture params
	MRTFLOAT m_fScaleX;
	MRTFLOAT m_fScaleY;
	MRTFLOAT m_fOffsetX;
	MRTFLOAT m_fOffsetY;

	//a unique ID per primitive
	int m_nPrimitiveID;
	static int s_nNextPrimitiveID;
};

#endif //PRIMITIVEBASE_H