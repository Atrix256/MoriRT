/*======================================================

Game_Dots.cpp

The logic for the dots game

======================================================*/

#include "Game_Dots.h"
#include "UISheet_GameUI.h"
#include "../../SceneManager.h"
#include "../../PrimitiveBase.h"
#include "../../PrimitiveBox.h"
#include "../../PrimitiveSphere.h"
#include "../../PrimitiveTriangle.h"
#include "../../Camera.h"
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../CameraManager.h"
#include <math.h>

const MRTFLOAT CGameDots::s_fCellWidth =  (1.0f) / (MRTFLOAT)s_nGridCellsX;
const MRTFLOAT CGameDots::s_fCellHeight = (1.0f) / (MRTFLOAT)s_nGridCellsY;

const MRTFLOAT CGameDots::s_fCameraFov = 45.0f;
const MRTFLOAT CGameDots::s_fCameraRenderRectSize = 0.5f;

const MRTFLOAT CGameDots::s_fWorldGridSize = 1.0f / (MRTFLOAT)s_nScreenGridCellCount;

const Vec3 CGameDots::s_CameraPos( 0.5f, 0.5f,-0.2f);
const Vec3 CGameDots::s_TargetPos( 0.5f, 0.5f, 1.0f);
const Vec3 CGameDots::s_CameraUp ( 0.0f, 1.0f, 0.0f);

CGameDots::CGameDots()
{
	m_pDotsScene = NULL;
	m_pRectangleSelectionPrimitive = NULL;
	m_pCamera = NULL;

	m_bHorizontalLineSelected = false;
	m_nSelectedLineIndexX = 0;
	m_nSelectedLineIndexY = 0;

	m_fGameTime = 0.0f;

	m_bPlayer1sTurn = true;

	m_pGameUI = NULL;
}

void CGameDots::Populate(int nScreenWidth, int nScreenHeight)
{
	//create the camera
	m_pCamera = CameraManager.AddCamera(s_CameraPos,         //eye
										s_TargetPos,          //target
										s_CameraUp,           //up
										nScreenWidth,         //pixel width
										nScreenHeight,        //pixel height
										s_fCameraFov,         //FOV
										s_fCameraRenderRectSize,
										s_nScreenGridCellCount);

	//make the camera orthographic
	//m_pCamera->SetModeOrthographic();

	//add our scene
	m_pDotsScene = SceneManager.AddScene(s_fWorldGridSize, Vec3(0,0,0), Vec3(1,1,1));
	m_pDotsScene->SetAmbientLight(Vec3(0.0f,0.0f,0.0f));
	m_pCamera->SetScene(m_pDotsScene);

	//create our game UI and tell it some info
	m_pGameUI = CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_GameUI>();
	m_pGameUI->SetPlayer1sTurn(m_bPlayer1sTurn);
	m_pGameUI->SetTotalTiles((s_nGridCellsX - 1) * (s_nGridCellsY - 1));

	//make our grid
	MakeGrid();

	//lights
	CPrimitiveBase *pPrimitive = m_pDotsScene->CreateSphere(Vec3(0.7f,0.7f,-1.0f), 0.01f);
	pPrimitive->SetColor(Vec3(1,1,1));
	pPrimitive->SetIsLight(true);

	//our selection marker
	m_pRectangleSelectionPrimitive = m_pDotsScene->CreateBox(Vec3(-10.0f,-10.0f,-1.0f),Vec3(0.05f,0.05f,0.05f));
	m_pRectangleSelectionPrimitive->SetEmissiveColor(Vec3(0.2f,0.2f,0.2f));
}

void CGameDots::OnView(bool bGainingFocus)
{
	if(bGainingFocus)
	{
		CRayTracer::GetSingleton().SetCamera(m_pCamera);
	}

	m_pGameUI->OnView(bGainingFocus);
}

