/*======================================================

Game_Snake.cpp

The logic for the snake game

======================================================*/

#include "Game_Snake.h"
#include "../../SceneManager.h"
#include "../../PrimitiveBase.h"
#include "../../PrimitiveBox.h"
#include "../../PrimitiveSphere.h"
#include "../../PrimitiveTriangle.h"
#include "../../PrimitivePlanes.h"
#include "../../Camera.h"
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../CameraManager.h"
#include "../../TextureManager.h"
#include "../../Texture.h"
#include "UISheet_SnakeGameUI.h"
#include "UISheet_SnakeIntroUI.h"
#include "UISheet_Lost.h"
#include <math.h>
#include <time.h>

const MRTFLOAT CGameSnake::s_fCameraFov = 45.0f;
const MRTFLOAT CGameSnake::s_fCameraRenderRectSize = 0.5f;
const MRTFLOAT CGameSnake::s_fWorldGridSize = 1.0f / (MRTFLOAT)s_nScreenGridCellCount;

const Vec3 CGameSnake::s_CameraPos( 0.5f,-2.5f,-1.0f);
const Vec3 CGameSnake::s_TargetPos( 0.5f, 0.5f, 1.0f);
const Vec3 CGameSnake::s_CameraUp ( 0.0f, 0.0f,-1.0f);

#define SIZE_FACTOR 1.0f

const MRTFLOAT CGameSnake::s_fUpdateTime = 0.125f; // * SIZE_FACTOR;
const MRTFLOAT CGameSnake::s_fGameGridCellSize = 0.05f * SIZE_FACTOR;

const int CGameSnake::s_nNumGameGridCellsX = (int)ceilf(1.0f / s_fGameGridCellSize);
const int CGameSnake::s_nNumGameGridCellsY = (int)ceilf(1.0f / s_fGameGridCellSize);

CTexture *g_pGridTexture = 0;
int g_nQualitySettings = 1;

CGameSnake::CGameSnake()
{
	m_fGameTime = 0.0f;

	m_pCamera = 0;

	m_pBreakoutScene = NULL;

	m_bLeftKeyPressed = false;
	m_bRightKeyPressed = false;
	m_bUpKeyPressed = false;
	m_bDownKeyPressed = false;

	m_bWKeyPressed = false;
	m_bAKeyPressed = false;
	m_bSKeyPressed = false;
	m_bDKeyPressed = false;

	m_eDirection = kNone;
	m_eQueuedDirection = kNone;

	m_fTimeBucket = 0.0f;

	m_nQueuedBodyPeices = s_nSnakeSizeStart;
	m_nCurrentBodyHeadIndex = 0;

	m_pApplePrimitive = 0;

	m_eGameState = kPlaying;

	m_pLostDialog = 0;
	m_pIntroDialog = 0;
	m_pGameUI = 0;

	m_pGridDetailTexture = 0;
}

CGameSnake::~CGameSnake()
{
	TextureManager.FreeTexture(m_pGridDetailTexture);
}

