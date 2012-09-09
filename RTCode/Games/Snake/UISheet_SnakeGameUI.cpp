/*======================================================

UISheet_SnakeGameUI.h

The functionality for the game UI in the snake game

======================================================*/

#include "UISheet_SnakeGameUI.h"
#include <stdio.h>
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../TextureManager.h"
#include "../../Texture.h"
#include "../../CommonUI/UIStack.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../CommonUI/UIControl_ImageButton.h"
#include "Game_Snake.h"
#include "../../Scene.h"

CUISheet_SnakeGameUI::CUISheet_SnakeGameUI()
:m_bHasFocus(false)
,m_nScore(0)
,m_nHighScore(0)
,m_pGameScene(NULL)
{
	m_pQualityLow = AddControl<CUIControl_ImageButton>();
	m_pQualityLow->SetImageIdle("Assets/Snake/QualityLow_Idle.png");
	m_pQualityLow->SetImageOver("Assets/Snake/QualityLow_Over.png");
	m_pQualityLow->SetImageSelected("Assets/Snake/QualityLow_Selected.png");
	m_pQualityLow->SetClickCallback(StaticOnClickQualityLow,this);

	m_pQualityMedium = AddControl<CUIControl_ImageButton>();
	m_pQualityMedium->SetImageIdle("Assets/Snake/QualityMedium_Idle.png");
	m_pQualityMedium->SetImageOver("Assets/Snake/QualityMedium_Over.png");
	m_pQualityMedium->SetImageSelected("Assets/Snake/Qualitymedium_Selected.png");
	m_pQualityMedium->SetClickCallback(StaticOnClickQualityMedium,this);

	m_pQualityHigh = AddControl<CUIControl_ImageButton>();
	m_pQualityHigh->SetImageIdle("Assets/Snake/QualityHigh_Idle.png");
	m_pQualityHigh->SetImageOver("Assets/Snake/QualityHigh_Over.png");
	m_pQualityHigh->SetImageSelected("Assets/Snake/QualityHigh_Selected.png");
	m_pQualityHigh->SetClickCallback(StaticOnClickQualityHigh,this);
	
	m_nQualitySetting = 1;
	m_pQualityMedium->SetSelected(true);

	m_pScreenSmall = AddControl<CUIControl_ImageButton>();
	m_pScreenSmall->SetImageIdle("Assets/Snake/ScreenSmall_Idle.png");
	m_pScreenSmall->SetImageOver("Assets/Snake/ScreenSmall_Over.png");
	m_pScreenSmall->SetImageSelected("Assets/Snake/ScreenSmall_Selected.png");
	m_pScreenSmall->SetClickCallback(StaticOnClickScreenSmall,this);

	m_pScreenMedium = AddControl<CUIControl_ImageButton>();
	m_pScreenMedium->SetImageIdle("Assets/Snake/ScreenMedium_Idle.png");
	m_pScreenMedium->SetImageOver("Assets/Snake/ScreenMedium_Over.png");
	m_pScreenMedium->SetImageSelected("Assets/Snake/ScreenMedium_Selected.png");
	m_pScreenMedium->SetClickCallback(StaticOnClickScreenMedium,this);

	m_pScreenLarge = AddControl<CUIControl_ImageButton>();
	m_pScreenLarge->SetImageIdle("Assets/Snake/ScreenLarge_Idle.png");
	m_pScreenLarge->SetImageOver("Assets/Snake/ScreenLarge_Over.png");
	m_pScreenLarge->SetImageSelected("Assets/Snake/ScreenLarge_Selected.png");
	m_pScreenLarge->SetClickCallback(StaticOnClickScreenLarge,this);

	m_pScreenMedium->SetSelected(true);

	m_p3dMode = AddControl<CUIControl_ImageButton>();
	m_p3dMode->SetImageIdle("Assets/Snake/glasses_Idle.png");
	m_p3dMode->SetImageOver("Assets/Snake/glasses_Over.png");
	m_p3dMode->SetImageSelected("Assets/Snake/glasses_Selected.png");
	m_p3dMode->SetClickCallback(StaticOnClick3dMode,this);
}

