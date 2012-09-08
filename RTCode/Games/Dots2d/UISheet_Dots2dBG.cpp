/*======================================================

UISheet_Dots2dGB.cpp

The functionality for the scrolling background image of dots2d
======================================================*/

#include "UISheet_Dots2dBG.h"
#include <stdio.h>
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../TextureManager.h"
#include "../../CommonUI/UIStack.h"
#include "../../CommonUI/TextBox.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../CommonUI/StaticText.h"
#include "NewAccountDialog.h"
#include "../../StringBuffer.h"
#include "../../WebService.h"
#include <math.h>

CUISheet_Dots2dBG::CUISheet_Dots2dBG()
:m_fCurrentTime(0)
{
	//add the background and title image
	m_pBackgroundImage = TextureManager.LoadTexture("Assets/Dots2d/background.png");
}

CUISheet_Dots2dBG::~CUISheet_Dots2dBG()
{
	TextureManager.FreeTexture(m_pBackgroundImage);
}

void CUISheet_Dots2dBG::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	float fScrollOffsetX = fmodf(m_fCurrentTime * 0.03f,1.0f);
	float fScrollOffsetY = fmodf(m_fCurrentTime * 0.01f,1.0f);

	pUIStack->RenderTexture(0,0,pUIStack->GetScreenWidth(),pUIStack->GetScreenHeight(),m_pBackgroundImage,fScrollOffsetX,fScrollOffsetY,fScrollOffsetX+1.0f,fScrollOffsetY+1.0f);

	CUISheet::Render(nOffsetX, nOffsetY, pUIStack);
}

void CUISheet_Dots2dBG::Update(MRTFLOAT fFrameTime)
{
	CUISheet::Update(fFrameTime);

	m_fCurrentTime += fFrameTime;
}