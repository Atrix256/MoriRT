/*======================================================

Scene.cpp

The object that holds data about a scene

======================================================*/

#include "Scene.h"
#include "PrimitiveSphere.h"
#include "PrimitiveTriangle.h"
#include "PrimitiveBox.h"
#include "PrimitivePlanes.h"
#include "PrimitiveComposite.h"
#include "RayTracer.h"
#include <algorithm>
#include "RTMath.h"
#include "Camera.h"
#include "PlatformBase.h"
#include "RayCache.h"
#include "ScreenCell.h"
#include <algorithm>
#include <stdio.h>

CScene::CScene(MRTFLOAT fGridSize, const Vec3 &vWorldMin, const Vec3 &vWorldMax)
:m_vBackgroundColor(0.0f,0.0f,0.0f)
,m_vAmbientLight(0.0f,0.0f,0.0f)
,m_fGridSize(fGridSize)
,m_pGridCells(0)
{
	//set our AABB
	m_AABB.m_vMin = vWorldMin;
	m_AABB.m_vMax = vWorldMax;
	m_AABB.GrowToMultipleOf(m_fGridSize);

	//calculate our grid sizes
	int nTotalSize = 1;
	for(int nIndex = 0; nIndex < 3; ++nIndex)
	{
		m_nGridCellStartIndices[nIndex] = (int)floorf(m_AABB.m_vMin[nIndex] / m_fGridSize);
		m_nGridCellEndIndices[nIndex] = (int)ceilf(m_AABB.m_vMax[nIndex] / m_fGridSize);
		m_nGridCellWidth[nIndex] = m_nGridCellEndIndices[nIndex] - m_nGridCellStartIndices[nIndex] + 1;

		nTotalSize *= m_nGridCellWidth[nIndex];
	}

	m_pGridCells = new CWorldCell[nTotalSize];

	for(int nIndex = 0; nIndex < nTotalSize; ++nIndex)
	{
		m_pGridCells[nIndex].SetCellIndex(nIndex);
	}
}

CScene::~CScene()
{
	//free the memory for each primitive
	for(PrimitiveList::iterator it = m_PrimitiveList.begin(); it != m_PrimitiveList.end(); ++it)
	{
		delete (*it);
	}

	//empty the primitive lists
	m_PrimitiveList.clear();
	m_LightPrimitiveList.clear();

	//free grid cells
	delete[] m_pGridCells;
	m_pGridCells = NULL;
}

void CScene::RemovePrimitive(CPrimitiveBase *pPrimitive)
{
	UnmarkGridWithPrimitive(pPrimitive);

	PrimitiveList::iterator it = std::find(m_PrimitiveList.begin(),m_PrimitiveList.end(),pPrimitive);
	if(it != m_PrimitiveList.end())
		m_PrimitiveList.erase(it);

	it = std::find(m_LightPrimitiveList.begin(),m_LightPrimitiveList.end(),pPrimitive);
	if(it != m_LightPrimitiveList.end())
		m_LightPrimitiveList.erase(it);

	delete pPrimitive;
}

void CScene::AddPrimitive(CPrimitiveBase *pPrimitive, bool bAddToScene)
{
	//add the primitive to the primitive list
	m_PrimitiveList.push_back(pPrimitive);

	//add the primitive into the grid cells it belongs to if we should
	if(bAddToScene)
		MarkGridWithPrimitive(pPrimitive);

	//add it to the light list if we should
	if(pPrimitive->GetIsLight())
		m_LightPrimitiveList.push_back(pPrimitive);
}

void CScene::PrimitivePreChangeGeometry(CPrimitiveBase *pPrimitive)
{
	//remove this primitive from the grid
	UnmarkGridWithPrimitive(pPrimitive);
}

void CScene::PrimitivePostChangeGeometry(CPrimitiveBase *pPrimitive)
{
	//add this primitive to the grid
	MarkGridWithPrimitive(pPrimitive);
}

void CScene::PrimitivePreChangeColor(CPrimitiveBase *pPrimitive)
{
}

void CScene::PrimitivePostChangeColor(CPrimitiveBase *pPrimitive)
{
	//get our aabb
	AABB aabb = pPrimitive->GetAABB();

	//calculate the mins and maxes of grid cells affected by this aabb
	int nMins[3];
	int nMaxes[3];
	GetCellExtentsForAABB(aabb,nMins,nMaxes);

	//for each cell in the grid affected
	for(int nIndexZ = nMins[2]; nIndexZ <= nMaxes[2]; ++nIndexZ)
	{
		for(int nIndexY = nMins[1]; nIndexY <= nMaxes[1]; ++nIndexY)
		{
			for(int nIndexX = nMins[0]; nIndexX <= nMaxes[0]; ++nIndexX)
			{
				//mark the camera cells as "pixel dirty" (and geometry clean) that have rays which pass through this cell
				CWorldCell *pCell = GetWorldCell(nIndexX,nIndexY,nIndexZ);

				if(pCell)
				{
					ScreenCellVisitorList &VisitorList = pCell->GetScreenCellVisitors();

					for(int nIndex = VisitorList.size() - 1; nIndex >= 0; --nIndex)
					{
						CCamera *pCamera = VisitorList[nIndex]->GetParentCamera();
						pCamera->MarkScreenCellAsPixelDirty(VisitorList[nIndex]->GetScreenCellIndex());
					}
				}
			}
		}
	}
}

