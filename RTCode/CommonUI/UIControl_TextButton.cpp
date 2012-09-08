/*======================================================

UIControl_Textbutton.cpp

A text button

======================================================*/

#include "UIControl_TextButton.h"
#include "../RayTracer.h"
#include "../PlatformBase.h"
#include "UISheet.h"

CUIControl_TextButton::CUIControl_TextButton()
:m_nMargin(2)
,m_nForcedWidth(0)
,m_eButtonState(kNormal)
,m_pCallback(NULL)
,m_pUserData(NULL)
{
}

void CUIControl_TextButton::SetText(const char *szText)
{
	m_szButtonText = szText;
}

void CUIControl_TextButton::GetSize(int &nX1, int &nY1, int &nX2, int &nY2) const
{
	nX1 = nX2 = 0;
	nY1 = nY2 = 0;

	CRayTracer::GetSingleton().GetUIStack().GetTextRect(m_szButtonText.c_str(),nX1,nY1,nX2,nY2,CUIStack::kAlignmentCentered);

	//add the margin
	nX1 -= m_nMargin;
	nY1 -= m_nMargin;
	nX2 += m_nMargin;
	nY2 += m_nMargin;

	//apply a forced width if we need to
	if(m_nForcedWidth > 0)
	{
		int nMidPoint = (nX1 + nX2) / 2;

		nX1 = nMidPoint - m_nForcedWidth / 2;
		nX2 = nMidPoint + m_nForcedWidth / 2;
	}

	//make sure x1,y1 are at 0,0
	nX2 = nX2 - nX1;
	nY2 = nY2 - nY1;
	nX1 = 0;
	nY1 = 0;

	//move the box to the right position
	nX1 += m_nPosX;
	nY1 += m_nPosY;
	nX2 += m_nPosX;
	nY2 += m_nPosY;

	//add our parent's offset
	CUISheet *pParentSheet = GetParentSheet();
	if(pParentSheet)
	{
		int nParentPosX = pParentSheet->GetPositionX();
		int nParentPosY = pParentSheet->GetPositionY();

		nX1 += nParentPosX;
		nY1 += nParentPosY;
		nX2 += nParentPosX;
		nY2 += nParentPosY;
	}
}

int CUIControl_TextButton::GetWidth() const
{
	int nX1,nY1,nX2,nY2;

	GetSize(nX1,nY1,nX2,nY2);

	return nX2 - nX1;
}

int CUIControl_TextButton::GetHeight() const
{
	int nX1,nY1,nX2,nY2;

	GetSize(nX1,nY1,nX2,nY2);

	return nY2 - nY1;
}

void CUIControl_TextButton::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	int nX1,nY1,nX2,nY2;
	nX1 = 0;
	nY1 = 0;
	nX2 = 0;
	nY2 = 0;
	GetSize(nX1,nY1,nX2,nY2);

	switch(m_eButtonState)
	{
		case kNormal:
		{
			pUIStack->Draw3dShaded2dBox(nX1,nY1,nX2,nY2,0.6f,0.6f,0.6f); 
			break;
		}
		case kMouseOver:
		{
			pUIStack->Draw3dShaded2dBox(nX1,nY1,nX2,nY2,0.7f,0.7f,0.7f); 
			break;
		}
		case kMouseDown:
		{
			pUIStack->Draw3dShaded2dBox(nX1,nY1,nX2,nY2,0.7f,0.7f,0.7f,false); 
			break;
		}
	}

	nX1 = nX2 = (nX1 + nX2) / 2;
	nY1 += m_nMargin;

	if(m_eButtonState == kMouseDown)
	{
		nX1++;
		nY1++;
	}

	pUIStack->RenderText(nX1,nY1,m_szButtonText.c_str(),Vec3(0.0f,0.0f,0.0f),CUIStack::kAlignmentCentered);
}

void CUIControl_TextButton::Update(MRTFLOAT fFrameTime)
{
	if(!m_pParentSheet || !m_pParentSheet->HasFocus())
	{
		m_eButtonState = kNormal;
		return;
	}

	int nMouseX,nMouseY;
	CRayTracer::GetSingleton().GetPlatform().GetMousePosition(nMouseX,nMouseY);

	int nX1,nY1,nX2,nY2;
	GetSize(nX1,nY1,nX2,nY2);

	if(nMouseX >= nX1 && nMouseX <= nX2 &&
	   nMouseY >= nY1 && nMouseY <= nY2)
	{
		if(CRayTracer::GetSingleton().GetPlatform().GetLeftMouseDown())
			m_eButtonState = kMouseDown;
		else
			m_eButtonState = kMouseOver;
	}
	else
	{
		m_eButtonState = kNormal;
	}
}

bool CUIControl_TextButton::OnLeftClick()
{
	int nMouseX,nMouseY;
	CRayTracer::GetSingleton().GetPlatform().GetMousePosition(nMouseX,nMouseY);

	int nX1,nY1,nX2,nY2;
	GetSize(nX1,nY1,nX2,nY2);

	if(nMouseX >= nX1 && nMouseX <= nX2 &&
	   nMouseY >= nY1 && nMouseY <= nY2)
	{
		if(m_pCallback)
		{
			m_pCallback(m_pUserData);
		}
		return true;
	}
	else
	{
		return false;
	}
}