void CGameSnake::Populate(int nScreenWidth, int nScreenHeight)
{
	srand((unsigned)time(0));
	Vec3 vCameraPos = s_CameraPos;
	Vec3 vDir = s_TargetPos - s_CameraPos;
	vDir.Normalize();
	vCameraPos += vDir * 1.7f;

	//create our game UI and tell it some info
	m_pGameUI = CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_SnakeGameUI>();
	m_pGameUI->SetScore(1);
	m_pGameUI->SetGamePointer(this);

	m_pIntroDialog = CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_SnakeIntro>();

	//create the camera
	m_pCamera = CameraManager.AddCamera(vCameraPos,           //eye
										s_TargetPos,          //target
										s_CameraUp,           //up
										nScreenWidth,         //pixel width
										nScreenHeight,        //pixel height
										s_fCameraFov,         //FOV
										s_fCameraRenderRectSize,
										s_nScreenGridCellCount,
										Vec3(0.175f,0.0f,0.0f));

	//m_pCamera->SetModeOrthographic();

	//add our scene
	m_pBreakoutScene = SceneManager.AddScene(s_fWorldGridSize, Vec3(-0.1f,-0.1f,0.9f), Vec3(1.1f,1.1f,1.2f));
	m_pBreakoutScene->SetAmbientLight(Vec3(0.0f,0.0f,0.0f));
	m_pCamera->SetScene(m_pBreakoutScene);

	//create the board primitive
	CPrimitiveBase *pPrimitive = m_pBreakoutScene->CreateBox(Vec3(0.5,0.5,1.05f),Vec3(1.0f,1.0f,0.1f));
	pPrimitive->SetTextureFunction(BlackWhiteGridTextureFunction);
	pPrimitive->SetTextureScale(5.0f,5.0f);
	pPrimitive->SetReflection(0.8f);

	//lights
	m_pBreakoutScene->AddDirectionalLight(Vec3(0.5f,0.1f,1.0f),Vec3(1.0f,1.0f,1.0f));

	ResetGame();

	ApplyQualitySettings();

	//load our grid detail texture
	m_pGridDetailTexture = TextureManager.LoadTexture("Assets/Snake/boarddetail.png");
	g_pGridTexture = m_pGridDetailTexture;
}

void CGameSnake::ResetGame()
{
	//set the score to 1
	m_pGameUI->SetScore(1);

	//make the game grid
	m_eGameGrid.clear();
	int nNumGameCells = s_nNumGameGridCellsX*s_nNumGameGridCellsY;
	while(nNumGameCells)
	{
		m_eGameGrid.push_back(kEmpty);
		nNumGameCells--;
	}

	//clear the snake body
	while(m_SnakeBody.size() > 0)
	{
		m_pBreakoutScene->RemovePrimitive(m_SnakeBody.last());
		m_SnakeBody.pop_back();
	}

	//clear the apple if it exists
	if(m_pApplePrimitive)
	{
		m_pBreakoutScene->RemovePrimitive(m_pApplePrimitive);
		m_pApplePrimitive = 0;
	}

	//starting snake body
	Vec3 vCenterPos = GameGridToWorld(s_nNumGameGridCellsX / 2, s_nNumGameGridCellsY / 2);
	CPrimitiveSphere *pPrimitive = m_pBreakoutScene->CreateSphere(vCenterPos,s_fGameGridCellSize / 2.0f);
	ColorizeSnakeSphere(pPrimitive);
	m_SnakeBody.push_back(pPrimitive);
	m_nCurrentBodyHeadIndex = 0;
	m_nQueuedBodyPeices = s_nSnakeSizeStart;
	SetSnakeSphereHeight(pPrimitive);

	//go back to playing state
	m_eGameState = kPlaying;

	//set the snake to not moving
	m_eDirection = kNone;
	m_eQueuedDirection = kNone;
}

void CGameSnake::OnView(bool bGainingFocus)
{
	if(bGainingFocus)
	{
		CRayTracer::GetSingleton().SetCamera(m_pCamera);
	}

	m_pGameUI->OnView(bGainingFocus);
}

void CGameSnake::Update(MRTFLOAT fFrameTime)
{
	m_fGameTime += fFrameTime;

	//place an apple if we need to
	ConditionallyPlaceApple();

	if(m_eGameState == kPlaying)
	{
		m_fTimeBucket += fFrameTime;
		while(m_eGameState == kPlaying && m_fTimeBucket > s_fUpdateTime)
		{
			m_fTimeBucket -= s_fUpdateTime;
			FixedUpdate();
		}
	}
}

void CGameSnake::EatApple()
{
	//remove the apple primitive from the scene
	m_pBreakoutScene->RemovePrimitive(m_pApplePrimitive);
	m_pApplePrimitive = 0;

	//mark our snake for growth
	m_nQueuedBodyPeices+=s_nSnakeSizeAppleGrowth;
}

