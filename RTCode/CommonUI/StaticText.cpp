/*======================================================

StaticText.cpp

Static text

======================================================*/

#include "StaticText.h"
#include "../RayTracer.h"
#include "../PlatformBase.h"
#include "UISheet.h"

CStaticText::CStaticText()
:m_eAlignment(CUIStack::kAlignmentLeft)
{
}

void CStaticText::SetText(const char *szText)
{
	m_szText = szText;
}

void CStaticText::GetSize(int &nX1, int &nY1, int &nX2, int &nY2) const
{
	nX1 = nX2 = 0;
	nY1 = nY2 = 0;

	CRayTracer::GetSingleton().GetUIStack().GetTextRect(m_szText.c_str(),nX1,nY1,nX2,nY2,CUIStack::kAlignmentCentered);

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

int CStaticText::GetWidth() const
{
	int nX1,nY1,nX2,nY2;

	GetSize(nX1,nY1,nX2,nY2);

	return nX2 - nX1;
}

int CStaticText::GetHeight() const
{
	int nX1,nY1,nX2,nY2;

	GetSize(nX1,nY1,nX2,nY2);

	return nY2 - nY1;
}

void CStaticText::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	pUIStack->RenderText(m_nPosX + nOffsetX,m_nPosY + nOffsetY,m_szText.c_str(),m_vColor,m_eAlignment);
}
