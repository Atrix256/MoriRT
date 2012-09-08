/*======================================================

TextBox.cpp

A single line text input box

======================================================*/

#include "TextBox.h"
#include "../TextureManager.h"
#include "UIStack.h"
#include "../Texture.h"
#include "../RayTracer.h"
#include "../PlatformBase.h"
#include "UISheet.h"
#include <math.h>

CUIControl_TextBox::CUIControl_TextBox()
:m_fWidth(100.0f)
,m_fHeight(0.0f)
,m_fTime(0.0f)
,m_nTextCursorPos(0)
,m_bPasswordMode(false)
{
}

CUIControl_TextBox::~CUIControl_TextBox()
{
}

MRTFLOAT CUIControl_TextBox::GetHeight() const
{
	return m_fHeight;
}

void CUIControl_TextBox::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	//calculate the height of the box
	m_fHeight = ((MRTFLOAT)pUIStack->GetTextHeight()*1.5f);
	int nHeight = (int)m_fHeight;

	//m_bPasswordMode
	const char *szText = m_szString.c_str();
	std::string szPasswordBox;
	if(m_bPasswordMode)
	{
		int size = m_szString.length();
		while(size > 0)
		{
			szPasswordBox += "*";
			size--;
		}
		szText = szPasswordBox.c_str();
	}

	int nPosX = m_nPosX + nOffsetX;
	int nPosY = m_nPosY + nOffsetY;

	//draw the outline of the box
	if(HasFocus())
	{
		pUIStack->Draw3dShaded2dBox(nPosX,nPosY,nPosX+(int)m_fWidth,nPosY+nHeight,0.65f,0.65f,0.65f,false);

		if(fmodf(m_fTime,1.0f) > 0.5f)
		{
			int nStringWidth = pUIStack->GetTextWidth(szText);

			if(nStringWidth + pUIStack->GetCharacterWidth('_') < (int)m_fWidth)
			{
				pUIStack->RenderText(nPosX+nStringWidth,nPosY+5,"_",Vec3(0.0f,0.0f,0.0f),CUIStack::kAlignmentLeft);
			}
		}
	}
	else
		pUIStack->Draw3dShaded2dBox(nPosX,nPosY,nPosX+(int)m_fWidth,nPosY+nHeight,0.5f,0.5f,0.5f,false);

	pUIStack->RenderText(nPosX,nPosY+5,szText,Vec3(0.0f,0.0f,0.0f),CUIStack::kAlignmentLeft,(int)m_fWidth);
}

void CUIControl_TextBox::Update(MRTFLOAT fFrameTime)
{
	m_fTime += fFrameTime;
}

bool CUIControl_TextBox::OnLeftClick()
{
	int nMouseX,nMouseY;
	CRayTracer::GetSingleton().GetPlatform().GetMousePosition(nMouseX,nMouseY);

	nMouseX -= m_pParentSheet->GetPositionX();
	nMouseY -= m_pParentSheet->GetPositionY();

	int nX1,nY1,nX2,nY2;
	nX1 = m_nPosX;
	nY1 = m_nPosY;
	nX2 = nX1 + (int)m_fWidth;
	nY2 = nY1 + (int)m_fHeight;

	if(nMouseX >= nX1 && nMouseX <= nX2 &&
	   nMouseY >= nY1 && nMouseY <= nY2)
	{
		m_nTextCursorPos = 0;
		return true;
	}
	else
	{
		return false;
	}
}

bool CUIControl_TextBox::OnKeyTyped(unsigned char nKey)
{
	if(HasFocus())
	{
		if(nKey >= 0x20 && nKey <= 0x7F)
		{
			char newChar[2];
			newChar[0] = nKey;
			newChar[1] = 0;
			m_szString += newChar;
		}
		else if(nKey == 8)
		{
			int nSize = m_szString.size();
			if(nSize > 0)
			{
				m_szString = m_szString.substr(0, nSize-1);
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}