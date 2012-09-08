/*======================================================

DebugMenu.cpp

The functionality for the debug menu brought up by pressing ~

======================================================*/

#include "DebugMenu.h"
#include "RayTracer.h"
#include "PlatformBase.h"
#include <stdio.h>
#include "CommonUI/UISheet_Menu.h"
#include "Camera.h"
#include <stdio.h>
#include <string.h>
#include "Game.h"
#include "AABB.h"
#include "CommonUI/UIStack.h"

CDebugMenu::CDebugMenu()
{
	m_pDebugMenuSheet = NULL;
	m_bShowFPS = false;
	m_bShowCameraInfo = false;
	m_bShowCellStats = false;
	m_bShowMousePos = false;

	m_fAvgFPSTimeBucket = 0.0f;
	m_nAvgFPSFrameBucket = 0;
	m_fAvgFPSLastValue = 0.0f;
}

CDebugMenu::~CDebugMenu()
{
}

bool CDebugMenu::OnKeyDown(unsigned char nKey)
{
	return IsMenuOpen();
}

bool CDebugMenu::OnKeyUp(unsigned char nKey)
{
	if(IsMenuOpen() && nKey == 27)
	{
		CloseMenu();
		return true;
	}

#if FINALGAME != GAME_NONE
	static int nState = 0;

	switch(nState)
	{
		case 0:
		{
			if(nKey == 'M')
				nState++;
			else
				nState = 0;
			break;
		}
		case 1:
		{
			if(nKey == '0')
				nState++;
			else
				nState = 0;
			break;
		}
		case 2:
		{
			if(nKey == '0')
				nState++;
			else
				nState = 0;
			break;
		}
		case 3:
		{
			if(nKey == 'F')
			{
				if(!IsMenuOpen())
					OpenMenu();
			}

			nState = 0;
			break;
		}
	}

#else
	if(nKey=='~')
	{
		if(IsMenuOpen())
		{
			CloseMenu();
		}
		else
		{
			OpenMenu();
		}
	}
#endif

	return IsMenuOpen();
}

bool CDebugMenu::OnLeftClick()
{
	return false;
}

void CDebugMenu::UpdateAndRender(CUIStack *pUIStack, MRTFLOAT fFrameTime)
{
	m_fAvgFPSTimeBucket += fFrameTime;
	m_nAvgFPSFrameBucket++;

	if(m_fAvgFPSTimeBucket > 2.0f && m_nAvgFPSFrameBucket > 0)
	{
		m_fAvgFPSLastValue = ((MRTFLOAT)m_nAvgFPSFrameBucket / m_fAvgFPSTimeBucket);
		m_nAvgFPSFrameBucket = 0;
		m_fAvgFPSTimeBucket = 0.0f;
	}

	CPlatformBase &pPlatform = CRayTracer::GetSingleton().GetPlatform();
	char szBuffer[256];

	int nYPos = 0;
	int nTextHeight = pUIStack->GetTextHeight();

	if(m_bShowFPS)
	{
		sprintf(szBuffer,"%ims (%0.2f fps) (%0.2f avg fps)",(int)(fFrameTime*1000.0f), 1.0f/fFrameTime,m_fAvgFPSLastValue);
		pUIStack->RenderText(0,nYPos*(nTextHeight + 3),szBuffer,Vec3(0.9f,0.9f,0.1f),CUIStack::kAlignmentLeft);
		nYPos++;
	}

	if(m_bShowCameraInfo)
	{
		CCamera *pCamera = CRayTracer::GetSingleton().GetCamera();

		if(pCamera)
		{
			if(pCamera->GetOrthographic())
			{
				strcpy(szBuffer,"Camera: Ortho");
			}
			else
			{
				Vec3 vCameraPos = pCamera->GetPos();
				Vec3 vCameraFwd = pCamera->GetFwd();
				sprintf(szBuffer,"Camera: Pos (%0.2f,%0.2f,%0.2f) Fwd (%0.2f,%0.2f,%0.2f)",vCameraPos.m_fX,vCameraPos.m_fY,vCameraPos.m_fZ,vCameraFwd.m_fX,vCameraFwd.m_fY,vCameraFwd.m_fZ);
			}
		}
		else
		{
			strcpy(szBuffer,"Camera: <None>");
		}
		
		pUIStack->RenderText(0,nYPos*(nTextHeight + 3),szBuffer,Vec3(0.9f,0.9f,0.1f),CUIStack::kAlignmentLeft);
		nYPos++;

		//say the actual aabb
		AABB aabb = CRayTracer::GetSingleton().GetSceneAABB();
		sprintf(szBuffer,"Scene AABB: (%0.2f,%0.2f,%0.2f) - (%0.2f,%0.2f,%0.2f)",aabb.m_vMin.m_fX,aabb.m_vMin.m_fY,aabb.m_vMin.m_fZ,aabb.m_vMax.m_fX,aabb.m_vMax.m_fY,aabb.m_vMax.m_fZ);
		pUIStack->RenderText(0,nYPos*(nTextHeight + 3),szBuffer,Vec3(0.9f,0.9f,0.1f),CUIStack::kAlignmentLeft);
		nYPos++;
	}

	if(m_bShowCellStats)
	{
		int nCellsRendered,nCellsUpdated,nTotalCells;
		CRayTracer::GetSingleton().GetCellsRenderedStats(nCellsRendered,nCellsUpdated,nTotalCells);
		sprintf(szBuffer,"%i / %i / %i  out of %i",nCellsRendered,nCellsUpdated,nTotalCells - (nCellsRendered+nCellsUpdated),nTotalCells);
		pUIStack->RenderText(0,nYPos*(nTextHeight + 3),szBuffer,Vec3(0.9f,0.9f,0.1f),CUIStack::kAlignmentLeft);
		nYPos++;
	}

	if(m_bShowMousePos)
	{
		int nMouseX,nMouseY;
		pPlatform.GetMousePosition(nMouseX,nMouseY);
		sprintf(szBuffer,"Mouse: %i, %i",nMouseX,nMouseY);
		pUIStack->RenderText(0,nYPos*(nTextHeight + 3),szBuffer,Vec3(0.9f,0.9f,0.1f),CUIStack::kAlignmentLeft);
		nYPos++;
	}
}

void CDebugMenu::OpenMenu()
{
	m_pDebugMenuSheet = CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_Menu>();
}

void CDebugMenu::CloseMenu()
{
	m_pDebugMenuSheet->Close();
	m_pDebugMenuSheet = NULL;
}