void CScene::MarkGridWithPrimitive(CPrimitiveBase *pPrimitive)
{
	//get our aabb
	AABB aabb = pPrimitive->GetAABB();

	//calculate the mins and maxes of grid cells affected by this aabb
	int nMins[3];
	int nMaxes[3];
	GetCellExtentsForAABB(aabb,nMins,nMaxes);

	//for each cell in the grid affected, we need to add this primitive
	for(int nIndexZ = nMins[2]; nIndexZ <= nMaxes[2]; ++nIndexZ)
	{
		for(int nIndexY = nMins[1]; nIndexY <= nMaxes[1]; ++nIndexY)
		{
			for(int nIndexX = nMins[0]; nIndexX <= nMaxes[0]; ++nIndexX)
			{
				//get the primitive list for this cell
				PrimitiveList *pPrimitiveList = GetPrimitiveListForCell(nIndexX,nIndexY,nIndexZ);
				if(pPrimitiveList)
				{
					pPrimitiveList->push_back(pPrimitive);

					//mark the camera cells as dirty that have rays which pass through this cell
					CWorldCell *pCell = GetWorldCell(nIndexX,nIndexY,nIndexZ);
					ScreenCellVisitorList &VisitorList = pCell->GetScreenCellVisitors();

					for(int nIndex = VisitorList.size() - 1; nIndex >= 0; --nIndex)
					{
						CCamera *pCamera = VisitorList[nIndex]->GetParentCamera();
						pCamera->MarkScreenCellAsDirty(VisitorList[nIndex]->GetScreenCellIndex());
					}
				}
			}
		}
	}
}

void CScene::UnmarkGridWithPrimitive(CPrimitiveBase *pPrimitive)
{
	//get our aabb
	AABB aabb = pPrimitive->GetAABB();

	//calculate the mins and maxes of grid cells affected by this aabb
	int nMins[3];
	int nMaxes[3];
	GetCellExtentsForAABB(aabb,nMins,nMaxes);

	//for each cell in the grid affected, we need to remove this primitive
	for(int nIndexZ = nMins[2]; nIndexZ <= nMaxes[2]; ++nIndexZ)
	{
		for(int nIndexY = nMins[1]; nIndexY <= nMaxes[1]; ++nIndexY)
		{
			for(int nIndexX = nMins[0]; nIndexX <= nMaxes[0]; ++nIndexX)
			{
				//get the primitive list for this cell
				CWorldCell *pCell = GetWorldCell(nIndexX,nIndexY,nIndexZ);
				if(pCell)
				{
					PrimitiveList *pPrimitiveList = pCell->GetPrimitiveList();

					//find the primitive in the list
					PrimitiveList::iterator it = std::find(pPrimitiveList->begin(),pPrimitiveList->end(),pPrimitive);

					//remove it if it was in the list
					if(it != pPrimitiveList->end())
					{
						pPrimitiveList->erase(it);

						//mark the camera cells as dirty that have rays which pass through this cell
						ScreenCellVisitorList &VisitorList = pCell->GetScreenCellVisitors();

						for(int nIndex = VisitorList.size() - 1; nIndex >= 0; --nIndex)
						{
							CCamera *pCamera = VisitorList[nIndex]->GetParentCamera();
							pCamera->MarkScreenCellAsDirty(VisitorList[nIndex]->GetScreenCellIndex());
						}
					}
				}
			}
		}
	}
}

CPrimitiveSphere *CScene::CreateSphere(Vec3 Center, MRTFLOAT fRadius, bool bAddToScene /*= true*/)
{
	//create a sphere
	CPrimitiveSphere *pNewPrimitive = new CPrimitiveSphere(this,Center,fRadius);

	//add it to the scene;
	if(bAddToScene)
		AddPrimitive(pNewPrimitive,bAddToScene);

	//return the new primitive
	return pNewPrimitive;
}

CPrimitiveTriangle *CScene::CreateTriangle(Vec3 vA, Vec3 vB, Vec3 vC, bool bAddToScene /*= true*/)
{
	//create a triangle
	CPrimitiveTriangle *pNewPrimitive = new CPrimitiveTriangle(this,vA,vB,vC);

	//add it to the scene;
	if(bAddToScene)
		AddPrimitive(pNewPrimitive,bAddToScene);

	//return the new primitive
	return pNewPrimitive;
}

CPrimitiveBox *CScene::CreateBox(Vec3 vPos, Vec3 vScale, bool bAddToScene /*= true*/)
{
	//create a box
	CPrimitiveBox *pNewPrimitive = new CPrimitiveBox(this,vPos,vScale);

	//add it to the scene;
	if(bAddToScene)
		AddPrimitive(pNewPrimitive,bAddToScene);

	//return the new primitive
	return pNewPrimitive;
}

CPrimitivePlanes *CScene::CreatePlanesPrimitive()
{
	//create a planes primitive
	CPrimitivePlanes *pNewPrimitive = new CPrimitivePlanes(this);

	//return the new primitive
	return pNewPrimitive;
}

CPrimitiveComposite *CScene::CreateCompositePrimitive(CPrimitiveBase *pPrimLeft,CPrimitiveBase *pPrimRight,int nOperation, bool bAddToScene /*= true*/)
{
	//create a composite primitive
	CPrimitiveComposite *pNewPrimitive = new CPrimitiveComposite(this,pPrimLeft,pPrimRight,(CPrimitiveComposite::EOperation)nOperation);

	//add it to the scene;
	if(bAddToScene)
		AddPrimitive(pNewPrimitive,bAddToScene);

	//return the new primitive
	return pNewPrimitive;
}