void CGameDots::Update(MRTFLOAT fFrameTime)
{
	//animate the color of our selection primitive
	m_fGameTime += fFrameTime;
	MRTFLOAT fColor = ((sinf(m_fGameTime * 8.0f) + 1.0f) / 2.0f) * 0.75f;

	if(m_bPlayer1sTurn)
		m_pRectangleSelectionPrimitive->SetColor(Vec3(0.0f,0.0f,0.25f + fColor));
	else
		m_pRectangleSelectionPrimitive->SetColor(Vec3(0.25f + fColor,0.0f,0.0f));


	//get the mouse position
	int nMouseX, nMouseY;
	CRayTracer::GetSingleton().GetPlatform().GetMousePosition(nMouseX,nMouseY);

	CCamera *pCamera = CRayTracer::GetSingleton().GetCamera();
	if(!pCamera)
	{
		return;
	}

	//get the ray for where our mouse is, if we can
	Ray MouseRay;
	if(!pCamera->GetRay(nMouseX,nMouseY,MouseRay))
	{
		return;
	}

	//calculate where our ray hits the Z = 1 plane
	MRTFLOAT fTimeTilZOne = (1.0f - MouseRay.m_vPos.m_fZ) / MouseRay.m_vDir.m_fZ;
	Vec3 vPos = MouseRay.m_vPos + MouseRay.m_vDir * fTimeTilZOne;


	#if 1

	MRTFLOAT fPercentX = vPos.m_fX;
	MRTFLOAT fPercentY = vPos.m_fY;

	#else
	//get the screen dimensions
	int nScreenWidth = CRayTracer::GetSingleton().GetScreenWidth();
	int nScreenHeight = CRayTracer::GetSingleton().GetScreenHeight();

	//get the mouse position as a percent of the screen
	MRTFLOAT fPercentX = ((MRTFLOAT)nMouseX) / ((MRTFLOAT)nScreenWidth);
	MRTFLOAT fPercentY = 1.0f - ((MRTFLOAT)nMouseY) / ((MRTFLOAT)nScreenHeight);
	#endif

	//calculate which cell the mouse is in
	int nCellX = (int)floorf(fPercentX * (MRTFLOAT)s_nGridCellsX + 0.5f);
	int nCellY = (int) ceilf(fPercentY * (MRTFLOAT)s_nGridCellsY - 0.5f);

	if(nCellX < 1)
		nCellX = 1;
	else if(nCellX > s_nGridCellsX - 1)
		nCellX =  s_nGridCellsX - 1;

	if(nCellY < 1)
		nCellY = 1;
	else if(nCellY > s_nGridCellsY - 1)
		nCellY = s_nGridCellsY - 1;

	//calculate our distances from the edges
	MRTFLOAT fDistX = fPercentX - ((MRTFLOAT)nCellX) * s_fCellWidth;
	MRTFLOAT fDistY = fPercentY - ((MRTFLOAT)nCellY) * s_fCellHeight;

	m_nSelectedLineIndexX = nCellX;
	m_nSelectedLineIndexY = nCellY;

	//adjust our selection rectangle as appropriate
	if(fabsf(fDistX / s_fCellWidth) > fabsf(fDistY / s_fCellHeight))
	{
		MRTFLOAT fOffset = fDistX > 0.0f ? 0.5f : -0.5f ;

		if(fDistX > 0.0f)
			m_nSelectedLineIndexX++;

		Vec3 vOldPos = m_pRectangleSelectionPrimitive->GetPosition();
		Vec3 vNewPos(((MRTFLOAT)nCellX + fOffset) * s_fCellWidth,((MRTFLOAT)nCellY) * s_fCellHeight,1);
		vOldPos -= vNewPos;
		if(vOldPos.Length() > 0.01f)
		{
			m_pRectangleSelectionPrimitive->SetPosition(vNewPos);
			m_pRectangleSelectionPrimitive->SetScale(Vec3(0.01f,s_fCellHeight,0.05f));
		}

		m_bHorizontalLineSelected = false;
	}
	else
	{
		MRTFLOAT fOffset = fDistY > 0.0f ? 0.5f : -0.5f ;

		if(fDistY > 0.0f)
			m_nSelectedLineIndexY++;

		Vec3 vOldPos = m_pRectangleSelectionPrimitive->GetPosition();
		Vec3 vNewPos(((MRTFLOAT)nCellX) * s_fCellWidth,((MRTFLOAT)nCellY + fOffset) * s_fCellHeight,1);
		vOldPos -= vNewPos;
		if(vOldPos.Length() > 0.01f)
		{
			m_pRectangleSelectionPrimitive->SetPosition(vNewPos);
			m_pRectangleSelectionPrimitive->SetScale(Vec3(s_fCellWidth,0.01f,0.05f));
		}

		m_bHorizontalLineSelected = true;
	}
}