void CGameSnake::ConditionallyPlaceApple()
{
	//bail out if we already have an apple
	if(m_pApplePrimitive)
		return;

	//calculate which cell to put an apple in
	int nCellX = -1;
	int nCellY = -1;
	do
	{
		nCellX = rand() % s_nNumGameGridCellsX;
		nCellY = rand() % s_nNumGameGridCellsY;
	}
	while(GetCellContents(nCellX,nCellY) != kEmpty);

	//create the apple primitive
	//m_pApplePrimitive = m_pBreakoutScene->CreateSphere(GameGridToWorld(nCellX,nCellY), s_fGameGridCellSize / 2.0f);
	m_pApplePrimitive = m_pBreakoutScene->CreateBox(GameGridToWorld(nCellX,nCellY), Vec3(s_fGameGridCellSize*0.75f,s_fGameGridCellSize*0.75f,s_fGameGridCellSize*0.75f));

	ColorizeApple();

	//place the apple on the grid
	SetCellContents(nCellX,nCellY,kApple);
}

void CGameSnake::FixedUpdate()
{
	//apply our new direction if we have one queued up
	m_eDirection = m_eQueuedDirection;

	//bail out if we aren't moving
	if(m_eDirection == kNone)
		return;

	//get the position of our oldest peice
	int nLastIndex = m_nCurrentBodyHeadIndex - 1;
	if(nLastIndex < 0)
		nLastIndex = m_SnakeBody.size() - 1;

	Vec3 vNewPos = m_SnakeBody[nLastIndex]->GetPosition();

	//move the position based on the direction
	switch(m_eDirection)
	{
		case kUp:
		{
			vNewPos.m_fY += s_fGameGridCellSize;
			break;
		}
		case kDown:
		{
			vNewPos.m_fY -= s_fGameGridCellSize;
			break;
		}
		case kRight:
		{
			vNewPos.m_fX += s_fGameGridCellSize;
			break;
		}
		case kLeft:
		{
			vNewPos.m_fX -= s_fGameGridCellSize;
			break;
		}
		case kNone:
		{
			break;
		}
	}

	//see what we ran into, if anything
	int nNextCellX;
	int nNextCellY;
	WorldToGameGrid(vNewPos,nNextCellX,nNextCellY);
	switch(GetCellContents(nNextCellX,nNextCellY))
	{
		case kEmpty:
		{
			//nothing to do, everything is ok
			break;
		}
		case kSnake:
		{
			LoseGame();
			return;
		}
		case kApple:
		{
			EatApple();
			break;
		}
		case kOffBoard:
		{
			LoseGame();
			return;
		}
	}

	//mark that the snake is in this cell now
	SetCellContents(nNextCellX,nNextCellY,kSnake);

	//if we have new body peices to create, create one
	if(m_nQueuedBodyPeices > 0)
	{
		//decriment how many body peices we need to create
		m_nQueuedBodyPeices--;

		//create the new body peice
		CPrimitiveSphere *pPrimitive = m_pBreakoutScene->CreateSphere(vNewPos,s_fGameGridCellSize / 2.0f);
		ColorizeSnakeSphere(pPrimitive);
		m_SnakeBody.insert(m_nCurrentBodyHeadIndex,pPrimitive,true);

		SetSnakeSphereHeight(pPrimitive);

		//set the new score
		m_pGameUI->SetScore(m_SnakeBody.size());
	}
	//else move the tail to the front
	else
	{
		//unmark the grid cell of where the tail was
		int nOldCellX;
		int nOldCellY;
		WorldToGameGrid(m_SnakeBody[m_nCurrentBodyHeadIndex]->GetPosition(),nOldCellX,nOldCellY);
		SetCellContents(nOldCellX,nOldCellY,kEmpty);

		//move the tail to the new location
		m_SnakeBody[m_nCurrentBodyHeadIndex]->SetPosition(vNewPos);

		SetSnakeSphereHeight(m_SnakeBody[m_nCurrentBodyHeadIndex]);
	}

	//advance to the next index
	m_nCurrentBodyHeadIndex++;
	if(m_nCurrentBodyHeadIndex >= m_SnakeBody.size())
		m_nCurrentBodyHeadIndex= 0;
}