Vec3 CScene::GetPointShadeFromLight(CScreenCell &ScreenCell, int nRayIndex, Vec3 &vPoint, void *pLightPrimitive, CPrimitiveBase *pIgnorePrimitive, bool bCanUseCachedData, Ray &ray, float fMaxLength /*= -1.0f*/)
{
	//get our ray cache item
	SRayCacheItem *pRayCacheItem = ScreenCell.m_RayCache.GetRayCacheItem(nRayIndex);

	//refresh our lighting info if we need to
	if(!bCanUseCachedData)
	{
		//our running total of lighting collision info
		LightingCollisionInfoList LightingCollisionInfo;

		//the primitives we've seen so far
		PrimitiveList IntersectedPrimitives;

		int nDebugLoopCount = 0;

		bool bDone = false;
		while(!bDone)
		{
			nDebugLoopCount++;
			if(nDebugLoopCount > 10000)
			{
				((int *)0)[0] = 0;
			}

			SCollisionInfo CollisionInfo;
			CPrimitiveBase *pClosestPrimitive;
			if(RayIntersects(false, ScreenCell,nRayIndex, ray, pClosestPrimitive, CollisionInfo, false, pIgnorePrimitive))
			{
				//if we haven't seen this primitive before
				if(std::find(IntersectedPrimitives.begin(),IntersectedPrimitives.end(),pClosestPrimitive) == IntersectedPrimitives.end())
				{
					//add this primitive to the list of primitives we have seen before
					IntersectedPrimitives.push_back(pClosestPrimitive);

					//if the primitive is refractive
					if(CRayTracer::GetSingleton().GetRefractionsEnabled() && pClosestPrimitive->GetRefraction() > 0.0f)
					{
						//if we are not already inside, we need to push the point down the vector a little bit and collide with the shape again
						//to see how deep we go through it
						if(!CollisionInfo.m_bFromInside)
						{
							//see if we can find the other side (if we can't... wtf!)
							Ray TempRay(ray.m_vPos + ray.m_vDir * 0.01f,ray.m_vDir);
							if(pClosestPrimitive->RayIntersects(-1,TempRay,CollisionInfo,fMaxLength))
							{
								//put this info into the light collision info list
								SLightingCollisionInfo NewInfo;
								NewInfo.m_fTimeInside = CollisionInfo.m_fCollisionTime;
								NewInfo.m_pCollisionPrimitive = pClosestPrimitive;
								NewInfo.m_pLightPointer = pLightPrimitive;
								LightingCollisionInfo.push_back(NewInfo);
							}
						}
						else
						{
							//put this info into the light collision info list
							SLightingCollisionInfo NewInfo;
							NewInfo.m_fTimeInside = CollisionInfo.m_fCollisionTime;
							NewInfo.m_pCollisionPrimitive = pClosestPrimitive;
							NewInfo.m_pLightPointer = pLightPrimitive;
							LightingCollisionInfo.push_back(NewInfo);
						}
					}
					else
					{
						//if we get here, the light is blocked and that's all there is to it! No lighting from this light for this ray
						bDone = true;

						SLightingCollisionInfo NewInfo;
						NewInfo.m_fTimeInside = 0.0f;
						NewInfo.m_pCollisionPrimitive = pClosestPrimitive;
						NewInfo.m_pLightPointer = pLightPrimitive;
						pRayCacheItem->m_LightCollisionInfo.push_back(NewInfo);
					}
				}

				//move the ray's position to just beyond this collision
				ray.m_vPos += ray.m_vDir * (CollisionInfo.m_fCollisionTime + 0.01f);
			}
			//if no intersection, we are done.  Add these items to the light collision info list
			else
			{
				for(LightingCollisionInfoList::iterator it = LightingCollisionInfo.begin(); it != LightingCollisionInfo.end(); ++it)
				{
					pRayCacheItem->m_LightCollisionInfo.push_back(*it);
				}

				bDone = true;
			}
		}
	}

	//loop through our cached light collision info and calculate our shade
	Vec3 vShade(1.0f,1.0f,1.0f);
	for(LightingCollisionInfoList::iterator it = pRayCacheItem->m_LightCollisionInfo.begin(); it != pRayCacheItem->m_LightCollisionInfo.end(); ++it)
	{
		if((*it).m_pLightPointer == pLightPrimitive)
		{
			if(CRayTracer::GetSingleton().GetShadowsSetting() == CRayTracer::kShadowsSimple)
			{
				return Vec3(0.0f,0.0f,0.0f);
			}
			else
			{
				if(CRayTracer::GetSingleton().GetRefractionsEnabled() && (*it).m_pCollisionPrimitive->GetRefraction() > 0.0f)
					vShade *= (*it).m_pCollisionPrimitive->CalculateLightAbsorbance((*it).m_fTimeInside);
				else
					return Vec3(0.0f,0.0f,0.0f);
			}
		}
	}

	return vShade;
}

Vec3 CScene::GetPointShadeFromDirectionalLight(CScreenCell &ScreenCell, int nRayIndex, Vec3 &vPoint, SDirectionalLight *pDirectionalLight, CPrimitiveBase *pIgnorePrimitive, bool bCanUseCachedData)
{
	//calculate the ray
	Vec3 vDir = pDirectionalLight->vDir * -1.0f;
	vDir.Normalize();
	Ray ray(vPoint,vDir);

	return GetPointShadeFromLight(ScreenCell,nRayIndex,vPoint,pDirectionalLight,pIgnorePrimitive,bCanUseCachedData,ray);
}

Vec3 CScene::GetPointShadeFromPointLight(CScreenCell &ScreenCell, int nRayIndex, Vec3 &vPoint, CPrimitiveBase *pLightPrimitive, CPrimitiveBase *pIgnorePrimitive, bool bCanUseCachedData)
{
	//calculate the ray
	Vec3 vDir = pLightPrimitive->GetPosition() - vPoint;
	MRTFLOAT fMaxLength = vDir.Length();
	vDir.Normalize();
	Ray ray(vPoint,vDir);

	return GetPointShadeFromLight(ScreenCell,nRayIndex,vPoint,pLightPrimitive,pIgnorePrimitive,bCanUseCachedData,ray,fMaxLength);
}

