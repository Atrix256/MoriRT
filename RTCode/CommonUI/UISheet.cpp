/*======================================================

UISheet.cpp

The base functionality of the UI Sheets

======================================================*/

#include "UISheet.h"
#include "UIControl.h"
#include "../RayTracer.h"
#include "UIStack.h"

CUISheet::CUISheet()
:m_pFocusedControl(NULL)
,m_bModal(false)
,m_nPosX(0)
,m_nPosY(0)
,m_nWidth(0)
,m_nHeight(0)
{
	m_bWantsToClose = false;
	m_pCloseCallback = 0;
	m_pCloseCallbackArg = 0;
}

CUISheet::~CUISheet()
{
	//clear the UI controls
	for(UIControlList::iterator it = m_UIControlls.begin(); it != m_UIControlls.end(); ++it)
	{
		delete *it;
	}

	m_UIControlls.clear();
}

bool CUISheet::OnKeyTyped(unsigned char nKey)
{
	if(nKey == '	')
	{
		if(HasFocus())
			CycleFocus();
		return true;
	}

	for(UIControlList::reverse_iterator it = m_UIControlls.rbegin(); it != m_UIControlls.rend(); ++it)
	{
		if((*it)->OnKeyTyped(nKey))
			return true;
	}

	return Internal_OnKeyTyped(nKey);
}

bool CUISheet::OnKeyDown(unsigned char nKey)
{
	for(UIControlList::reverse_iterator it = m_UIControlls.rbegin(); it != m_UIControlls.rend(); ++it)
	{
		if((*it)->OnKeyDown(nKey))
			return true;
	}

	return Internal_OnKeyDown(nKey);
}

bool CUISheet::OnKeyUp(unsigned char nKey)
{
	for(UIControlList::reverse_iterator it = m_UIControlls.rbegin(); it != m_UIControlls.rend(); ++it)
	{
		if((*it)->OnKeyUp(nKey))
			return true;
	}

	return Internal_OnKeyUp(nKey);
}

bool CUISheet::OnLeftClick()
{
	for(UIControlList::reverse_iterator it = m_UIControlls.rbegin(); it != m_UIControlls.rend(); ++it)
	{
		if((*it)->OnLeftClick())
		{
			if(HasFocus() && (*it)->CanAcceptFocus())
			{
				m_pFocusedControl = (*it);
			}

			return true;
		}
	}

	return Internal_OnLeftClick();
}

void CUISheet::Update(MRTFLOAT fFrameTime)
{
	for(UIControlList::reverse_iterator it = m_UIControlls.rbegin(); it != m_UIControlls.rend(); ++it)
	{
		(*it)->Update(fFrameTime);
	}

	Internal_Update(fFrameTime);
}

void CUISheet::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	for(UIControlList::reverse_iterator it = m_UIControlls.rbegin(); it != m_UIControlls.rend(); ++it)
	{
		(*it)->Render(m_nPosX + nOffsetX,m_nPosY + nOffsetY,pUIStack);
	}
}

bool CUISheet::HasFocus() const
{
	return CRayTracer::GetSingleton().GetUIStack().GetTopOfStack() == this;
}

void CUISheet::OnControlDeleted(CUIControl *pControl)
{
	if(m_pFocusedControl == pControl)
	{
		m_pFocusedControl = NULL;
	}
}

CUIControl *CUISheet::GetFocusedControl()
{
	return m_pFocusedControl;
}

void CUISheet::SetFocus(CUIControl *pFocusControl)
{
	m_pFocusedControl = pFocusControl;
}

void CUISheet::CycleFocus()
{
	//find the index of what currently has focus
	int nCurrentFocus = -1;
	
	if(m_pFocusedControl)
	{
		nCurrentFocus = 0;
		for(UIControlList::iterator it = m_UIControlls.begin(); it != m_UIControlls.end(); ++it)
		{
			if((*it) == m_pFocusedControl)
			{
				break;
			}

			nCurrentFocus++;
		}
	}

	//now, find the next control after this one that accepts focus
	nCurrentFocus++;
	for(unsigned int nRelativeIndex = 0; nRelativeIndex < m_UIControlls.size(); nRelativeIndex++)
	{
		if(m_UIControlls[(nCurrentFocus+nRelativeIndex)%m_UIControlls.size()]->CanAcceptFocus())
		{
			m_pFocusedControl = m_UIControlls[(nCurrentFocus+nRelativeIndex)%m_UIControlls.size()];
			return;
		}
	}
}