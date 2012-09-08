/*======================================================

UIControl_ImageButton.cpp

An image button

======================================================*/

#include "UIControl_ImageButton.h"
#include "../RayTracer.h"
#include "../PlatformBase.h"
#include "../TextureManager.h"
#include "../Texture.h"
#include "UISheet.h"

CUIControl_ImageButton::CUIControl_ImageButton()
:m_fScale(1.0f)
,m_bSelected(false)
,m_pCallback(NULL)
,m_pUserData(NULL)
,m_pTextureIdle(NULL)
,m_pTextureOver(NULL)
,m_pTextureSelected(NULL)
{
	//set to invalid images to start out
	SetImageIdle("");
	SetImageOver("");
	SetImageSelected("");
}

void CUIControl_ImageButton::SetImageIdle(const char *szFileName)
{
	if(m_pTextureIdle)
		TextureManager.FreeTexture(m_pTextureIdle);

	m_pTextureIdle = TextureManager.LoadTexture(szFileName);
}

void CUIControl_ImageButton::SetImageOver(const char *szFileName)
{
	if(m_pTextureOver)
		TextureManager.FreeTexture(m_pTextureOver);

	m_pTextureOver = TextureManager.LoadTexture(szFileName);
}

void CUIControl_ImageButton::SetImageSelected(const char *szFileName)
{
	if(m_pTextureSelected)
		TextureManager.FreeTexture(m_pTextureSelected);

	m_pTextureSelected = TextureManager.LoadTexture(szFileName);
}

void CUIControl_ImageButton::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	int nWidth = (int)((MRTFLOAT)m_pTextureIdle->GetWidth() * m_fScale);
	int nHeight = (int)((MRTFLOAT)m_pTextureIdle->GetHeight() * m_fScale);

	if(m_bSelected)
		pUIStack->RenderTexture(m_nPosX,m_nPosY,nWidth,nHeight,m_pTextureSelected);
	else
	{
		int nMouseX,nMouseY;
		CRayTracer::GetSingleton().GetPlatform().GetMousePosition(nMouseX,nMouseY);

		if(m_pParentSheet->HasFocus() &&
		   nMouseX >= m_nPosX && nMouseX < m_nPosX + nWidth &&
		   nMouseY >= m_nPosY && nMouseY < m_nPosY + nHeight)
		{
			pUIStack->RenderTexture(m_nPosX,m_nPosY,nWidth,nHeight,m_pTextureOver);
		}
		else
		{
			pUIStack->RenderTexture(m_nPosX,m_nPosY,nWidth,nHeight,m_pTextureIdle);
		}
	}
}

bool CUIControl_ImageButton::OnLeftClick()
{
	int nWidth = (int)((MRTFLOAT)m_pTextureIdle->GetWidth() * m_fScale);
	int nHeight = (int)((MRTFLOAT)m_pTextureIdle->GetHeight() * m_fScale);

	int nMouseX,nMouseY;
	CRayTracer::GetSingleton().GetPlatform().GetMousePosition(nMouseX,nMouseY);

	if(m_pParentSheet->HasFocus() &&
		nMouseX >= m_nPosX && nMouseX < m_nPosX + nWidth &&
		nMouseY >= m_nPosY && nMouseY < m_nPosY + nHeight)
	{
		if(m_pCallback)
		{
			m_pCallback(m_pUserData);
		}		

		return true;
	}

	return false;
}