void CUISheet_SnakeGameUI::SetScene(CScene *pScene)
{
	m_pGameScene = pScene;
}

CUISheet_SnakeGameUI::~CUISheet_SnakeGameUI()
{
}

void CUISheet_SnakeGameUI::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	CUISheet::Render(nOffsetX,nOffsetY,pUIStack);

	if(!m_bHasFocus)
		return;

	int nScreenWidth = CRayTracer::GetSingleton().GetScreenWidth();
	//int nScreenHeight = CRayTracer::GetSingleton().GetScreenHeight();

	//make sure the UI is in the right place (to handle resolution switching)
	PlaceIcons();

	int nTextHeight = pUIStack->GetTextHeight();
	char szBuffer[256];

	#if PIXELDEBUG == true
		pUIStack->RenderText(nTextHeight,nTextHeight,m_pGameScene->GetSceneDebugtext(),Vec3(1.0f,1.0f,1.0f),CUIStack::kAlignmentLeft);
	#else
		sprintf(szBuffer,"Score: %i",m_nScore);
		pUIStack->RenderText(nTextHeight,nTextHeight,szBuffer,Vec3(1.0f,1.0f,1.0f),CUIStack::kAlignmentLeft);
	#endif

	sprintf(szBuffer,"High Score: %i",m_nHighScore);
	Vec3 vColor;
	if(m_nScore == m_nHighScore)
	{
		if(CRayTracer::GetSingleton().GetRedBlue3dModeOn())
			vColor = Vec3(1.0f,1.0f,1.0f);
		else
			vColor = Vec3(1.0f,1.0f,0.0f);
	}
	else
	{
		if(CRayTracer::GetSingleton().GetRedBlue3dModeOn())
			vColor = Vec3(0.0f,1.0f,1.0f);
		else
			vColor = Vec3(0.0f,0.0f,1.0f);
	}

	pUIStack->RenderText(nScreenWidth - nTextHeight,nTextHeight,szBuffer,vColor,CUIStack::kAlignmentRight);

	//draw the fps
	sprintf(szBuffer,"%0.2f FPS",CRayTracer::GetSingleton().GetDebugMenu().GetAvgFPS());
	vColor = Vec3(0.4f,0.4f,0.1f);
	pUIStack->RenderText(nScreenWidth - nTextHeight,0,szBuffer,vColor,CUIStack::kAlignmentRight);
}

void CUISheet_SnakeGameUI::SetScore(int nScore)
{
	m_nScore = nScore;

	if(m_nScore > m_nHighScore)
	{
		m_nHighScore = m_nScore;
	}
}

void CUISheet_SnakeGameUI::OnClickQualityLow()
{
	m_nQualitySetting = 0;
	m_pQualityLow->SetSelected(true);
	m_pQualityMedium->SetSelected(false);
	m_pQualityHigh->SetSelected(false);

	m_pSnakeGame->ApplyQualitySettings();
}

void CUISheet_SnakeGameUI::OnClickQualityMedium()
{
	m_nQualitySetting = 1;
	m_pQualityLow->SetSelected(false);
	m_pQualityMedium->SetSelected(true);
	m_pQualityHigh->SetSelected(false);

	m_pSnakeGame->ApplyQualitySettings();
}

void CUISheet_SnakeGameUI::OnClickQualityHigh()
{
	m_nQualitySetting = 2;
	m_pQualityLow->SetSelected(false);
	m_pQualityMedium->SetSelected(false);
	m_pQualityHigh->SetSelected(true);

	m_pSnakeGame->ApplyQualitySettings();
}