Vec3 CScene::GetColorForRay(bool bDebugThisRay, CScreenCell &ScreenCell, int nRayIndex, Ray &ray, MRTFLOAT fRayRefractionIndex, int nBouncesAllowed, MRTFLOAT &fTimeTilFirstHit)
{
	#if !PIXELDEBUG
		bDebugThisRay = false;
	#endif

	if(nRayIndex < 0)
	{
		return m_vBackgroundColor;
	}

	//if we've gone too deep, just return the background color
	if(nBouncesAllowed < 0)
	{
		return m_vBackgroundColor;
	}

	bool bCanUseCachedData = ((ScreenCell.m_nFlags & CScreenCell::kFlags_GeometryClean) != 0);

	//get the ray cache item for this ray
	SRayCacheItem *pRayCacheItem = ScreenCell.m_RayCache.GetRayCacheItem(nRayIndex);

	//if we don't have any collision info for this ray, let's get it
	fTimeTilFirstHit = -1;
	if(pRayCacheItem->m_eCollisionStatus == SRayCacheItem::kCollisionUnknown || !bCanUseCachedData)
	{
		if(RayIntersects(bDebugThisRay,ScreenCell,nRayIndex,ray,pRayCacheItem->m_pCollisionPrimitive,pRayCacheItem->m_CollisionInfo,false))
		{
			pRayCacheItem->m_eCollisionStatus = SRayCacheItem::kCollisionHit;
		}
		else
		{
			pRayCacheItem->m_eCollisionStatus = SRayCacheItem::kCollisionMiss;
		}

		//make sure the light collision info is cleared out
		pRayCacheItem->m_LightCollisionInfo.clear();
	}

	//if no intersection with ray, return the background color
	if(pRayCacheItem->m_eCollisionStatus == SRayCacheItem::kCollisionMiss)
	{
		return m_vBackgroundColor;
	}

	//get the UV coordinates again in case they changed
	pRayCacheItem->m_CollisionInfo.m_pTexturePrimitive->GetUVCoordinates(pRayCacheItem->m_CollisionInfo.m_vCollisionPoint,pRayCacheItem->m_CollisionInfo.m_fTextureU,pRayCacheItem->m_CollisionInfo.m_fTextureV);

	//init the time til hit
	fTimeTilFirstHit = pRayCacheItem->m_CollisionInfo.m_fCollisionTime;

	//get some accessors to our data
	CPrimitiveBase *pClosestPrimitive = pRayCacheItem->m_pCollisionPrimitive;
	SCollisionInfo &CollisionInfo = pRayCacheItem->m_CollisionInfo;

	//return the diffuse, unshaded color if shading isn't on, or we hit a light
	if(pClosestPrimitive->GetIsLight())
		return pClosestPrimitive->GetDiffuseColor(CollisionInfo);

	//initialize the accumulated color to the ambient light value
	Vec3 vAccumulatedColor = m_vAmbientLight;

	//if we should render refractions and this primitive has a refraction
	if(CRayTracer::GetSingleton().GetRefractionsEnabled() && pClosestPrimitive->GetRefraction() > 0.0f)
	{
		MRTFLOAT fRefractionIndex = pClosestPrimitive->GetRefractionIndex();
		MRTFLOAT n = fRayRefractionIndex / fRefractionIndex;

		Vec3 vN = CollisionInfo.m_vSurfaceNormal;

		//flip the normal if we hit from the inside
		if(CollisionInfo.m_bFromInside)
			vN = vN * -1;

		MRTFLOAT fCosI = -vN.Dot(ray.m_vDir);
		MRTFLOAT fCosT2 = 1.0f - n * n * (1.0f - fCosI * fCosI);

		if(fCosT2 > 0.0f)
		{
			Vec3 vT = (ray.m_vDir * n) + vN * (n * fCosI - sqrtf( fCosT2));
			MRTFLOAT fTimeInsideOfShape;

			//make sure we have a refraction ray cache item
			if(pRayCacheItem->m_nRefractionRay == -1)
			{
				pRayCacheItem->m_nRefractionRay = ScreenCell.m_RayCache.CreateRayCacheItem();
			}

			Ray RefractedRay = Ray(CollisionInfo.m_vCollisionPoint + vT * 0.01f,vT);

			Vec3 vRefractedColor = GetColorForRay(false, ScreenCell,pRayCacheItem->m_nRefractionRay, RefractedRay, fRefractionIndex,nBouncesAllowed - 1,fTimeInsideOfShape);

			//calculate absorbance
			Vec3 vLightAbsorbance = pClosestPrimitive->CalculateLightAbsorbance(fTimeInsideOfShape);

			Vec3 vDelta = (vRefractedColor * vLightAbsorbance) * pClosestPrimitive->GetRefraction();

			//apply it
			vAccumulatedColor += (vRefractedColor * vLightAbsorbance) * pClosestPrimitive->GetRefraction();
		}
	}

	//if we should render reflections and this primitive has a reflection
	if(CRayTracer::GetSingleton().GetReflectionsEnabled() && pClosestPrimitive->GetReflection() > 0.0f)
	{			
		//calculate the reflection ray
		//vector3 R = a_Ray.GetDirection() - 2.0f * DOT( a_Ray.GetDirection(), N ) * N;
		Vec3 vN = CollisionInfo.m_vSurfaceNormal;
		Vec3 vR = ray.m_vDir - vN * ray.m_vDir.Dot(vN) * 2;

		//push our collision point down the ray a tiny amount so our bounce trace doesn't hit this same object again
		Vec3 vPoint = CollisionInfo.m_vCollisionPoint + vR * 0.01f;

		Ray ReflectedRay(vPoint,vR);

		//make sure we have a reflection ray cache item
		if(pRayCacheItem->m_nReflectionRay == -1)
		{
			pRayCacheItem->m_nReflectionRay = ScreenCell.m_RayCache.CreateRayCacheItem();
		}

		//get the reflected color
		MRTFLOAT fDummyTimeTilFirstHit;
		Vec3 vReflectedColor = GetColorForRay(false, ScreenCell, pRayCacheItem->m_nReflectionRay,ReflectedRay,fRayRefractionIndex,nBouncesAllowed - 1,fDummyTimeTilFirstHit);

		//scale it by how much reflection there is
		vReflectedColor = vReflectedColor * pClosestPrimitive->GetReflection();// * pClosestPrimitive->GetDiffuseColor(CollisionInfo.m_vCollisionPoint);

		//add the reflected color into the accumulated color
		vAccumulatedColor += vReflectedColor;
	}

	//apply phong lighting
	if(CRayTracer::GetSingleton().GetShadingEnabled())
	{
		//add the emissive color
		vAccumulatedColor += pClosestPrimitive->GetEmissiveColor();

		MRTFLOAT fDiffuseAmount = pClosestPrimitive->GetDiffuse();
		MRTFLOAT fSpecularAmount = 1.0f - fDiffuseAmount;

		//directional lights
		for(int nLightIndex = 0; nLightIndex < m_DirectionalLights.size(); ++nLightIndex)
		{
			SDirectionalLight &rLight = m_DirectionalLights[nLightIndex];

			//get the normalized vector from the collision point to the light point
			Vec3 vCollisionPointToLight = rLight.vDir * -1.0f;

			Vec3 vShade(1.0f,1.0f,1.0f);

			//if we should calculate shadows, let's see if anything is between the collision point and the light
			if(CRayTracer::GetSingleton().GetShadowsSetting() != CRayTracer::kShadowsOff)
			{
				vShade = GetPointShadeFromDirectionalLight(ScreenCell, nRayIndex, CollisionInfo.m_vCollisionPoint,&m_DirectionalLights[nLightIndex],pClosestPrimitive,bCanUseCachedData);
			}

			//get the normal of the primitive at the intersection point
			Vec3 vNormal = CollisionInfo.m_vSurfaceNormal;

			//apply the diffuse lighting
			if(fDiffuseAmount > 0.0f)
			{
				MRTFLOAT fDot = vNormal.Dot(vCollisionPointToLight);

				if(fDot > 0.0f)
				{
					vAccumulatedColor += pClosestPrimitive->GetDiffuseColor(CollisionInfo) * fDot * fDiffuseAmount * rLight.vColor * vShade;
				}
			}

			//apply the specular lighting
			if(fSpecularAmount > 0.0f)
			{
				//calculate the reflection vector
				//vector3 R = L - N * 2.0f * DOT( L, N );
				Vec3 vReflection = vCollisionPointToLight - vNormal * 2.0f * vCollisionPointToLight.Dot(vNormal);

				//calculte the dot
				MRTFLOAT fDot = ray.m_vDir.Dot(vReflection);
				if(fDot > 0.0f)
				{
					MRTFLOAT fLightSpecularAmount = powf(fDot,20);
					vAccumulatedColor += pClosestPrimitive->GetSpecularColor() * fLightSpecularAmount * fSpecularAmount * rLight.vColor * vShade;
				}
			}
		}

		//point lights
		for(PrimitiveList::const_iterator it = m_LightPrimitiveList.begin(); it != m_LightPrimitiveList.end(); ++it)
		{
			CPrimitiveBase *pLightPrimitive = (*it);

			//get the normalized vector from the collision point to the light point
			Vec3 vCollisionPointToLight = pLightPrimitive->GetPosition() - CollisionInfo.m_vCollisionPoint;
			vCollisionPointToLight.Normalize();

			Vec3 vShade(1.0f,1.0f,1.0f);

			//if we should calculate shadows, let's see if anything is between the collision point and the light
			if(CRayTracer::GetSingleton().GetShadowsSetting() != CRayTracer::kShadowsOff)
			{
				vShade = GetPointShadeFromPointLight(ScreenCell, nRayIndex, CollisionInfo.m_vCollisionPoint,pLightPrimitive,pClosestPrimitive,bCanUseCachedData);
			}

			//get the normal of the primitive at the intersection point
			Vec3 vNormal = CollisionInfo.m_vSurfaceNormal;

			//apply the diffuse lighting
			if(fDiffuseAmount > 0.0f)
			{
				MRTFLOAT fDot = vNormal.Dot(vCollisionPointToLight);

				if(fDot > 0.0f)
				{
					vAccumulatedColor += pClosestPrimitive->GetDiffuseColor(CollisionInfo) * fDot * fDiffuseAmount * pLightPrimitive->GetDiffuseColor(CollisionInfo) * vShade;
				}
			}

			//apply the specular lighting
			if(fSpecularAmount > 0.0f)
			{
				//calculate the reflection vector
				//vector3 R = L - N * 2.0f * DOT( L, N );
				Vec3 vReflection = vCollisionPointToLight - vNormal * 2.0f * vCollisionPointToLight.Dot(vNormal);

				//calculte the dot
				MRTFLOAT fDot = ray.m_vDir.Dot(vReflection);
				if(fDot > 0.0f)
				{
					MRTFLOAT fLightSpecularAmount = powf(fDot,20);
					vAccumulatedColor += pClosestPrimitive->GetSpecularColor() * fLightSpecularAmount * fSpecularAmount * pLightPrimitive->GetSpecularColor() * vShade;
				}
			}
		}
	}
	//if no phong shading, just add the diffuse color in
	else
	{
		vAccumulatedColor += pClosestPrimitive->GetDiffuseColor(CollisionInfo);
	}
	
	return vAccumulatedColor;
}