void CGameDots::OnLeftClick()
{
	int nX = m_nSelectedLineIndexX - 1;
	int nY = m_nSelectedLineIndexY - 1;

	if(m_bHorizontalLineSelected)
	{
		if(m_GridCells[nX][nY].m_eLineHoriz == kUnplayed)
		{
			m_GridCells[nX][nY].m_eLineHoriz = m_bPlayer1sTurn ? kPlayer1 : kPlayer2;
			m_GridCells[nX][nY].m_pPrimitiveHoriz = m_pDotsScene->CreateBox(Vec3(((MRTFLOAT)nX + 1.0f)*s_fCellWidth,((MRTFLOAT)nY + 0.5f)*s_fCellHeight,1.0f),Vec3(s_fCellWidth,0.01f,0.06f));

			if(m_bPlayer1sTurn)
			{
				m_GridCells[nX][nY].m_pPrimitiveHoriz->SetColor(Vec3(0.0f,0.0f,1.0f));
				m_GridCells[nX][nY].m_pPrimitiveHoriz->SetEmissiveColor(Vec3(0.0f,0.0f,0.1f));
			}
			else
			{
				m_GridCells[nX][nY].m_pPrimitiveHoriz->SetColor(Vec3(1.0f,0.0f,0.0f));
				m_GridCells[nX][nY].m_pPrimitiveHoriz->SetEmissiveColor(Vec3(0.1f,0.0f,0.0f));
			}

			if(!CheckBoardForBoxCompletions())
				OnTurnPlayed();
		}
	}
	else
	{
		if(m_GridCells[nX][nY].m_eLineVert == kUnplayed)
		{
			m_GridCells[nX][nY].m_eLineVert = m_bPlayer1sTurn ? kPlayer1 : kPlayer2;
			m_GridCells[nX][nY].m_pPrimitiveVert = m_pDotsScene->CreateBox(Vec3(((MRTFLOAT)nX + 0.5f)*s_fCellWidth,((MRTFLOAT)nY + 1.0f)*s_fCellHeight,1.0f),Vec3(0.01f,s_fCellHeight,0.06f));

			if(m_bPlayer1sTurn)
			{
				m_GridCells[nX][nY].m_pPrimitiveVert->SetColor(Vec3(0.0f,0.0f,1.0f));
				m_GridCells[nX][nY].m_pPrimitiveVert->SetEmissiveColor(Vec3(0.0f,0.0f,0.1f));
			}
			else
			{
				m_GridCells[nX][nY].m_pPrimitiveVert->SetColor(Vec3(1.0f,0.0f,0.0f));
				m_GridCells[nX][nY].m_pPrimitiveVert->SetEmissiveColor(Vec3(0.1f,0.0f,0.0f));
			}

			if(!CheckBoardForBoxCompletions())
				OnTurnPlayed();
		}
	}
}

void CGameDots::MakeGrid()
{
	MRTFLOAT fOffsetX = s_fCellWidth / 2.0f;
	MRTFLOAT fOffsetY = s_fCellHeight / 2.0f;

	for(int nIndexX = 0; nIndexX < s_nGridCellsX; ++nIndexX)
	{
		for(int nIndexY = 0; nIndexY < s_nGridCellsY; ++nIndexY)
		{
			SGridCell &Cell = m_GridCells[nIndexX][nIndexY];

			MRTFLOAT fPositionX = ((MRTFLOAT)nIndexX) * s_fCellWidth + fOffsetX;
			MRTFLOAT fPositionY = ((MRTFLOAT)nIndexY) * s_fCellHeight + fOffsetY;

			Cell.m_pPrimitive = m_pDotsScene->CreateSphere(Vec3(fPositionX,fPositionY,1), 0.015f);
			Cell.m_pPrimitive->SetDiffuse(0.6f);
			Cell.m_pPrimitive->SetTexture("Assets/NonFinalCommon/wood.png");
			Cell.m_pPrimitive->SetTextureScale(0.1f,0.1f);
		}
	}
}

