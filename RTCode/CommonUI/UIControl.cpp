/*======================================================

UIControl.cpp

The base functionality for UI controls

======================================================*/

#include "UIControl.h"
#include "UISheet.h"

CUIControl::CUIControl()
:m_pParentSheet(0)
,m_nPosX(0)
,m_nPosY(0)
{
}

CUIControl::~CUIControl()
{
	if(m_pParentSheet)
	{
		m_pParentSheet->OnControlDeleted(this);
	}
}

bool CUIControl::HasFocus()
{
	return m_pParentSheet && m_pParentSheet->HasFocus() && m_pParentSheet->GetFocusedControl() == this;
}