CWorldCell *CScene::GetWorldCell(int nX,int nY,int nZ)
{
	//make sure nx, ny and nz are in range of the grid that we have primitives for
	if(nX < m_nGridCellStartIndices[0] || nX > m_nGridCellEndIndices[0] ||
	   nY < m_nGridCellStartIndices[1] || nY > m_nGridCellEndIndices[1] ||
	   nZ < m_nGridCellStartIndices[2] || nZ > m_nGridCellEndIndices[2])
	{
		return 0;
	}

	//get the array indices
	nX -= m_nGridCellStartIndices[0];
	nY -= m_nGridCellStartIndices[1];
	nZ -= m_nGridCellStartIndices[2];

	//return the list!
	return &m_pGridCells[nZ * m_nGridCellWidth[1] * m_nGridCellWidth[0] + nY * m_nGridCellWidth[0] + nX];
}

PrimitiveList *CScene::GetPrimitiveListForCell(int nX,int nY,int nZ)
{
	CWorldCell *pCell = GetWorldCell(nX,nY,nZ);

	if(pCell)
	{
		return pCell->GetPrimitiveList();
	}
	else
	{
		return NULL;
	}
}

bool CScene::RayIntersects(bool bDebugThisRay, CScreenCell &ScreenCell,int nRayIndex, Ray ray, CPrimitiveBase *&pClosestCollisionPrimitive, SCollisionInfo &CollisionInfo, bool bTestLights /* = true*/, CPrimitiveBase *pIgnorePrimitive /* = 0*/, MRTFLOAT fMaxDistance /*= -1.0f*/)
{
	#if !PIXELDEBUG
		bDebugThisRay = false;
	#else
		if(bDebugThisRay)
		{
			m_SceneDebugText = "";
		}
	#endif

	//do a ray vs AABB test to see if the ray hits the AABB, and also we can use this info to advance the ray to the beginning of the AABB
	//for each axis
	MRTFLOAT fRayMinTime = 0.0f;
	MRTFLOAT fRayMaxTime = MRTFLOAT_MAX;
	for(int nIndex = 0; nIndex < 3; ++nIndex)
	{
		//calculate the min and max of the box on this axis
		MRTFLOAT fAxisMin = m_AABB.m_vMin[nIndex];
		MRTFLOAT fAxisMax = m_AABB.m_vMax[nIndex];

		//if the ray is paralel with this axis
		if(fabsf(ray.m_vDir[nIndex]) < 0.0001f)
		{
			//if the ray isn't in the box, bail out we know there's no intersection
			if(ray.m_vPos[nIndex] < fAxisMin || ray.m_vPos[nIndex] > fAxisMax)
			{
				return false;
			}
		}
		else
		{
			//figure out the intersection times of the ray with the 2 values of this axis
			MRTFLOAT fAxisMinTime = (fAxisMin - ray.m_vPos[nIndex]) / ray.m_vDir[nIndex];
			MRTFLOAT fAxisMaxTime = (fAxisMax - ray.m_vPos[nIndex]) / ray.m_vDir[nIndex];

			if(fAxisMinTime > fAxisMaxTime)
			{
				MRTFLOAT fTemp = fAxisMinTime;
				fAxisMinTime = fAxisMaxTime;
				fAxisMaxTime = fTemp;
			}

			//union this time slice with our running total time slice
			if(fAxisMinTime > fRayMinTime)
				fRayMinTime = fAxisMinTime;

			if(fAxisMaxTime < fRayMaxTime)
				fRayMaxTime = fAxisMaxTime;

			//if our time slice shrinks to below zero of a time window, we don't intersect
			if(fRayMinTime > fRayMaxTime)
			{
				return false;
			}
		}
	}

	//advance the ray to the beginning of the AABB
	//for some reason this causes black pixels on macs, so not doing this
	//ray.m_vPos += ray.m_vDir * fRayMinTime;

	//init some vars
	pClosestCollisionPrimitive = 0;
	MRTFLOAT fClosestCollisionTime = fMaxDistance;
	Vec3 vClosestCollisionPoint;

	//calculate our starting position in the grid
	int nGridPos[3];
	GetCellPos(ray.m_vPos,nGridPos);

	//calculate our step for each axis
	int nGridStep[3];
	nGridStep[0] = ray.m_vDir.m_fX < 0.0f ? -1 : 1;
	nGridStep[1] = ray.m_vDir.m_fY < 0.0f ? -1 : 1;
	nGridStep[2] = ray.m_vDir.m_fZ < 0.0f ? -1 : 1;

	//calculate how long in time it takes to cross each grid boundary
	Vec3 vGridDelta;
	//vGridDelta.m_fX = ray.m_vDir.m_fX > 0.0f ? fabsf(m_fGridSize / ray.m_vDir.m_fX) : MRTFLOAT_MAX;
	//vGridDelta.m_fY = ray.m_vDir.m_fY > 0.0f ? fabsf(m_fGridSize / ray.m_vDir.m_fY) : MRTFLOAT_MAX;
	//vGridDelta.m_fZ = ray.m_vDir.m_fZ > 0.0f ? fabsf(m_fGridSize / ray.m_vDir.m_fZ) : MRTFLOAT_MAX;
	vGridDelta.m_fX = ray.m_vDir.m_fX != 0.0f ? fabsf(m_fGridSize / ray.m_vDir.m_fX) : MRTFLOAT_MAX;
	vGridDelta.m_fY = ray.m_vDir.m_fY != 0.0f ? fabsf(m_fGridSize / ray.m_vDir.m_fY) : MRTFLOAT_MAX;
	vGridDelta.m_fZ = ray.m_vDir.m_fZ != 0.0f ? fabsf(m_fGridSize / ray.m_vDir.m_fZ) : MRTFLOAT_MAX;

	//calculate our current distance away from each cell boundary
	Vec3 vTimeToEdges;
	vTimeToEdges.m_fX = CalculateTimeToNextMultipleOf(ray.m_vPos.m_fX,m_fGridSize,ray.m_vDir.m_fX);
	vTimeToEdges.m_fY = CalculateTimeToNextMultipleOf(ray.m_vPos.m_fY,m_fGridSize,ray.m_vDir.m_fY);
	vTimeToEdges.m_fZ = CalculateTimeToNextMultipleOf(ray.m_vPos.m_fZ,m_fGridSize,ray.m_vDir.m_fZ);

	//calculate the exit point on each axis
	int nGridExitPoint[3];
	nGridExitPoint[0] = ray.m_vDir.m_fX < 0.0f ? m_nGridCellStartIndices[0] - 1 : m_nGridCellEndIndices[0] + 1;
	nGridExitPoint[1] = ray.m_vDir.m_fY < 0.0f ? m_nGridCellStartIndices[1] - 1 : m_nGridCellEndIndices[1] + 1;
	nGridExitPoint[2] = ray.m_vDir.m_fZ < 0.0f ? m_nGridCellStartIndices[2] - 1 : m_nGridCellEndIndices[2] + 1;

	while(true)
	{
		//find the next closest edge
		int nClosestEdge = 0;
		if(vTimeToEdges[1] < vTimeToEdges[nClosestEdge])
		{
			nClosestEdge = 1;
		}
		if(vTimeToEdges[2] < vTimeToEdges[nClosestEdge])
		{
			nClosestEdge = 2;
		}

		//get the world cell
		CWorldCell *pCell = GetWorldCell(nGridPos[0],nGridPos[1],nGridPos[2]);
		if(pCell)
		{
			//note that the current screen cell had rays that visited this world cell
			ScreenCell.OnVisitWorldCell(pCell);

			//get the primitive list
			PrimitiveList *pPrimitiveList = pCell->GetPrimitiveList();
			AABB gridAABB;
			for(int nIndex = 0; nIndex < 3; ++nIndex)
			{
				gridAABB.m_vMin[nIndex] = ((MRTFLOAT)nGridPos[nIndex]) * m_fGridSize;
				gridAABB.m_vMax[nIndex] = gridAABB.m_vMin[nIndex] + m_fGridSize;
			}

			for(PrimitiveList::iterator it = pPrimitiveList->begin(); it != pPrimitiveList->end(); ++it)
			{
				CPrimitiveBase *pPrimitive = *it;
				
				if(bTestLights || !pPrimitive->GetIsLight())
				{
					if(pPrimitive != pIgnorePrimitive)
					{
						SCollisionInfo TempCollisionInfo;
						if(pPrimitive->RayIntersects(nRayIndex,ray,TempCollisionInfo,fClosestCollisionTime))
						{
							//make sure the collision point is in the current grid cell
							bool bCollisionInGrid = gridAABB.ContainsPoint(TempCollisionInfo.m_vCollisionPoint);
							if(bCollisionInGrid)
							{
								CollisionInfo = TempCollisionInfo;
								pClosestCollisionPrimitive = pPrimitive;
								fClosestCollisionTime = CollisionInfo.m_fCollisionTime;
							}
						}
					}
				}
			}
		}

		//if we hit something, we are done, return!
		if(pClosestCollisionPrimitive)
		{
			return true;
		}

		//move to the next cell
		vTimeToEdges[nClosestEdge] += vGridDelta[nClosestEdge];
		nGridPos[nClosestEdge] += nGridStep[nClosestEdge];

		//if we left the grid, return that we didn't hit anything
		if(nGridPos[nClosestEdge] == nGridExitPoint[nClosestEdge])
		{
			return false;
		}
	}
}