bool CGameDots::CheckCellForCompletion(int nX, int nY)
{
	//if any of the edges on the square are unplayed, bail
	if(m_GridCells[nX][nY].m_eLineHoriz   == kUnplayed ||
		m_GridCells[nX][nY].m_eLineVert    == kUnplayed ||
		m_GridCells[nX+1][nY].m_eLineVert  == kUnplayed ||
		m_GridCells[nX][nY+1].m_eLineHoriz == kUnplayed)
	{
		return false;
	}

	//create the completion square
	m_GridCells[nX][nY].m_pBoxCompletedPrimitive = m_pDotsScene->CreateBox(Vec3(((MRTFLOAT)nX + 1.0f)*s_fCellWidth,((MRTFLOAT)nY + 1.0f)*s_fCellHeight,1.0f),Vec3(s_fCellWidth,s_fCellHeight,0.06f));
	m_GridCells[nX][nY].m_pBoxCompletedPrimitive->SetTexture("Assets/NonFinalCommon/wood.png");
	m_GridCells[nX][nY].m_pBoxCompletedPrimitive->SetTextureOffset(((MRTFLOAT)nX + 1.0f)*s_fCellWidth,((MRTFLOAT)nY + 1.0f)*s_fCellHeight);

	if(m_bPlayer1sTurn)
		m_GridCells[nX][nY].m_pBoxCompletedPrimitive->SetColor(Vec3(0.0f,0.0f,1.0f));
	else
		m_GridCells[nX][nY].m_pBoxCompletedPrimitive->SetColor(Vec3(1.0f,0.0f,0.0f));

	//remove the 4 walls
	if(m_GridCells[nX][nY].m_pPrimitiveHoriz)
	{
		m_pDotsScene->RemovePrimitive(m_GridCells[nX][nY].m_pPrimitiveHoriz);
		m_GridCells[nX][nY].m_pPrimitiveHoriz = 0;
	}

	if(m_GridCells[nX][nY].m_pPrimitiveVert)
	{
		m_pDotsScene->RemovePrimitive(m_GridCells[nX][nY].m_pPrimitiveVert);
		m_GridCells[nX][nY].m_pPrimitiveVert = 0;
	}

	if(m_GridCells[nX+1][nY].m_pPrimitiveVert)
	{
		m_pDotsScene->RemovePrimitive(m_GridCells[nX+1][nY].m_pPrimitiveVert);
		m_GridCells[nX+1][nY].m_pPrimitiveVert = 0;
	}

	if(m_GridCells[nX][nY+1].m_pPrimitiveHoriz)
	{
		m_pDotsScene->RemovePrimitive(m_GridCells[nX][nY+1].m_pPrimitiveHoriz);
		m_GridCells[nX][nY+1].m_pPrimitiveHoriz = 0;
	}

	m_pGameUI->OnPlayerScore(m_bPlayer1sTurn);

	return true;
}

bool CGameDots::CheckBoardForBoxCompletions()
{
	bool bRet = false;

	int nX = m_nSelectedLineIndexX - 1;
	int nY = m_nSelectedLineIndexY - 1;

	//if a horizontal line was placed
	if(m_bHorizontalLineSelected)
	{
		if(nY < s_nGridCellsY - 1)
		{
			bRet |= CheckCellForCompletion(nX,nY);
		}

		if(nY > 0)
		{
			bRet |= CheckCellForCompletion(nX,nY - 1);
		}
	}
	//else a vertical line was placed
	else
	{
		if(nX < s_nGridCellsX - 1)
		{
			bRet |= CheckCellForCompletion(nX,nY);
		}

		if(nX > 0)
		{
			bRet |= CheckCellForCompletion(nX - 1,nY);
		}
	}

	return bRet;
}

void CGameDots::OnTurnPlayed()
{
	m_bPlayer1sTurn = !m_bPlayer1sTurn;

	m_pGameUI->SetPlayer1sTurn(m_bPlayer1sTurn);
}
