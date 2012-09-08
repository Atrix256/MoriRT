/*======================================================

UISheet_Menu.cpp

The functionality for the main menu

======================================================*/

#include "UISheet_Menu.h"
#include "UIStack.h"
#include "../RayTracer.h"
#include "../PlatformBase.h"
#include "UIControl_TextButton.h"
#include <stdio.h>
#include <string.h>

CUISheet_Menu::CUISheet_Menu()
{
	char szBuffer[256];
	CPlatformBase &pPlatform = CRayTracer::GetSingleton().GetPlatform();

	m_bRecording = pPlatform.IsRecordingVideo();

	//int nTextHeight = pPlatform.GetTextHeight();

	CUIControl_TextButton *pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle Phong Shading");
	pTextButtonControl->SetClickCallback(OnTogglePhong,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	switch(CRayTracer::GetSingleton().GetShadowsSetting())
	{
		case CRayTracer::kShadowsOn:
		{
			strcpy(szBuffer,"Shadows: On");
			break;
		}
		case CRayTracer::kShadowsSimple:
		{
			strcpy(szBuffer,"Shadows: Simple");
			break;
		}
		case CRayTracer::kShadowsOff:
		{
			strcpy(szBuffer,"Shadows: Off");
			break;
		}
	}

	pTextButtonControl->SetText(szBuffer);
	pTextButtonControl->SetClickCallback(OnToggleShadows,this);
	m_TextButtons.push_back(pTextButtonControl);
	m_pShadowsButton = pTextButtonControl;

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle Reflections");
	pTextButtonControl->SetClickCallback(OnToggleReflections,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle Refractions");
	pTextButtonControl->SetClickCallback(OnToggleRefractions,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle Textures");
	pTextButtonControl->SetClickCallback(OnToggleTextures,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle Red Blue 3d");
	pTextButtonControl->SetClickCallback(OnToggleRedBlue3d,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle FPS");
	pTextButtonControl->SetClickCallback(OnToggleFPS,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle Camera Info");
	pTextButtonControl->SetClickCallback(OnToggleCamera,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle Cell Stats");
	pTextButtonControl->SetClickCallback(OnToggleCellStats,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle World Grid");
	pTextButtonControl->SetClickCallback(OnToggleWorldGrid,this);
	m_TextButtons.push_back(pTextButtonControl);

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Toggle Mouse Pos");
	pTextButtonControl->SetClickCallback(OnToggleMousePos,this);
	m_TextButtons.push_back(pTextButtonControl);

	sprintf(szBuffer,"Threads: %i / %i",CRayTracer::GetSingleton().GetPlatform().GetNumberActiveRenderThreads(),CRayTracer::GetSingleton().GetPlatform().GetNumberRenderThreads());
	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText(szBuffer);
	pTextButtonControl->SetClickCallback(OnClickThreads,this);
	m_TextButtons.push_back(pTextButtonControl);
	m_pThreadsButton = pTextButtonControl;

	sprintf(szBuffer,"Ray Bounces: %i",CRayTracer::GetSingleton().GetRayBounceCount());
	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText(szBuffer);
	pTextButtonControl->SetClickCallback(OnClickRayBounce,this);
	m_TextButtons.push_back(pTextButtonControl);
	m_pRayBounceButton = pTextButtonControl;

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Start Recording");
	pTextButtonControl->SetClickCallback(OnToggleRecording,this);
	m_TextButtons.push_back(pTextButtonControl);
	m_pRecordingButton = pTextButtonControl;
	m_pRecordingButton->SetText(m_bRecording?"Stop Recording":"Start Recording");

	pTextButtonControl = AddControl<CUIControl_TextButton>();
	pTextButtonControl->SetText("Exit");
	pTextButtonControl->SetClickCallback(OnExit,this);
	m_TextButtons.push_back(pTextButtonControl);


	int nMaxSize = 0;
	int nTotalHeight = 0;
	for(TextButtonList::iterator it = m_TextButtons.begin(); it != m_TextButtons.end(); ++it)
	{
		int nSize = (*it)->GetWidth();
		if(nSize > nMaxSize)
			nMaxSize = nSize;

		nTotalHeight += (*it)->GetHeight() + 5;
	}

	//there was an extra 5 added
	nTotalHeight-=5;

	m_nSheetWidth = nMaxSize + 20;
	m_nSheetPosX = (CRayTracer::GetSingleton().GetScreenWidth() - m_nSheetWidth) / 2;

	m_nSheetHeight = nTotalHeight + 20;
	m_nSheetPosY = (CRayTracer::GetSingleton().GetScreenHeight() - m_nSheetHeight) / 2;

	int nCurrentYPos = m_nSheetPosY + 10;
	int nXPos = m_nSheetPosX + 10;
	for(TextButtonList::iterator it = m_TextButtons.begin(); it != m_TextButtons.end(); ++it)
	{
		(*it)->SetWidth(nMaxSize);
		(*it)->SetPos(nXPos,nCurrentYPos);
		nCurrentYPos += (*it)->GetHeight() + 5;
	}
	
}

CUISheet_Menu::~CUISheet_Menu()
{
}

bool CUISheet_Menu::Internal_OnKeyDown(unsigned char nKey)
{
	return false;
}

bool CUISheet_Menu::Internal_OnKeyUp(unsigned char nKey)
{
	return false;
}

bool CUISheet_Menu::Internal_OnLeftClick()
{
	return false;
}

void CUISheet_Menu::Internal_Update(MRTFLOAT fFrameTime)
{
}

void CUISheet_Menu::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	pUIStack->Draw3dShaded2dBox(m_nSheetPosX,m_nSheetPosY,m_nSheetPosX+m_nSheetWidth,m_nSheetPosY+m_nSheetHeight,0.6f,0.3f,0.4f);
	CUISheet::Render(nOffsetX,nOffsetY,pUIStack);
}

void CUISheet_Menu::OnTogglePhong(void *pUserData)
{
	bool bEnable = !CRayTracer::GetSingleton().GetShadingEnabled();
	CRayTracer::GetSingleton().SetShadingEnabled(bEnable);
}

void CUISheet_Menu::OnToggleTextures(void *pUserData)
{
	bool bEnable = !CRayTracer::GetSingleton().GetTexturesEnabled();
	CRayTracer::GetSingleton().SetTexturesEnabled(bEnable);
}

void CUISheet_Menu::OnToggleRedBlue3d(void *pUserData)
{
	bool bEnable = !CRayTracer::GetSingleton().GetRedBlue3dModeOn();
	CRayTracer::GetSingleton().SetRedBlue3dModeOn(bEnable);
}

void CUISheet_Menu::OnToggleShadows(void *pUserData)
{
	static_cast<CUISheet_Menu *>(pUserData)->NonStaticOnToggleShadows();
}

void CUISheet_Menu::OnToggleReflections(void *pUserData)
{
	bool bEnable = !CRayTracer::GetSingleton().GetReflectionsEnabled();
	CRayTracer::GetSingleton().SetReflectionsEnabled(bEnable);
}

void CUISheet_Menu::OnToggleRefractions(void *pUserData)
{
	bool bEnable = !CRayTracer::GetSingleton().GetRefractionsEnabled();
	CRayTracer::GetSingleton().SetRefractionsEnabled(bEnable);
}

void CUISheet_Menu::OnExit(void *pUserData)
{
	CRayTracer::GetSingleton().GetPlatform().ExitApplication();
}

void CUISheet_Menu::OnToggleFPS(void *pUserData)
{
	CRayTracer::GetSingleton().GetDebugMenu().ToggleShowFPS();
}

void CUISheet_Menu::OnToggleCamera(void *pUserData)
{
	CRayTracer::GetSingleton().GetDebugMenu().ToggleShowCameraInfo();
}

void CUISheet_Menu::OnToggleCellStats(void *pUserData)
{
	CRayTracer::GetSingleton().GetDebugMenu().ToggleShowCellStats();

	CRayTracer::GetSingleton().DirtyAllCachedData();
}

void CUISheet_Menu::OnToggleWorldGrid(void *pUserData)
{
	CRayTracer::GetSingleton().ToggleWorldGrid();
}

void CUISheet_Menu::OnToggleMousePos(void *pUserData)
{
	CRayTracer::GetSingleton().GetDebugMenu().ToggleShowMousePos();
}

void CUISheet_Menu::OnClickThreads(void *pUserData)
{
	static_cast<CUISheet_Menu *>(pUserData)->NonStaticOnClickThreads();
}

void CUISheet_Menu::NonStaticOnClickThreads()
{
	int nNumActiveThreads = CRayTracer::GetSingleton().GetPlatform().GetNumberActiveRenderThreads();
	int nNumTotalThreads = CRayTracer::GetSingleton().GetPlatform().GetNumberRenderThreads();

	nNumActiveThreads--;

	if(nNumActiveThreads < 1 )
		nNumActiveThreads = nNumTotalThreads;

	CRayTracer::GetSingleton().GetPlatform().SetNumberActiveRenderThreads(nNumActiveThreads);

	char szBuffer[256];
	sprintf(szBuffer,"Threads: %i / %i",CRayTracer::GetSingleton().GetPlatform().GetNumberActiveRenderThreads(),CRayTracer::GetSingleton().GetPlatform().GetNumberRenderThreads());
	m_pThreadsButton->SetText(szBuffer);
}

void CUISheet_Menu::OnClickRayBounce(void *pUserData)
{
	static_cast<CUISheet_Menu *>(pUserData)->NonStaticOnClickRayBounce();
}

void CUISheet_Menu::OnToggleRecording(void *pUserData)
{
	static_cast<CUISheet_Menu *>(pUserData)->NonStaticOnToggleRecording();
}

void CUISheet_Menu::NonStaticOnClickRayBounce()
{
	int nCount = CRayTracer::GetSingleton().GetRayBounceCount();

	nCount++;

	if(nCount > 5)
		nCount = 0;

	CRayTracer::GetSingleton().SetRayBounceCount(nCount);

	char szBuffer[256];
	sprintf(szBuffer,"Ray Bounces: %i",CRayTracer::GetSingleton().GetRayBounceCount());
	m_pRayBounceButton->SetText(szBuffer);
}

void CUISheet_Menu::NonStaticOnToggleRecording()
{
	m_bRecording = !m_bRecording;
	m_pRecordingButton->SetText(m_bRecording?"Stop Recording":"Start Recording");

	if(m_bRecording)
		CRayTracer::GetSingleton().GetPlatform().StartRecordingVideo();
	else
		CRayTracer::GetSingleton().GetPlatform().StopRecordingVideo();
}

void CUISheet_Menu::NonStaticOnToggleShadows()
{
	switch(CRayTracer::GetSingleton().GetShadowsSetting())
	{
		case CRayTracer::kShadowsOff:
		{
			CRayTracer::GetSingleton().SetShadowsSetting(CRayTracer::kShadowsSimple);
			break;
		}
		case CRayTracer::kShadowsSimple:
		{
			CRayTracer::GetSingleton().SetShadowsSetting(CRayTracer::kShadowsOn);
			break;
		}
		case CRayTracer::kShadowsOn:
		{
			CRayTracer::GetSingleton().SetShadowsSetting(CRayTracer::kShadowsOff);
			break;
		}
	}

	char szBuffer[256];
	switch(CRayTracer::GetSingleton().GetShadowsSetting())
	{
		case CRayTracer::kShadowsOn:
		{
			strcpy(szBuffer,"Shadows: On");
			break;
		}
		case CRayTracer::kShadowsSimple:
		{
			strcpy(szBuffer,"Shadows: Simple");
			break;
		}
		case CRayTracer::kShadowsOff:
		{
			strcpy(szBuffer,"Shadows: Off");
			break;
		}
	}
	m_pShadowsButton->SetText(szBuffer);
}