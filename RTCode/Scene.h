/*======================================================

Scene.h

The object that holds data about a scene

======================================================*/

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <map>
#include "Vec3.h"
#include "Ray.h"
#include "AABB.h"
#include "WorldCell.h"
#include "List.h"

//forward declarations
class CPrimitiveBase;
class CPrimitiveSphere;
class CPrimitiveTriangle;
class CPrimitiveBox;
class CPrimitivePlanes;
class CPrimitiveComposite;
class CCamera;
class CRayCache;
class CScreenCell;
struct SCollisionInfo;

struct SDirectionalLight
{
	Vec3 vDir;
	Vec3 vColor;
};

//typedefs
typedef std::vector<CPrimitiveBase *> PrimitiveList;
typedef RTDynamicList<SDirectionalLight> DirectionalLightList;

class CScene
{
public:
	friend class CPrimitivePlanes;

	CScene(MRTFLOAT fGridSize, const Vec3 &vWorldMin, const Vec3 &vWorldMax);
	~CScene();

	CPrimitiveSphere    *CreateSphere(Vec3 Center, MRTFLOAT fRadius, bool bAddToScene = true);
	CPrimitiveTriangle  *CreateTriangle(Vec3 vA, Vec3 vB, Vec3 vC, bool bAddToScene = true);
	CPrimitiveBox       *CreateBox(Vec3 vPos, Vec3 vScale, bool bAddToScene = true);
	CPrimitivePlanes    *CreatePlanesPrimitive();
	CPrimitiveComposite *CreateCompositePrimitive(CPrimitiveBase *pPrimLeft,CPrimitiveBase *pPrimRight,int nOperation, bool bAddToScene = true);

	void RemovePrimitive(CPrimitiveBase *pPrimitive);

	Vec3 GetColorForRay(CScreenCell &ScreenCell, int nRayIndex, Ray &ray, MRTFLOAT fRayRefractionIndex, int nBouncesAllowed, MRTFLOAT &fTimeTilFirstHit);

	void AddDirectionalLight(Vec3 vDirection, Vec3 vColor);

	//sets and gets the BG color and ambient light
	void SetBackgroundColor(Vec3 vColor) {m_vBackgroundColor = vColor;}
	Vec3 GetBackgroundColor() const {return m_vBackgroundColor;}
	void SetAmbientLight(Vec3 vColor) {m_vAmbientLight = vColor;}
	Vec3 GetAmbientLight() const {return m_vAmbientLight;}

	//objects call these functions before and after they change something that changes their geometry, or the geometry of rays that interact with it
	void PrimitivePreChangeGeometry(CPrimitiveBase *pPrimitive);
	void PrimitivePostChangeGeometry(CPrimitiveBase *pPrimitive);

	//objects call these functions before and after they change soemthing that doesn't affect geometry, just color
	void PrimitivePreChangeColor(CPrimitiveBase *pPrimitive);
	void PrimitivePostChangeColor(CPrimitiveBase *pPrimitive);

	AABB &GetAABB() {return m_AABB;}

	//for debugging
	void DebugLogScene(void);

	//called when a primitive's "IsLight" property is changed
	void OnPrimitiveSetIsLight(CPrimitiveBase *pPrimitive, bool bIsLight);

	//for debugging
	void ToggleWorldGrid();

	AABB GetActualAABB();

private:

	//this gets the primitive list for the specified cell
	PrimitiveList *GetPrimitiveListForCell(int nX,int nY,int nZ);

	//get the world cell at the given location
	CWorldCell *GetWorldCell(int nX,int nY,int nZ);

	//used for adding and removing a primitive from the grid
	void MarkGridWithPrimitive(CPrimitiveBase *pPrimitive);
	void UnmarkGridWithPrimitive(CPrimitiveBase *pPrimitive);

	void GetCellExtentsForAABB(AABB aabb, int nCellMins[3], int nCellMaxes[3]) const;
	void GetCellPos(const Vec3 &vPoint, int nCellPos[3]) const;

	void AddPrimitive(CPrimitiveBase *pPrimitive, bool bAddToScene);

	Vec3 GetPointShadeFromPointLight(CScreenCell &ScreenCell, int nRayIndex, Vec3 &vPoint, CPrimitiveBase *pLightPrimitive, CPrimitiveBase *pIgnorePrimitive, bool bCanUseCachedData);
	Vec3 GetPointShadeFromDirectionalLight(CScreenCell &ScreenCell, int nRayIndex, Vec3 &vPoint, SDirectionalLight *pDirectionalLight, CPrimitiveBase *pIgnorePrimitive, bool bCanUseCachedData);

	Vec3 GetPointShadeFromLight(CScreenCell &ScreenCell, int nRayIndex, Vec3 &vPoint, void *pLightPrimitive, CPrimitiveBase *pIgnorePrimitive, bool bCanUseCachedData, Ray &ray, float fMaxLength = -1.0f);

	//does a ray intersection against the scene using the grid
	bool RayIntersects(CScreenCell &ScreenCell,int nRayIndex, Ray ray, CPrimitiveBase *&pClosestCollisionPrimitive, SCollisionInfo &CollisionInfo, bool bTestLights = true, CPrimitiveBase *pIgnorePrimitive = 0, MRTFLOAT fMaxDistance = -1.0f);

	//this holds all primitives in the scene
	PrimitiveList m_PrimitiveList;

	//this holds all the light primitives in the scene, to be used as a quick lookup for lights
	PrimitiveList m_LightPrimitiveList;

	//directional lights
	DirectionalLightList m_DirectionalLights;

	//background color and ambient light of the scene - currently not working correctly!
	Vec3 m_vBackgroundColor;
	Vec3 m_vAmbientLight;

	//the resolution of the grid cells
	MRTFLOAT m_fGridSize;

	//the AABB of the grid
	AABB m_AABB;

	//the structure that holds the world cells.  yes seriously 4 stars! Dynamic 3d array
	CWorldCell *m_pGridCells;
	int m_nGridCellStartIndices[3];
	int m_nGridCellEndIndices[3];
	int m_nGridCellWidth[3];

	//for debug rendering the world grid
	PrimitiveList m_WorldGridPrimitives;
};

#endif //SCENE_H