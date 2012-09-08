/*======================================================

RayTracer.h

The entry point for raytracing

======================================================*/

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Defines.h"
#include "Vec3.h"
#include "DebugMenu.h"
#include "CommonUI/UIStack.h"
#include "AABB.h"

class CPlatformBase;
class CCamera;
class CGameBase;

class CRayTracer
{
public:
	enum EShadowsSetting
	{
		kShadowsOff,
		kShadowsSimple,
		kShadowsOn
	};

	static CRayTracer &GetSingleton();

	void Init(CPlatformBase *pPlatform, int nScreenWidth, int nScreenHeight);
	void RenderFrame(MRTFLOAT fFrameTime);
	void CopyFrameToPixels(unsigned char *pPixels, unsigned int nRowPitch);

	void OnKeyTyped(unsigned char nKey);
	void OnKeyDown(unsigned char nKey);
	void OnKeyUp(unsigned char nKey);
	void OnLeftClick();

	CCamera *GetCamera() {return m_pCamera;}
	void SetCamera(CCamera *pCamera);
	CPlatformBase &GetPlatform() const {return *m_pPlatform;}

	CGameBase *GetCurrentGame();

	int GetFrameID() const {return m_nFrameID;}

	bool GetReflectionsEnabled() const {return m_bReflectionsOn;}
	bool GetRefractionsEnabled() const {return m_bRefractionsOn;}
	bool GetShadingEnabled() const {return m_bShadingOn;}
	bool GetTexturesEnabled() const {return m_bTexturesOn;}
	EShadowsSetting GetShadowsSetting() const {return m_eShadowsSetting;}
	int GetRayBounceCount() const {return m_nMaxRayBounce;}
	bool GetRedBlue3dModeOn() const {return m_bRedBlue3dModeOn;}

	void SetReflectionsEnabled(bool bEnable);
	void SetRefractionsEnabled(bool bEnable);
	void SetShadingEnabled(bool bEnable);
	void SetTexturesEnabled(bool bEnable);
	void SetShadowsSetting(EShadowsSetting eShadowsSetting);
	void SetRayBounceCount(int nCount);
	void SetRedBlue3dModeOn(bool bEnable);

	void DirtyAllCachedData();

	unsigned char *GetPixelBufferRT() const {return m_pPixelBufferRT;}

	void GetCellsRenderedStats(int &nCellsRendered, int &nCellsUpdated, int &nTotalCells);
	void IncrementCellsRendered();
	void IncrementCellsUpdated();

	CUIStack &GetUIStack() {return m_UIStack;}

	int GetScreenWidth() const;
	int GetScreenHeight() const;

	CDebugMenu &GetDebugMenu() {return m_DebugMenu;}

	void PostRender();

	void ToggleWorldGrid();

	void SetResolution(int nWidth, int nHeight);

	AABB GetSceneAABB();

private:
	CRayTracer();
	~CRayTracer();

	//screen size
	int m_nScreenWidth;
	int m_nScreenHeight;

	//settings
	int m_nMaxRayBounce;
	bool m_bShadingOn;
	bool m_bTexturesOn;
	bool m_bReflectionsOn;
	EShadowsSetting m_eShadowsSetting;
	bool m_bRefractionsOn;
	bool m_bRedBlue3dModeOn;

	//other members
	CCamera *m_pCamera;
	CPlatformBase *m_pPlatform;
	int m_nFrameID;
	unsigned char *m_pPixelBufferRT;
	unsigned char *m_pPixelBufferRTAndUI;

	//for easier debugging
	static CRayTracer *s_pSingleTon;

	volatile unsigned int m_nCellsRenderedLastFrame;
	volatile unsigned int m_nCellsUpdatedLastFrame;

	//the debug menu
	CDebugMenu m_DebugMenu;

	//the ui sheet
	CUIStack m_UIStack;
};

#endif //RAYTRACER_H