void CGameSnake::OnKeyDown(unsigned char nKey)
{
	//38 is up
	if(nKey == 38)
	{
		m_bUpKeyPressed = true;

		if(m_eDirection != kDown)
			m_eQueuedDirection = kUp;
	}

	//40 is down
	if(nKey == 40)
	{
		m_bDownKeyPressed = true;

		if(m_eDirection != kUp)
			m_eQueuedDirection = kDown;
	}

	//37 is left
	if(nKey == 37)
	{
		m_bLeftKeyPressed = true;

		if(m_eDirection != kRight)
			m_eQueuedDirection = kLeft;
	}

	//39 is right
	if(nKey == 39)
	{
		m_bRightKeyPressed = true;

		if(m_eDirection != kLeft)
			m_eQueuedDirection = kRight;
	}

	if(nKey == 'W')
	{
		m_bWKeyPressed = true;

		if(m_eDirection != kDown)
			m_eQueuedDirection = kUp;
	}

	if(nKey == 'S')
	{
		m_bSKeyPressed = true;

		if(m_eDirection != kUp)
			m_eQueuedDirection = kDown;
	}

	if(nKey == 'A')
	{
		m_bAKeyPressed = true;

		if(m_eDirection != kRight)
			m_eQueuedDirection = kLeft;
	}

	if(nKey == 'D')
	{
		m_bDKeyPressed = true;

		if(m_eDirection != kLeft)
			m_eQueuedDirection = kRight;
	}
}

void CGameSnake::OnKeyUp(unsigned char nKey)
{
	//38 is up
	if(nKey == 38)
	{
		m_bUpKeyPressed = false;
	}

	//40 is down
	if(nKey == 40)
	{
		m_bDownKeyPressed = false;
	}

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

	if(nKey == 'W')
	{
		m_bWKeyPressed = false;
	}

	if(nKey == 'A')
	{
		m_bAKeyPressed = false;
	}

	if(nKey == 'S')
	{
		m_bSKeyPressed = false;
	}

	if(nKey == 'D')
	{
		m_bDKeyPressed = false;
	}
}

Vec3 CGameSnake::BlackWhiteGridTextureFunction(const SCollisionInfo &CollisionInfo, const CPrimitiveBase *pPrimitive, const Vec3 &vDiffuseColor)
{
	if(CollisionInfo.m_vCollisionPoint.m_fX < 0.015f ||
	   CollisionInfo.m_vCollisionPoint.m_fX > 0.985f ||
	   CollisionInfo.m_vCollisionPoint.m_fY < 0.015f ||
	   CollisionInfo.m_vCollisionPoint.m_fY > 0.985f)
	{
		return Vec3(1.0f,1.0f,0.2f) * vDiffuseColor;
	}

	MRTFLOAT fU = CollisionInfo.m_fTextureU * 1.0f;
	MRTFLOAT fV = CollisionInfo.m_fTextureV * 1.0f;

	MRTFLOAT fModU = fmodf(fU,1.0f);
	MRTFLOAT fModV = fmodf(fV,1.0f);

	if(fModU < 0.0f)
		fModU += 1.0f;

	if(fModV < 0.0f)
		fModV += 1.0f;

	bool bA = fModU > 0.5f;
	bool bB = fModV > 0.5f;

	switch(g_nQualitySettings)
	{
		case 1:
		{
			MRTFLOAT fAlpha = 0;
			Vec3 vDetailColor = g_pGridTexture->GetTexel(CollisionInfo.m_fTextureU * 0.75f,CollisionInfo.m_fTextureV * 0.75f,&fAlpha);
			vDetailColor *= (fAlpha * 0.75f);

			if(bA != bB)
			{
				return Vec3(0.4f,0.4f,0.6f) * vDiffuseColor + vDetailColor;
			}
			else
			{
				return Vec3(0.1f,0.1f,0.15f) * vDiffuseColor + vDetailColor;
			}
			break;
		}
		case 2:
		{
			MRTFLOAT fAlpha = 0;
			Vec3 vDetailColor = g_pGridTexture->GetTexelBilinear(CollisionInfo.m_fTextureU * 0.75f,CollisionInfo.m_fTextureV * 0.75f,&fAlpha);
			vDetailColor *= fAlpha;

			if(bA != bB)
			{
				return Vec3(0.4f,0.4f,0.6f) * vDiffuseColor + vDetailColor;
			}
			else
			{
				return Vec3(0.1f,0.1f,0.15f) * vDiffuseColor + vDetailColor;
			}
			break;
		}
		default:
		{
			if(bA != bB)
			{
				return Vec3(0.4f,0.4f,0.6f) * vDiffuseColor;
			}
			else
			{
				return Vec3(0.1f,0.1f,0.15f) * vDiffuseColor;
			}
			break;
		}
	}

}

