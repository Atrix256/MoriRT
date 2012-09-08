/*======================================================

MessageBox.cpp

The functionality for a generic message box dialog

======================================================*/

#include "MessageBox.h"
#include "StaticText.h"
#include "UIControl_TextButton.h"

CMessageBox::CMessageBox()
{
	//mark this dialog as modal (don't let events down through this dialog)
	SetModal(true);

	//set size and position
	m_nWidth = 300;
	m_nHeight = 100;
	m_nPosX = 100;
	m_nPosY = 200;

	//add our title text
	m_pText = AddControl<CStaticText>();
	m_pText->SetPos(m_nWidth/2,20);
	Vec3 vBlack(0.0f,0.0f,0.0f);
	m_pText->SetColor(vBlack);
	m_pText->SetAlignment(CUIStack::kAlignmentCentered);
	m_pText->SetText("");

	//add our OK button
	CUIControl_TextButton *pButton = AddControl<CUIControl_TextButton>();
	pButton->SetWidth(100);
	pButton->SetText("OK");
	pButton->SetPos(100,60);
	pButton->SetClickCallback(USE_MEMBERCALLBACK(OnOkClicked));
}

void CMessageBox::SetText(const char *szText)
{
	m_pText->SetText(szText);
}

void CMessageBox::OnOkClicked()
{
	Close();
}