void CScene::GetCellExtentsForAABB(AABB aabb, int nCellMins[3], int nCellMaxes[3]) const
{
	nCellMins[0] = (int)floorf(aabb.m_vMin.m_fX / m_fGridSize);
	nCellMins[1] = (int)floorf(aabb.m_vMin.m_fY / m_fGridSize);
	nCellMins[2] = (int)floorf(aabb.m_vMin.m_fZ / m_fGridSize);

	nCellMaxes[0] = (int)ceilf(aabb.m_vMax.m_fX / m_fGridSize);
	nCellMaxes[1] = (int)ceilf(aabb.m_vMax.m_fY / m_fGridSize);
	nCellMaxes[2] = (int)ceilf(aabb.m_vMax.m_fZ / m_fGridSize);
}

void CScene::GetCellPos(const Vec3 &vPoint, int nCellPos[3]) const
{
	nCellPos[0] = (int)floorf(vPoint.m_fX / m_fGridSize);
	nCellPos[1] = (int)floorf(vPoint.m_fY / m_fGridSize);
	nCellPos[2] = (int)floorf(vPoint.m_fZ / m_fGridSize);
}

void CScene::DebugLogScene(void)
{
	char szBuffer[256];
	CRayTracer::GetSingleton().GetPlatform().LogText("===============================================\nDumping Scene Info...\nPrimitive List:\n");

	//list all primitives, and what grid cells they should be in
	for(PrimitiveList::iterator it = m_PrimitiveList.begin(); it != m_PrimitiveList.end(); ++it)
	{
		CPrimitiveBase *prim = (*it);

		AABB aabb = prim->GetAABB();

		int nMins[3];
		int nMaxes[3];
		GetCellExtentsForAABB(aabb,nMins,nMaxes);

		sprintf(szBuffer,"%i: %s %s aabb: (%0.2f,%0.2f,%0.2f) - (%0.2f,%0.2f,%0.2f)  cells aabb: (%i,%i,%i) - (%i,%i,%i)\n",prim->GetID(),prim->GetPrimitiveTypeString(),prim->GetIsLight()?"(Light)":"",aabb.m_vMin.m_fX,aabb.m_vMin.m_fY,aabb.m_vMin.m_fZ,aabb.m_vMax.m_fX,aabb.m_vMax.m_fY,aabb.m_vMax.m_fZ,nMins[0],nMins[1],nMins[2],nMaxes[0],nMaxes[1],nMaxes[2]);
		CRayTracer::GetSingleton().GetPlatform().LogText(szBuffer);
	}

	//next, loop through all cells and display the non empty ones.
	int nXWidth = m_nGridCellEndIndices[0] - m_nGridCellStartIndices[0] + 1;
	int nYWidth = m_nGridCellEndIndices[1] - m_nGridCellStartIndices[1] + 1;
	int nZWidth = m_nGridCellEndIndices[2] - m_nGridCellStartIndices[2] + 1;

	CRayTracer::GetSingleton().GetPlatform().LogText("\nScene Grid:\n");
	for(int nIndexX = 0; nIndexX < nXWidth; ++nIndexX)
	{
		for(int nIndexY = 0; nIndexY < nYWidth; ++nIndexY)
		{
			for(int nIndexZ = 0; nIndexZ < nZWidth; ++nIndexZ)
			{
				PrimitiveList *pList = GetPrimitiveListForCell(nIndexX,nIndexY,nIndexZ);
				if(pList && !pList->empty())
				{
					sprintf(szBuffer,"[%i,%i,%i]: ", m_nGridCellStartIndices[0] + nIndexX, m_nGridCellStartIndices[1] + nIndexY, m_nGridCellStartIndices[2] + nIndexZ);
					CRayTracer::GetSingleton().GetPlatform().LogText(szBuffer);

					for(PrimitiveList::iterator it = pList->begin(); it != pList->end(); ++it)
					{
						sprintf(szBuffer,"%i ", (*it)->GetID());
						CRayTracer::GetSingleton().GetPlatform().LogText(szBuffer);
					}

					CRayTracer::GetSingleton().GetPlatform().LogText("\n");
				}
			}
		}
	}
}

