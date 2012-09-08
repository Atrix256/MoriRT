/*======================================================

Game_Breakout.cpp

The logic for the breakout game

======================================================*/

#include "Game_Breakout.h"
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

const MRTFLOAT CGameBreakout::s_fCameraFov = 45.0f;
const MRTFLOAT CGameBreakout::s_fCameraRenderRectSize = 0.5f;
const MRTFLOAT CGameBreakout::s_fWorldGridSize = 1.0f / (MRTFLOAT)s_nScreenGridCellCount;

const Vec3 CGameBreakout::s_CameraPos( 0.5f, 0.5f,-0.2f);
const Vec3 CGameBreakout::s_TargetPos( 0.5f, 0.5f, 1.0f);
const Vec3 CGameBreakout::s_CameraUp ( 0.0f, 1.0f, 0.0f);

const Vec3 CGameBreakout::s_vBallStartPos(0.5f,0.04f,1.0f);
const Vec3 CGameBreakout::s_vPaddleScale(0.2f,0.05f,0.05f);
const Vec3 CGameBreakout::s_vPaddleStartPos(0.5f,0.04f,1.0f);

const MRTFLOAT CGameBreakout::s_fMaxPaddleSpeed = 0.7f;     //max speed, units per second
const MRTFLOAT CGameBreakout::s_fPaddleAcceleration = 2.0f; //units per second, per second
const MRTFLOAT CGameBreakout::s_fPaddleDrag = 3.0f;
const MRTFLOAT CGameBreakout::s_fPaddleBounceFactor = 0.33f;

const MRTFLOAT CGameBreakout::s_fBallRadius = 0.02f;

CGameBreakout::CGameBreakout()
{
	m_fGameTime = 0.0f;

	m_pPaddleRectangle = 0;
	m_pBall = 0;

	m_pCamera = 0;

	m_pBreakoutScene = NULL;

	m_bLeftKeyPressed = false;
	m_bRightKeyPressed = false;

	m_vBallVelocity = Vec3(-0.6f,0.8f,0.0f);
	m_fPaddleSpeed = 0.0f;
}

void CGameBreakout::Populate(int nScreenWidth, int nScreenHeight)
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

	m_pCamera->SetModeOrthographic();

	//add our scene
	m_pBreakoutScene = SceneManager.AddScene(s_fWorldGridSize, Vec3(0,0,0), Vec3(1,1,1));
	m_pBreakoutScene->SetAmbientLight(Vec3(0.0f,0.0f,0.0f));
	m_pCamera->SetScene(m_pBreakoutScene);

	MakeBoard();

	//lights
	CPrimitiveBase *pPrimitive = m_pBreakoutScene->CreateSphere(Vec3(0.7f,0.7f,-1.0f), 0.01f);
	pPrimitive->SetColor(Vec3(1,1,1));
	pPrimitive->SetIsLight(true);
}

void CGameBreakout::OnView(bool bGainingFocus)
{
	if(bGainingFocus)
	{
		CRayTracer::GetSingleton().SetShadowsSetting(CRayTracer::kShadowsOff);

		CRayTracer::GetSingleton().SetCamera(m_pCamera);
	}
	else
	{
		CRayTracer::GetSingleton().SetShadowsSetting(CRayTracer::kShadowsOn);
	}
}

void CGameBreakout::Update(MRTFLOAT fFrameTime)
{
	m_fGameTime += fFrameTime;

	//get the mouse position
	int nMouseX, nMouseY;
	CRayTracer::GetSingleton().GetPlatform().GetMousePosition(nMouseX,nMouseY);

	if(m_bLeftKeyPressed != m_bRightKeyPressed)
	{
		if(m_bRightKeyPressed)
		{
			m_fPaddleSpeed += s_fPaddleAcceleration * fFrameTime;
			if(m_fPaddleSpeed > s_fMaxPaddleSpeed)
				m_fPaddleSpeed = s_fMaxPaddleSpeed;				
		}
		else
		{
			m_fPaddleSpeed -= s_fPaddleAcceleration * fFrameTime;
			if(m_fPaddleSpeed < -s_fMaxPaddleSpeed)
				m_fPaddleSpeed = -s_fMaxPaddleSpeed;
		}
	}
	else
	{
		m_fPaddleSpeed -= m_fPaddleSpeed * fFrameTime * s_fPaddleDrag;
	}

	Vec3 vPos = m_pPaddleRectangle->GetPosition();
	Vec3 vOldPos = vPos;
	vPos.m_fX += m_fPaddleSpeed * fFrameTime;

	if(vPos.m_fX < s_vPaddleScale.m_fX / 2.0f)
	{
		vPos.m_fX = s_vPaddleScale.m_fX / 2.0f;

		if(m_fPaddleSpeed < 0.0f)
		{
			m_fPaddleSpeed *= -s_fPaddleBounceFactor;
		}
	}

	if(vPos.m_fX > 1.0f - s_vPaddleScale.m_fX / 2.0f)
	{
		vPos.m_fX = 1.0f - s_vPaddleScale.m_fX / 2.0f;

		if(m_fPaddleSpeed > 0.0f)
		{
			m_fPaddleSpeed *= -s_fPaddleBounceFactor;
		}
	}

	//only move if it's far enough
	if((vOldPos - vPos).Length() > 0.0001f)
		m_pPaddleRectangle->SetPosition(vPos);

	//move the ball
	vPos = m_pBall->GetPosition();
	vPos += m_vBallVelocity * fFrameTime;

	//handle the ball bouncing off of the walls
	if(vPos.m_fX < s_fBallRadius)
	{
		vPos.m_fX = s_fBallRadius;
		Vec3 vN(1.0f,0.0f,0.0f);
		m_vBallVelocity = m_vBallVelocity - vN * m_vBallVelocity.Dot(vN) * 2;
	}

	if(vPos.m_fX > 1.0f - s_fBallRadius)
	{
		vPos.m_fX = 1.0f - s_fBallRadius;
		Vec3 vN(-1.0f,0.0f,0.0f);
		m_vBallVelocity = m_vBallVelocity - vN * m_vBallVelocity.Dot(vN) * 2;
	}

	if(vPos.m_fY < s_fBallRadius)
	{
		vPos.m_fY = s_fBallRadius;
		Vec3 vN(0.0f,1.0f,0.0f);
		m_vBallVelocity = m_vBallVelocity - vN * m_vBallVelocity.Dot(vN) * 2;
	}

	if(vPos.m_fY > 1.0f - s_fBallRadius)
	{
		vPos.m_fY = 1.0f - s_fBallRadius;
		Vec3 vN(0.0f,-1.0f,0.0f);
		m_vBallVelocity = m_vBallVelocity - vN * m_vBallVelocity.Dot(vN) * 2;
	}

	m_pBall->SetPosition(vPos);
}

