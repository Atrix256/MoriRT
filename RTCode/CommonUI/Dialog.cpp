/*======================================================

Dialog.cpp

The base class for dialogs

======================================================*/

#include "Dialog.h"
#include "UIStack.h"

CDialog::CDialog()
{
}

CDialog::~CDialog()
{
}

void CDialog::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	//draw the background of the dialog
	pUIStack->Draw3dShaded2dBox(m_nPosX,m_nPosY,m_nPosX+m_nWidth,m_nPosY+m_nHeight,0.8f,0.8f,0.8f);

	//let the base class render everything else
	CUISheet::Render(nOffsetX,nOffsetY,pUIStack);
}