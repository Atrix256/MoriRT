/*======================================================

UISheet_SnakeIntroUI.cpp

The functionality for the dialog you see when you start the game

======================================================*/

#include "UISheet_SnakeIntroUI.h"
#include "../../CommonUI/UIStack.h"
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../TextureManager.h"
#include "../../Texture.h"

CUISheet_SnakeIntro::CUISheet_SnakeIntro()
{
	m_pTexture = TextureManager.LoadTexture("Assets/Snake/Intro.png");
}

CUISheet_SnakeIntro::~CUISheet_SnakeIntro()
{
	TextureManager.FreeTexture(m_pTexture);
}

void CUISheet_SnakeIntro::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	int nScreenWidth = CRayTracer::GetSingleton().GetScreenWidth();
	int nScreenHeight = CRayTracer::GetSingleton().GetScreenHeight();

	int nTextureWidth = 0;
	int nTextureHeight = 0;

	if(nScreenWidth < nScreenHeight)
	{
		nTextureWidth = nTextureHeight = (int)((MRTFLOAT)nScreenWidth * 0.9f);
	}
	else
	{
		nTextureWidth = nTextureHeight = (int)((MRTFLOAT)nScreenHeight * 0.9f);
	}

	if(nTextureWidth > 256)
	{
		nTextureWidth = nTextureHeight = 256;
	}

	int nTextureX = nScreenWidth / 2 - nTextureWidth / 2;
	int nTextureY = nScreenHeight / 2 - nTextureHeight / 2;

	pUIStack->RenderTextureAlphaBlend(nTextureX,nTextureY,nTextureWidth,nTextureHeight,m_pTexture,Vec3(1.0f,1.0f,1.0f));

	CUISheet::Render(nOffsetX,nOffsetY,pUIStack);
}

bool CUISheet_SnakeIntro::OnLeftClick()
{
	Close();
	return true;
}

bool CUISheet_SnakeIntro::OnKeyDown(unsigned char nKey)
{
	return true;
}

bool CUISheet_SnakeIntro::OnKeyUp(unsigned char nKey)
{
	/*
	//dont let normal movement keys close this window, to prevent accidentally closing the window
	if((nKey >= 37 && nKey <= 40) || nKey == 'W' || nKey == 'A' || nKey == 'S' || nKey == 'D')
		return true;

	Close();
	*/
	return true;
}