void CGameBreakout::OnKeyDown(unsigned char nKey)
{
	//37 is left
	if(nKey == 37)
	{
		m_bLeftKeyPressed = true;
	}

	//39 is right
	if(nKey == 39)
	{
		m_bRightKeyPressed = true;
	}
}

void CGameBreakout::OnKeyUp(unsigned char nKey)
{
	//37 is left
	if(nKey == 37)
	{
		m_bLeftKeyPressed = false;
	}

	//39 is right
	if(nKey == 39)
	{
		m_bRightKeyPressed = false;
	}
}

void CGameBreakout::MakeBoard()
{
	MRTFLOAT fBrickSizeX = 0.1f;
	MRTFLOAT fBrickMarginX = 0.01f;
	MRTFLOAT fBrickSpacingX = (fBrickSizeX+fBrickMarginX);
	int nNumBricksX = (int)(1.0f / fBrickSpacingX);

	MRTFLOAT fCenterXOffset = (1.0f - (((MRTFLOAT)nNumBricksX) * fBrickSpacingX)) / 2.0f + fBrickSpacingX / 2.0f;

	MRTFLOAT fBrickSizeY = 0.025f;
	MRTFLOAT fBrickMarginY = 0.02f;
	MRTFLOAT fBrickSpacingY = (fBrickSizeY+fBrickMarginY);
	int nNumBricksY = (int)(1.0f / fBrickSpacingY) / 2;

	MRTFLOAT fCenterYOffset = (1.0f - (((MRTFLOAT)nNumBricksY) * fBrickSpacingY));

	MRTFLOAT fBrickSizeZ = 0.05f;

	for(int nIndexY = 0; nIndexY < nNumBricksY; ++nIndexY)
	{
		MRTFLOAT fPositionY = ((MRTFLOAT)nIndexY * fBrickSpacingY) + fCenterYOffset;

		for(int nIndexX = 0; nIndexX < nNumBricksX; ++nIndexX)
		{
			MRTFLOAT fPositionX = ((MRTFLOAT)nIndexX * fBrickSpacingX) + fCenterXOffset;

			CPrimitiveBase *pPrimitive = m_pBreakoutScene->CreateBox(Vec3(fPositionX,fPositionY,1.0f),Vec3(fBrickSizeX,fBrickSizeY,fBrickSizeZ));
				
			MRTFLOAT fR = ((MRTFLOAT)(rand() % 3)) / 2.0f;
			MRTFLOAT fG = ((MRTFLOAT)(rand() % 3)) / 2.0f;
			MRTFLOAT fB = ((MRTFLOAT)(rand() % 3)) / 2.0f;
			pPrimitive->SetColor(Vec3(fR,fG,fB));
			pPrimitive->SetDiffuse(0.6f);
			pPrimitive->SetEmissiveColor(Vec3(0.1f,0.1f,0.1f));
		}
	}

	m_pPaddleRectangle = m_pBreakoutScene->CreateBox(s_vPaddleStartPos,s_vPaddleScale);
	//pPaddleRectangle->SetColor(Vec3(0.1f,0.1f,0.1f));


	m_pBall = m_pBreakoutScene->CreateSphere(s_vBallStartPos,s_fBallRadius);
	m_pBall->SetColor(Vec3(0.7f,0.35f,0.35f));
	m_pBall->SetDiffuse(0.5f);
	//pBall->SetRefraction(1.0f);
	//pBall->SetRefractionIndex(2.0f);
	//pBall->SetEmissiveColor(Vec3(0.2f,0.2f,0.2f));

	CPrimitiveBase *pPrimitive = m_pBreakoutScene->CreateBox(Vec3(0.5,0.5,1.0f),Vec3(2.0f,2.0f,0.001f));
	pPrimitive->SetTexture("Assets/Breakout/space1.jpg");
	pPrimitive->SetTextureScale(1.0f/1.3f,1.0f);
	pPrimitive->SetTextureOffset(0.5f,0.0f);
}