void CScene::OnPrimitiveSetIsLight(CPrimitiveBase *pPrimitive, bool bIsLight)
{
	if(bIsLight)
	{
		m_LightPrimitiveList.push_back(pPrimitive);
	}
	else
	{
		PrimitiveList::iterator it = std::find(m_LightPrimitiveList.begin(),m_LightPrimitiveList.end(),pPrimitive);
		if(it != m_LightPrimitiveList.end())
		{
			m_LightPrimitiveList.erase(it);
		}
	}
}

void CScene::ToggleWorldGrid()
{
	if(!m_WorldGridPrimitives.empty())
	{
		for(PrimitiveList::iterator it = m_WorldGridPrimitives.begin(); it != m_WorldGridPrimitives.end(); ++it)
		{
			RemovePrimitive(*it);
		}

		m_WorldGridPrimitives.clear();
	}
	else
	{
		CPrimitiveBase *pPrimitive;

		MRTFLOAT fMinX = m_AABB.m_vMin.m_fX;
		MRTFLOAT fMinY = m_AABB.m_vMin.m_fY;
		MRTFLOAT fMinZ = m_AABB.m_vMin.m_fZ;

		MRTFLOAT fMaxX = m_AABB.m_vMax.m_fX;
		MRTFLOAT fMaxY = m_AABB.m_vMax.m_fY;
		MRTFLOAT fMaxZ = m_AABB.m_vMax.m_fZ;

		MRTFLOAT fMidX = (fMinX + fMaxX) * 0.5f;
		MRTFLOAT fMidY = (fMinY + fMaxY) * 0.5f;
		MRTFLOAT fMidZ = (fMinZ + fMaxZ) * 0.5f;

		MRTFLOAT fLenX = fMaxX - fMinX;
		MRTFLOAT fLenY = fMaxY - fMinY;
		MRTFLOAT fLenZ = fMaxZ - fMinZ;

		fMinX += 1.0f;
		fMinY += 1.0f;
		fMinZ += 1.0f;

		fMaxX -= 1.0f;
		fMaxY -= 1.0f;
		fMaxZ -= 1.0f;

		MRTFLOAT fLineWidth = m_fGridSize / 50.0f;

		//draw Z oriented lines
		for(int nIndexY = 0; nIndexY <= m_nGridCellWidth[1]; ++nIndexY)
		{
			MRTFLOAT fY = (MRTFLOAT)(nIndexY + m_nGridCellStartIndices[1]) * fLenY / (MRTFLOAT)m_nGridCellWidth[1];

			for(int nIndexX = 0; nIndexX <= m_nGridCellWidth[0]; ++nIndexX)
			{
				MRTFLOAT fX = (MRTFLOAT)(nIndexX + m_nGridCellStartIndices[0]) * fLenX / (MRTFLOAT)m_nGridCellWidth[0];

				pPrimitive = CreateBox(Vec3(fX,fY,fMidZ),Vec3(fLineWidth,fLineWidth,fLenZ));
				pPrimitive->SetEmissiveColor(Vec3(0.0f,0.0f,0.3f));
				pPrimitive->SetRefraction(0.5f);
				pPrimitive->SetDiffuse(0.0f);
				pPrimitive->SetSpecularColor(Vec3(0.0f,0.0f,0.0f));
				m_WorldGridPrimitives.push_back(pPrimitive);
			}
		}

		//draw Y oriented lines
		for(int nIndexZ = 0; nIndexZ <= m_nGridCellWidth[2]; ++nIndexZ)
		{
			MRTFLOAT fZ = (MRTFLOAT)(nIndexZ + m_nGridCellStartIndices[2]) * fLenZ / (MRTFLOAT)m_nGridCellWidth[2];

			for(int nIndexX = 0; nIndexX <= m_nGridCellWidth[0]; ++nIndexX)
			{
				MRTFLOAT fX = (MRTFLOAT)(nIndexX + m_nGridCellStartIndices[0]) * fLenX / (MRTFLOAT)m_nGridCellWidth[0];

				pPrimitive = CreateBox(Vec3(fX,fMidY,fZ),Vec3(fLineWidth,fLenY,fLineWidth));
				pPrimitive->SetEmissiveColor(Vec3(0.0f,0.0f,0.3f));
				pPrimitive->SetRefraction(0.5f);
				pPrimitive->SetDiffuse(0.0f);
				pPrimitive->SetSpecularColor(Vec3(0.0f,0.0f,0.0f));
				m_WorldGridPrimitives.push_back(pPrimitive);
			}
		}

		//draw X oriented lines
		for(int nIndexZ = 0; nIndexZ <= m_nGridCellWidth[2]; ++nIndexZ)
		{
			MRTFLOAT fZ = (MRTFLOAT)(nIndexZ + m_nGridCellStartIndices[2]) * fLenZ / (MRTFLOAT)m_nGridCellWidth[2];

			for(int nIndexY = 0; nIndexY <= m_nGridCellWidth[1]; ++nIndexY)
			{
				MRTFLOAT fY = (MRTFLOAT)(nIndexY + m_nGridCellStartIndices[1]) * fLenY / (MRTFLOAT)m_nGridCellWidth[1];

				pPrimitive = CreateBox(Vec3(fMidX,fY,fZ),Vec3(fLenX,fLineWidth,fLineWidth));
				pPrimitive->SetEmissiveColor(Vec3(0.0f,0.0f,0.3f));
				pPrimitive->SetRefraction(0.5f);
				pPrimitive->SetDiffuse(0.0f);
				pPrimitive->SetSpecularColor(Vec3(0.0f,0.0f,0.0f));
				m_WorldGridPrimitives.push_back(pPrimitive);
			}
		}
	}
}

void CScene::AddDirectionalLight(Vec3 vDirection, Vec3 vColor)
{
	SDirectionalLight NewLight;
	NewLight.vDir = vDirection;
	NewLight.vColor = vColor;
	NewLight.vDir.Normalize();

	m_DirectionalLights.push_back(NewLight);
}

const char *CScene::GetSceneDebugtext()
{
	return m_SceneDebugText.c_str();
}

AABB CScene::GetActualAABB()
{
	AABB aabb;

	if(!m_PrimitiveList.empty())
	{
		aabb = m_PrimitiveList[0]->GetAABB();

		for(unsigned int nIndex = 1; nIndex < m_PrimitiveList.size(); ++nIndex)
		{
			aabb.AddAABB(m_PrimitiveList[nIndex]->GetAABB());
		}
	}

	return aabb;
}