Vec3 CGameSnake::GameGridToWorld(int nCellX, int nCellY)
{
	return Vec3(s_fGameGridCellSize * (MRTFLOAT)nCellX + s_fGameGridCellSize * 0.5f,s_fGameGridCellSize * (MRTFLOAT)nCellY + s_fGameGridCellSize * 0.5f,1.0f - s_fGameGridCellSize / 2.0f);
}

void CGameSnake::WorldToGameGrid(Vec3 vPos, int &nCellX, int &nCellY)
{
	nCellX = (int)((vPos.m_fX) / s_fGameGridCellSize);
	nCellY = (int)((vPos.m_fY) / s_fGameGridCellSize);
}

CGameSnake::ECellContents CGameSnake::GetCellContents(int nCellX, int nCellY)
{
	if(nCellX < 0 || nCellY < 0 ||
	   nCellX >= s_nNumGameGridCellsX ||
	   nCellY >= s_nNumGameGridCellsY)
	{
		return kOffBoard;
	}

	return m_eGameGrid[nCellY * s_nNumGameGridCellsX + nCellX];
}

void CGameSnake::SetCellContents(int nCellX, int nCellY, ECellContents eCellContents)
{
	if(nCellX < 0 || nCellY < 0 ||
	   nCellX >= s_nNumGameGridCellsX ||
	   nCellY >= s_nNumGameGridCellsY)
	{
		return;
	}

	m_eGameGrid[nCellY * s_nNumGameGridCellsX + nCellX] = eCellContents;
}

void CGameSnake::LoseGame()
{
	m_eGameState = kLost;

	m_pLostDialog = CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_Lost>();
	m_pLostDialog->SetOnCloseCallback(StaticOnCloseUI_Lost,this);
}

void CGameSnake::OnCloseUI_Lost()
{
	m_pLostDialog = 0;
	ResetGame();
}

void CGameSnake::StaticOnCloseUI_Lost(void *pGameSnake)
{
	static_cast<CGameSnake *>(pGameSnake)->OnCloseUI_Lost();
}

void CGameSnake::OnCloseUI_Intro()
{
	m_pIntroDialog = 0;
}

void CGameSnake::StaticOnCloseUI_Intro(void *pGameSnake)
{
	static_cast<CGameSnake *>(pGameSnake)->OnCloseUI_Intro();
}

void CGameSnake::OnSet3dMode()
{
	ColorizeApple();
	ColorizeAllSnakeSpheres();
}

void CGameSnake::SetSnakeSphereHeight(CPrimitiveBase *pSnakeBall)
{
	//didnt look that great
	/*
	static MRTFLOAT fSnakePhase = 0.0f;

	Vec3 vCenterPos = GameGridToWorld(s_nNumGameGridCellsX / 2, s_nNumGameGridCellsY / 2);

	Vec3 vPos = pSnakeBall->GetPosition();
	vPos.m_fZ = 1.0f - s_fGameGridCellSize / 2.0f;
	vPos.m_fZ -= (sin(fSnakePhase) + 1.0f) * 0.01f;
	pSnakeBall->SetPosition(vPos);

	fSnakePhase += 0.7f;
	*/
}

