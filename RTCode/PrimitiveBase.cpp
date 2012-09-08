/*======================================================

PrimitiveBase.cpp

The base class for primtives

======================================================*/

#include "PrimitiveBase.h"
#include "RayTracer.h"
#include "TextureManager.h"
#include "Texture.h"
#include "Scene.h"
#include <algorithm>

int CPrimitiveBase::s_nNextPrimitiveID = 0;
	
CPrimitiveBase::CPrimitiveBase(CScene *pParentScene)
:m_fReflection(0.0f)
,m_fDiffuse(1.0f)
,m_DiffuseColor(1.0f,1.0f,1.0f)
,m_SpecularColor(1.0f,1.0f,1.0f)
,m_EmissiveColor(0.0f,0.0f,0.0f)
,m_vLightAbsorption(1.0f,1.0f,1.0f)
,m_fLightAbsorptionScalar(0.05f)
,m_fRefraction(0.0f)
,m_fRefractionIndex(1.0f)
,m_pParentScene(pParentScene)
,m_pTexture(0)
,m_pTextureFunction(0)
,m_fScaleX(1.0f)
,m_fScaleY(1.0f)
,m_fOffsetX(0.0f)
,m_fOffsetY(0.0f)
{
	m_nPrimitiveID = s_nNextPrimitiveID;
	s_nNextPrimitiveID++;
}

CPrimitiveBase::~CPrimitiveBase()
{
	if(m_pTexture)
		TextureManager.FreeTexture(m_pTexture);
}

Vec3 CPrimitiveBase::CalculateLightAbsorbance(MRTFLOAT fTimeInShape) const
{
	Vec3 vAbsorbance = GetLightAbsorption() * GetLightAbsorptionScalar() * -fTimeInShape;
	Vec3 vTransparency = Vec3( expf( vAbsorbance.m_fX ), 
								expf( vAbsorbance.m_fY ), 
								expf( vAbsorbance.m_fZ ) );

	return vTransparency;
}

const char *CPrimitiveBase::GetPrimitiveTypeString()
{
	switch(GetPrimitiveType())
	{
		case kPrimitiveSphere: return "Sphere";
		case kPrimitiveTriangle: return "Triangle";
		case kPrimitiveBox: return "Box";
		case kPrimitivePlanes: return "Planes";
		case kPrimitiveComposite: return "Composite";
	}

	return "<unknown>";
}

bool CPrimitiveBase::RayIntersects(int nRayIndex, const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance /*= -1.0f*/)
{
	return InternalRayIntersects(ray,CollisionInfo,fMaxDistance);

	//not multithreaded friendly!
	/*
	int nFrameID = CRayTracer::GetSingleton().GetFrameID();

	//if our collision cache is stale, recalculate it
	if( nRayIndex == -1 ||
		m_CachedCollisionInfo.m_nRayIndex == -1 ||
		m_CachedCollisionInfo.m_nRayIndex != nRayIndex ||
		m_CachedCollisionInfo.m_nFrameIndex != nFrameID ||
		m_CachedCollisionInfo.m_fMaxDistance != fMaxDistance)
	{
		m_CachedCollisionInfo.m_nRayIndex = nRayIndex;
		m_CachedCollisionInfo.m_nFrameIndex = nFrameID;
		m_CachedCollisionInfo.m_fMaxDistance = fMaxDistance;

		m_CachedCollisionInfo.m_bIntersected = InternalRayIntersects(ray,m_CachedCollisionInfo.m_fCollisionTime,m_CachedCollisionInfo.m_vCollisionPoint,m_CachedCollisionInfo.m_bFrominside,fMaxDistance);
	}

	//return the cached data
	fCollisionTime = m_CachedCollisionInfo.m_fCollisionTime;
	vCollisionPoint = m_CachedCollisionInfo.m_vCollisionPoint;
	bFromInside = m_CachedCollisionInfo.m_bFrominside;
	return m_CachedCollisionInfo.m_bIntersected;*/
}

