/*======================================================

Camera.h

The camera object

======================================================*/

#ifndef CAMERA_H
#define CAMERA_H

#include "Vec3.h"
#include "ScreenGrid.h"

//forward declarations
class CScene;
class Ray;
class CPrimitiveBase;
class CRayCache;

class CCamera
{
public:

	enum ERenderMode
	{
		kRenderNormal,
		kRenderRed,
		kRenderBlue
	};

	CCamera(Vec3 vCameraPos, Vec3 vCameraTarget, Vec3 vUp, int nRenderWidth, int nRenderHeight, MRTFLOAT fFOV, MRTFLOAT fScreenRectSize, int nScreenGridCellCount, Vec3 v3dOffset, bool bChildCamera);
	~CCamera();

	void Update();

	void LookAt(Vec3 vCameraPos, Vec3 vCameraTarget, Vec3 vUp);

	Vec3 GetColorForRay(bool bDebugThisRay, CScreenCell &ScreenCell, unsigned int nRayCacheRayIndex, unsigned int nCameraRayIndex, int nMaxRayBounce);

	void SetScene(CScene *pScene);

	void DirtyAllCachedData(void);

	int GetNumRenderCells() const {return m_nScreenGridCellCount * m_nScreenGridCellCount;}
	void RenderCell(int nIndex, int nMaxRayBounce, ERenderMode eRenderMode, CScreenCell *pOverRideCellPixels = NULL);

	CScene *GetScene() {return m_pScene;}

	void MarkScreenCellAsDirty(int nScreenCellIndex);
	void MarkScreenCellAsPixelDirty(int nScreenCellIndex);

	int GetScreenWidth() const {return m_nRenderWidth;}
	int GetScreenHeight() const {return m_nRenderHeight;}

	void SetRenderCellBoundaries(bool bRender) {m_bRenderCellBoundaries = bRender;}
	bool GetRenderCellBoundaries() const {return m_bRenderCellBoundaries;}
	void SetRedBlue3dMode(bool bRedBlue3dMode);

	bool GetRedBlue3dMode() const {return m_bRedBlue3dMode;}

	void PostRender(unsigned char *pPixelBuffer,int nPixelBufferWidth);

	void SetModeOrthographic(MRTFLOAT fWidth = 1.0f, MRTFLOAT fHeight = 1.0f, MRTFLOAT fOffsetX = 0.5f, MRTFLOAT fOffsetY = 0.5f, MRTFLOAT fNear = 0.0f);
	void SetModeProjection();
	bool GetOrthographic() const {return m_bOrthographic;}

	bool GetRay(int nPixelX, int nPixelY, Ray &CameraRay);

	Vec3 GetPos() const {return m_vCameraPos;}
	Vec3 GetFwd() const {return (m_vCameraTarget - m_vCameraPos).Normalize();}

	void SetResolution(int nWidth, int nHeight);

	void Set3dOffset(Vec3 vOffset) {}

private:
	CScene *m_pScene;
	Vec3 m_vCameraPos;
	Vec3 m_vCameraTarget;
	Vec3 m_vCameraUp;
	int m_nRenderWidth;
	int m_nRenderHeight;
	MRTFLOAT m_fFOV;
	MRTFLOAT m_fVerticalFOV;
	MRTFLOAT m_fScreenRectWidth;
	MRTFLOAT m_fScreenRectHeight;
	int m_nScreenGridCellCount;
	bool m_bOrthographic;
	MRTFLOAT m_fOrthoWidth;
	MRTFLOAT m_fOrthoHeight;
	MRTFLOAT m_fOrthoOffsetX;
	MRTFLOAT m_fOrthoOffsetY;
	MRTFLOAT m_fOrthoNear;

	CScreenGrid m_ScreenGrid;

	bool m_bDirty;
	int m_nRayCount;
	Ray *m_pRays;
	bool m_bRenderCellBoundaries;
	bool m_bRedBlue3dMode;

	Vec3 m_v3dOffset;
	bool m_bIs3dChildCamera;

	CCamera *m_pOtherCameraFor3d;
};

#endif //CAMERA_H