void CGameSnake::ColorizeSnakeSphere(CPrimitiveBase *pSnakeBall)
{
	if(CRayTracer::GetSingleton().GetRedBlue3dModeOn())
	{
		pSnakeBall->SetColor(Vec3(0.75f,1.0f,0.75f));
		//pSnakeBall->SetColor(Vec3(1.0f,1.0f,1.0f));
		//pSnakeBall->SetLightAbsorption(Vec3(1.0f,0.0f,1.0f),10000.0f);
		pSnakeBall->SetDiffuse(0.5f);
		pSnakeBall->SetRefraction(0.5f);
		pSnakeBall->SetRefractionIndex(1.1f);
	}
	else
	{
		pSnakeBall->SetColor(Vec3(0.0f,1.0f,0.0f));
		//pSnakeBall->SetColor(Vec3(1.0f,1.0f,1.0f));
		//pSnakeBall->SetLightAbsorption(Vec3(1.0f,0.0f,1.0f),10000.0f);
		pSnakeBall->SetDiffuse(0.5f);
		pSnakeBall->SetRefraction(0.5f);
		pSnakeBall->SetRefractionIndex(1.1f);
	}
}

void CGameSnake::ColorizeAllSnakeSpheres()
{
	for(int nIndex = 0; nIndex < m_SnakeBody.size(); ++nIndex)
	{
		ColorizeSnakeSphere(m_SnakeBody[nIndex]);
	}
}

void CGameSnake::ColorizeApple(void)
{
	if(m_pApplePrimitive)
	{
		m_pApplePrimitive->SetDiffuse(0.5f);
		m_pApplePrimitive->SetRefraction(0.5f);
		m_pApplePrimitive->SetRefractionIndex(1.1f);

		if(CRayTracer::GetSingleton().GetRedBlue3dModeOn())
		{
			m_pApplePrimitive->SetColor(Vec3(1.0f,1.0f,1.0f));

			m_pApplePrimitive->SetEmissiveColor(Vec3(0.2f,0.2f,0.2f));
		}
		else
		{
			m_pApplePrimitive->SetColor(Vec3(1.0f,0.0f,0.0f));

			m_pApplePrimitive->SetEmissiveColor(Vec3(0.0f,0.0f,0.0f));
		}
	}
}

void CGameSnake::ApplyQualitySettings(void)
{
	g_nQualitySettings = m_pGameUI->GetQualitySetting();
	switch(m_pGameUI->GetQualitySetting())
	{
		//low
		case 0:
		{
			CRayTracer::GetSingleton().SetReflectionsEnabled(false);
			CRayTracer::GetSingleton().SetRefractionsEnabled(false);
			CRayTracer::GetSingleton().SetShadingEnabled(true);
			CRayTracer::GetSingleton().SetTexturesEnabled(true);
			CRayTracer::GetSingleton().SetShadowsSetting(CRayTracer::kShadowsOff);
			break;
		}
		//medium (default
		case 1:
		{
			CRayTracer::GetSingleton().SetReflectionsEnabled(true);
			CRayTracer::GetSingleton().SetRefractionsEnabled(true);
			CRayTracer::GetSingleton().SetShadingEnabled(true);
			CRayTracer::GetSingleton().SetTexturesEnabled(true);
			CRayTracer::GetSingleton().SetShadowsSetting(CRayTracer::kShadowsOff);
			break;
		}
		//high
		case 2:
		{
			CRayTracer::GetSingleton().SetReflectionsEnabled(true);
			CRayTracer::GetSingleton().SetRefractionsEnabled(true);
			CRayTracer::GetSingleton().SetShadingEnabled(true);
			CRayTracer::GetSingleton().SetTexturesEnabled(true);
			CRayTracer::GetSingleton().SetShadowsSetting(CRayTracer::kShadowsSimple);
			break;
		}
	}
}