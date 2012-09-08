/*======================================================

UISheet_Lost.cpp

The functionality for the dialog you see when you lose

======================================================*/

#include "UISheet_Lost.h"
#include "../../CommonUI/UIStack.h"
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../TextureManager.h"
#include "../../Texture.h"

CUISheet_Lost::CUISheet_Lost()
{
	m_pTexture = TextureManager.LoadTexture("Assets/Snake/Crash.png");
}

CUISheet_Lost::~CUISheet_Lost()
{
	TextureManager.FreeTexture(m_pTexture);
}

void CUISheet_Lost::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
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

	nTextureHeight = nTextureWidth / 4;

	int nTextureX = nScreenWidth / 2 - nTextureWidth / 2;
	int nTextureY = nScreenHeight - nTextureHeight - nScreenHeight / 10;

	pUIStack->RenderTextureAlphaBlend(nTextureX,nTextureY,nTextureWidth,nTextureHeight,m_pTexture,Vec3(1.0f,1.0f,1.0f));

	CUISheet::Render(nOffsetX,nOffsetY,pUIStack);
}

bool CUISheet_Lost::OnLeftClick()
{
	Close();
	return true;
}

bool CUISheet_Lost::OnKeyDown(unsigned char nKey)
{
	return true;
}

bool CUISheet_Lost::OnKeyUp(unsigned char nKey)
{
	if(nKey == ' ')
		Close();

	return true;
}