void CPrimitiveBase::SetTexture(const char *szFileName)
{
	if(CRayTracer::GetSingleton().GetTexturesEnabled())
		m_pParentScene->PrimitivePreChangeColor(this);

	CTexture *pOldTexture = m_pTexture;
	m_pTexture = TextureManager.LoadTexture(szFileName);

	if(pOldTexture)
		TextureManager.FreeTexture(pOldTexture);

	if(CRayTracer::GetSingleton().GetTexturesEnabled())
		m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetTextureFunction(FnTextureFunction pTextureFunction)
{
	if(CRayTracer::GetSingleton().GetTexturesEnabled())
		m_pParentScene->PrimitivePreChangeColor(this);

	m_pTextureFunction = pTextureFunction;

	if(CRayTracer::GetSingleton().GetTexturesEnabled())
		m_pParentScene->PrimitivePostChangeColor(this);
}

Vec3 CPrimitiveBase::GetDiffuseColor(const SCollisionInfo &CollisionInfo) const
{
	//if textures are on
	if(CRayTracer::GetSingleton().GetTexturesEnabled())
	{
		//if we have a texture, sample it
		if(m_pTexture)
		{
			MRTFLOAT fU,fV;
			GetUVCoordinates(CollisionInfo.m_vCollisionPoint,fU,fV);
			Vec3 vRet = m_pTexture->GetTexelBilinear(fU,fV);
			vRet *= m_DiffuseColor;
			return vRet;
		}
		//else if we have a texture function, call it
		else if(m_pTextureFunction)
		{
			return m_pTextureFunction(CollisionInfo,this,m_DiffuseColor);
		}
	}

	//return the diffuse color if we get here
	return m_DiffuseColor;
}

void CPrimitiveBase::SetTextureScale(MRTFLOAT fScaleX,MRTFLOAT fScaleY)
{
	if(CRayTracer::GetSingleton().GetTexturesEnabled())
		m_pParentScene->PrimitivePreChangeColor(this);

	m_fScaleX = fScaleX;
	m_fScaleY = fScaleY;

	if(CRayTracer::GetSingleton().GetTexturesEnabled())
		m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetTextureOffset(MRTFLOAT fOffsetX,MRTFLOAT fOffsetY)
{
	if(CRayTracer::GetSingleton().GetTexturesEnabled())
		m_pParentScene->PrimitivePreChangeColor(this);

	m_fOffsetX = fOffsetX;
	m_fOffsetY = fOffsetY;

	if(CRayTracer::GetSingleton().GetTexturesEnabled())
		m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetReflection(MRTFLOAT fReflection)
{
	bool bWasReflective = m_fReflection != 0.0f;
	bool bIsReflective = fReflection != 0.0f;

	bool bUpdateGeometry = (bWasReflective != bIsReflective);

	if(bUpdateGeometry)
		m_pParentScene->PrimitivePreChangeGeometry(this);
	else
		m_pParentScene->PrimitivePreChangeColor(this);

	m_fReflection = fReflection;

	if(bUpdateGeometry)
		m_pParentScene->PrimitivePostChangeGeometry(this);
	else
		m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetDiffuse(MRTFLOAT fDiffuse)
{
	m_pParentScene->PrimitivePreChangeColor(this);

	m_fDiffuse = fDiffuse;

	m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetDiffuseColor(Vec3 NewColor)
{
	m_pParentScene->PrimitivePreChangeColor(this);

	m_DiffuseColor = NewColor;

	m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetSpecularColor(Vec3 NewColor)
{
	m_pParentScene->PrimitivePreChangeColor(this);

	m_SpecularColor = NewColor;

	m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetEmissiveColor(Vec3 NewColor)
{
	m_pParentScene->PrimitivePreChangeColor(this);

	m_EmissiveColor = NewColor;

	m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetLightAbsorption(Vec3 vLightAbsorption, MRTFLOAT fLightAbsorptionScalar)
{
	m_pParentScene->PrimitivePreChangeColor(this);

	m_vLightAbsorption = vLightAbsorption;
	m_fLightAbsorptionScalar = fLightAbsorptionScalar;

	m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetRefraction(MRTFLOAT fRefraction)
{
	m_pParentScene->PrimitivePreChangeColor(this);

	m_fRefraction = fRefraction;

	m_pParentScene->PrimitivePostChangeColor(this);
}

void CPrimitiveBase::SetRefractionIndex(MRTFLOAT fRefractionIndex)
{
	m_pParentScene->PrimitivePreChangeGeometry(this);

	m_fRefractionIndex = fRefractionIndex;

	m_pParentScene->PrimitivePostChangeGeometry(this);
}

void CPrimitiveBase::GetRayCollisionList(const Ray &ray, CompositeRayCollisionList &RayCollisionList)
{
	SCompositeRayCollisionItem NewItem;
	NewItem.m_pPrimitive = this;

	//find all the collisions down the ray
	Ray TempRay(ray);
	MRTFLOAT fCollisionTimeOffset = 0.0f;
	while(InternalRayIntersects(TempRay,NewItem.m_CollisionInfo))
	{
		//advance the ray
		TempRay.m_vPos += TempRay.m_vDir * (NewItem.m_CollisionInfo.m_fCollisionTime + 0.01f);

		//update our collision time offset
		fCollisionTimeOffset += NewItem.m_CollisionInfo.m_fCollisionTime;

		//add the item to the list, after setting the propper collision time
		NewItem.m_CollisionInfo.m_fCollisionTime = fCollisionTimeOffset;
		RayCollisionList.push_back(NewItem);
	}

	//we don't actually need to know about collisions before time zero because we have the info knowing if our ray hit from the inside or not
	/*
	//then, find all the collisions down the ray traveling the opposite direction
	TempRay = ray;
	TempRay.m_vDir *= -1.0f;
	fCollisionTimeOffset = 0.0f;
	while(InternalRayIntersects(TempRay,NewItem.m_CollisionInfo))
	{
		//advance the ray
		TempRay.m_vPos += TempRay.m_vDir * (NewItem.m_CollisionInfo.m_fCollisionTime + 0.01f);

		//update our collision time offset
		fCollisionTimeOffset += NewItem.m_CollisionInfo.m_fCollisionTime;

		//add the item to the list, after setting the propper collision time
		NewItem.m_CollisionInfo.m_fCollisionTime = fCollisionTimeOffset;
		RayCollisionList.push_back(NewItem);
	}
	*/

	std::sort(RayCollisionList.begin(),RayCollisionList.end());
}