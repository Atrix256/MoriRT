/*======================================================

UISheet_MainMenu.cpp

The functionality for the dots game main menu

======================================================*/

#include "UISheet_MainMenu.h"
#include "../../CommonUI/UIStack.h"
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../CommonUI/UIControl_TextButton.h"

CUISheet_MainMenu::CUISheet_MainMenu()
:m_bHasFocus(false)
{
}

CUISheet_MainMenu::~CUISheet_MainMenu()
{
}

void CUISheet_MainMenu::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	if(!m_bHasFocus)
		return;

	//int nScreenWidth = CRayTracer::GetSingleton().GetScreenWidth();
	//int nScreenHeight = CRayTracer::GetSingleton().GetScreenHeight();
	//pUIStack->Draw3d(0,0,nScreenWidth,nScreenHeight,0.7f,0.7f,0.7f);

	CUISheet::Render(nOffsetX,nOffsetY,pUIStack);
}