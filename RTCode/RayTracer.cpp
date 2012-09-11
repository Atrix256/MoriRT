/*======================================================

RayTracer.cpp

The entry point for raytracing

======================================================*/

#include "RayTracer.h"
#include "Game.h"
#include "CameraManager.h"
#include "Camera.h"
#include "PlatformBase.h"
#include "Scene.h"
#include <stdio.h>
#include <string.h>
#include "WebService.h"

CRayTracer *CRayTracer::s_pSingleTon = 0;

CRayTracer::CRayTracer()
:m_nScreenWidth(-1)
,m_nScreenHeight(-1)
,m_nMaxRayBounce(5)
,m_bShadingOn(true)
,m_bTexturesOn(true)
,m_bReflectionsOn(true)
,m_eShadowsSetting(kShadowsOn)
,m_bRefractionsOn(true)
,m_bRedBlue3dModeOn(false)
,m_pCamera(0)
,m_pPlatform(0)
,m_nFrameID(0)
,m_pPixelBufferRT(0)
,m_pPixelBufferRTAndUI(0)
,m_nCellsRenderedLastFrame(0)
,m_nCellsUpdatedLastFrame(0)
{
}

CRayTracer::~CRayTracer()
{
	delete[] m_pPixelBufferRT;
	m_pPixelBufferRT = 0;

	delete[] m_pPixelBufferRTAndUI;
	m_pPixelBufferRTAndUI = 0;
}

void CRayTracer::Init(CPlatformBase *pPlatform,int nScreenWidth, int nScreenHeight)
{
	m_pPlatform = pPlatform;

	SetResolution(nScreenWidth,nScreenHeight);

	GameInit(nScreenWidth, nScreenHeight);

	m_UIStack.Init();
}

CGameBase *CRayTracer::GetCurrentGame()
{
	return ::GetCurrentGame();
}

void CRayTracer::SetCamera(CCamera *pCamera)
{
	m_pCamera = pCamera;
	
	//todo: really, we just need to force a copy of the pixel buffer to the screen or something, but this works for now
	if(m_pCamera)
		m_pCamera->DirtyAllCachedData();
}

void CRayTracer::RenderFrame(MRTFLOAT fFrameTime)
{
	//give the platform a tick
	m_pPlatform->Update();

	//let the platform over-ride the frame time if it wants to (ie for recording video)
	m_pPlatform->PossiblyOverrideFrameTime(fFrameTime);

	//advance our frame id
	m_nFrameID++;

	//update the web service
	WebService::GetSingleton().Update(fFrameTime);

	//update the cameras
	CameraManager.Update();

	//Update the UI
	m_UIStack.Update(fFrameTime);

	//update the game logic
	GameUpdate(fFrameTime);

	//set the number of screen cells rendered to 0
	m_nCellsRenderedLastFrame = 0;
	m_nCellsUpdatedLastFrame = 0;

	//only render ray tracing stuff if there's a camera
	if(m_pCamera)
	{
		//tell the camera about any special rendering options
		m_pCamera->SetRenderCellBoundaries(GetDebugMenu().GetShowCellStats());
		m_pCamera->SetRedBlue3dMode(m_bRedBlue3dModeOn);

		//have the platform render to the pixel buffer, however it decides to (single or multithreaded) 
		m_pPlatform->Render(m_pCamera,m_nScreenWidth,m_nScreenHeight,m_nMaxRayBounce);

		//let the camera know that we are done rendering
		m_pCamera->PostRender(m_pPixelBufferRT,m_nScreenWidth);

		//copy from pixel buffer rt to pixel buffert rt and ui
		memcpy(m_pPixelBufferRTAndUI,m_pPixelBufferRT,m_nScreenWidth*m_nScreenHeight*4);
	}
	//else copy black to the buffer
	else
	{
		memset(m_pPixelBufferRTAndUI,0,m_nScreenWidth*m_nScreenHeight*4);
	}

	//render our UI to the screen buffer, not the pixel buffer that the ray casts use
	m_UIStack.SetRenderBuffer(m_pPixelBufferRTAndUI,m_nScreenWidth*4,m_nScreenWidth,m_nScreenHeight);
	m_UIStack.Render();

	//give the debug menu a chance to render what it wants to render
	m_DebugMenu.UpdateAndRender(&m_UIStack,fFrameTime);

	#if PIXELDEBUG
	if(m_pCamera)
	{
		m_pCamera->DirtyAllCachedData();
	}
	#endif
}

void CRayTracer::CopyFrameToPixels(unsigned char *pPixels, unsigned int nRowPitch)
{
	//copy from the pixel buffer to pPixels
	unsigned char *pCurrentPixelRow = m_pPixelBufferRTAndUI;
	unsigned char *pCurrentPixelDestRow = pPixels;
	int nBytesPerRow = m_nScreenWidth * 4;
	for(int nIndexY = 0; nIndexY < m_nScreenHeight; ++nIndexY)
	{
		//copy pixels
		memcpy(pCurrentPixelDestRow, pCurrentPixelRow, nBytesPerRow);

		//move to the next rows
		pCurrentPixelDestRow += nRowPitch;
		pCurrentPixelRow += nBytesPerRow;
	}
}