void CUISheet_SnakeGameUI::OnClickScreenSmall()
{
	if(!m_pScreenSmall->GetSelected())
	{
		m_pScreenSmall->SetSelected(true);
		m_pScreenMedium->SetSelected(false);
		m_pScreenLarge->SetSelected(false);

		CRayTracer::GetSingleton().GetPlatform().SetResolution(300,300);

		PlaceIcons();
	}
}

void CUISheet_SnakeGameUI::OnClickScreenMedium()
{
	if(!m_pScreenMedium->GetSelected())
	{
		m_pScreenSmall->SetSelected(false);
		m_pScreenMedium->SetSelected(true);
		m_pScreenLarge->SetSelected(false);

		CRayTracer::GetSingleton().GetPlatform().SetResolution(500,500);

		PlaceIcons();
	}
}

void CUISheet_SnakeGameUI::OnClickScreenLarge()
{
	if(!m_pScreenLarge->GetSelected())
	{
		m_pScreenSmall->SetSelected(false);
		m_pScreenMedium->SetSelected(false);
		m_pScreenLarge->SetSelected(true);

		CRayTracer::GetSingleton().GetPlatform().SetResolution(1000,1000);

		PlaceIcons();
	}
}

void CUISheet_SnakeGameUI::OnClick3dMode()
{
	if(m_p3dMode->GetSelected())
	{
		m_p3dMode->SetSelected(false);
		CRayTracer::GetSingleton().SetRedBlue3dModeOn(false);
	}
	else
	{
		m_p3dMode->SetSelected(true);
		CRayTracer::GetSingleton().SetRedBlue3dModeOn(true);
	}

	//tell the game that we've changed 3d mode
	m_pSnakeGame->OnSet3dMode();
}

void CUISheet_SnakeGameUI::StaticOnClickQualityLow(void *pArg)
{
	static_cast<CUISheet_SnakeGameUI *>(pArg)->OnClickQualityLow();
}

void CUISheet_SnakeGameUI::StaticOnClickQualityMedium(void *pArg)
{
	static_cast<CUISheet_SnakeGameUI *>(pArg)->OnClickQualityMedium();
}

void CUISheet_SnakeGameUI::StaticOnClickQualityHigh(void *pArg)
{
	static_cast<CUISheet_SnakeGameUI *>(pArg)->OnClickQualityHigh();
}

void CUISheet_SnakeGameUI::StaticOnClickScreenSmall(void *pArg)
{
	static_cast<CUISheet_SnakeGameUI *>(pArg)->OnClickScreenSmall();
}

void CUISheet_SnakeGameUI::StaticOnClickScreenMedium(void *pArg)
{
	static_cast<CUISheet_SnakeGameUI *>(pArg)->OnClickScreenMedium();
}

void CUISheet_SnakeGameUI::StaticOnClickScreenLarge(void *pArg)
{
	static_cast<CUISheet_SnakeGameUI *>(pArg)->OnClickScreenLarge();
}

void CUISheet_SnakeGameUI::StaticOnClick3dMode(void *pArg)
{
	static_cast<CUISheet_SnakeGameUI *>(pArg)->OnClick3dMode();
}

void CUISheet_SnakeGameUI::PlaceIcons()
{
	int nScreenWidth = CRayTracer::GetSingleton().GetScreenWidth();
	int nScreenHeight = CRayTracer::GetSingleton().GetScreenHeight();

	int nYPos = 0;
	if(nScreenWidth == 500)
		nYPos = nScreenHeight - 77;
	else if(nScreenWidth == 300)
		nYPos = nScreenHeight - 47;
	else if(nScreenWidth == 1000)
		nYPos = nScreenHeight - 147;

	m_pQualityLow->SetPos(15,nYPos);
	m_pQualityMedium->SetPos(50,nYPos);
	m_pQualityHigh->SetPos(85,nYPos);

	m_pScreenSmall->SetPos(135,nYPos);
	m_pScreenMedium->SetPos(170,nYPos);
	m_pScreenLarge->SetPos(205,nYPos);

	m_p3dMode->SetPos(255,nYPos);
}