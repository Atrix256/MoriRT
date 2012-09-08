/*======================================================

UISheet_Menu.cpp

The functionality for the main menu

======================================================*/

#include "UISheet_GameUI.h"
#include <stdio.h>
#include "../../CommonUI/UIStack.h"
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../CommonUI/UIControl_TextButton.h"

CUISheet_GameUI::CUISheet_GameUI()
:m_nScreenWidth(0)
,m_nScreenHeight(0)
,m_bIsPlayer1sTurn(true)
,m_nNumTiles(0)
,m_bHasFocus(false)
{
	m_nScore[0] = 0;
	m_nScore[1] = 0;
}

CUISheet_GameUI::~CUISheet_GameUI()
{
}

void CUISheet_GameUI::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	if(!m_bHasFocus)
		return;

	CUISheet::Render(nOffsetX,nOffsetY,pUIStack);

	m_nScreenWidth = CRayTracer::GetSingleton().GetScreenWidth();
	m_nScreenHeight = CRayTracer::GetSingleton().GetScreenHeight();

	int nTextHeight = pUIStack->GetTextHeight();
	if(m_bIsPlayer1sTurn)
	{
		pUIStack->RenderText(0,nTextHeight - nTextHeight / 2,"Player 1's Turn!",Vec3(0.0f,0.0f,1.0f),CUIStack::kAlignmentLeft);
	}
	else
	{
		pUIStack->RenderText(0,nTextHeight - nTextHeight / 2,"Player 2's Turn!",Vec3(1.0f,0.0f,0.0f),CUIStack::kAlignmentLeft);
	}

	char szBuffer[256];
	sprintf(szBuffer,"Player 1: %i     Player 2: %i     Total: %i",m_nScore[0],m_nScore[1],m_nNumTiles);
	pUIStack->RenderText(m_nScreenWidth,nTextHeight - nTextHeight / 2,szBuffer,Vec3(1.0f,1.0f,1.0f),CUIStack::kAlignmentRight);
}