void CRayTracer::OnKeyTyped(unsigned char nKey)
{
	m_UIStack.OnKeyTyped(nKey);
}

void CRayTracer::OnKeyDown(unsigned char nKey)
{
	if(m_DebugMenu.OnKeyDown(nKey))
		return;

	if(m_UIStack.OnKeyDown(nKey))
		return;

	GameOnKeyDown(nKey);
}

void CRayTracer::OnKeyUp(unsigned char nKey)
{
	if(m_DebugMenu.OnKeyUp(nKey))
		return;

	if(m_UIStack.OnKeyUp(nKey))
		return;

	GameOnKeyUp(nKey);
}

void CRayTracer::OnLeftClick()
{
	if(m_DebugMenu.OnLeftClick())
		return;

	if(m_UIStack.OnLeftClick())
		return;

	GameOnLeftClick();
}

CRayTracer &CRayTracer::GetSingleton()
{
	static CRayTracer singleton;
	return singleton;
}

void CRayTracer::GetCellsRenderedStats(int &nCellsRendered, int &nCellsUpdated, int &nTotalCells)
{
	if(m_pCamera)
	{
		nCellsRendered = m_nCellsRenderedLastFrame;
		nCellsUpdated = m_nCellsUpdatedLastFrame;
		nTotalCells = m_pCamera->GetNumRenderCells();
	}
	else
	{
		nCellsRendered = 0;
		nCellsUpdated = 0;
		nTotalCells = 0;
	}
}

void CRayTracer::IncrementCellsRendered()
{
	m_nCellsRenderedLastFrame++;
}

void CRayTracer::IncrementCellsUpdated()
{
	m_nCellsUpdatedLastFrame++;
}

int CRayTracer::GetScreenWidth() const
{
	return m_nScreenWidth;
}

int CRayTracer::GetScreenHeight() const
{
	return m_nScreenHeight;
}

void CRayTracer::DirtyAllCachedData()
{
	//throw away all cached data
	m_pCamera->DirtyAllCachedData();
}

void CRayTracer::SetReflectionsEnabled(bool bEnable)
{
	m_bReflectionsOn = bEnable;

	//throw away all cached data
	if(m_pCamera)
		m_pCamera->DirtyAllCachedData();
}

void CRayTracer::SetRefractionsEnabled(bool bEnable)
{
	m_bRefractionsOn = bEnable;

	//throw away all cached data
	if(m_pCamera)
		m_pCamera->DirtyAllCachedData();
}

void CRayTracer::SetRedBlue3dModeOn(bool bEnable)
{
	m_bRedBlue3dModeOn = bEnable;

	//throw away all cached data
	if(m_pCamera)
		m_pCamera->DirtyAllCachedData();
}

void CRayTracer::SetShadingEnabled(bool bEnable)
{
	m_bShadingOn = bEnable;

	//throw away all cached data
	if(m_pCamera)
		m_pCamera->DirtyAllCachedData();
}

void CRayTracer::SetTexturesEnabled(bool bEnable)
{
	m_bTexturesOn = bEnable;

	//throw away all cached data
	if(m_pCamera)
		m_pCamera->DirtyAllCachedData();
}


void CRayTracer::SetShadowsSetting(EShadowsSetting eShadowsSetting)
{
	m_eShadowsSetting = eShadowsSetting;

	//throw away all cached data
	if(m_pCamera)
		m_pCamera->DirtyAllCachedData();
}

void CRayTracer::SetRayBounceCount(int nCount)
{
	m_nMaxRayBounce = nCount;

	//throw away all cached data
	if(m_pCamera)
		m_pCamera->DirtyAllCachedData();
}

void CRayTracer::PostRender()
{
	m_pPlatform->PostRender();
}

void CRayTracer::ToggleWorldGrid()
{
	if(m_pCamera)
	{
		CScene *pScene = m_pCamera->GetScene();

		if(pScene)
			pScene->ToggleWorldGrid();
	}
}

void CRayTracer::SetResolution(int nWidth, int nHeight)
{
	//bail out if nothing to do
	if(m_nScreenWidth > 0 &&
	   m_nScreenHeight > 0 &&
	   m_nScreenWidth == nWidth &&
	   m_nScreenHeight == nHeight)
	{
		return;
	}

	//tell the camera manager about the resolution switch
	CameraManager.SetResolution(nWidth,nHeight);

	//set our new size
	m_nScreenWidth = nWidth;
	m_nScreenHeight = nHeight;

	//GameInit(nScreenWidth, nScreenHeight);

	//re-alloc the pixel buffers
	delete[] m_pPixelBufferRT;
	delete[] m_pPixelBufferRTAndUI;
	m_pPixelBufferRT = new unsigned char[m_nScreenWidth * m_nScreenHeight * 4];
	m_pPixelBufferRTAndUI = new unsigned char[m_nScreenWidth * m_nScreenHeight * 4];
}

AABB CRayTracer::GetSceneAABB()
{
	AABB aabb;

	if(m_pCamera && m_pCamera->GetScene())
	{
		aabb = m_pCamera->GetScene()->GetActualAABB();
	}

	return aabb;
}