/*======================================================

TestGames.cpp

The logic for the test / demo "games"

======================================================*/

#include "TestGames.h"
#include "../../SceneManager.h"
#include "../../Scene.h"
#include "../../PrimitiveBase.h"
#include "../../PrimitiveTriangle.h"
#include "../../PrimitiveSphere.h"
#include "../../PrimitiveBox.h"
#include "../../PrimitiveComposite.h"
#include "../../RayTracer.h"
#include "../../CameraManager.h"
#include "../../Camera.h"

const Vec3 CTestScene1::s_CameraPos(1.0f,8.0f,-70.0f);
const Vec3 CTestScene1::s_TargetPos(0.0f,0.0f, 0.0f);
const Vec3 CTestScene1::s_CameraUp(0.0f,1.0f,0.0f);
const MRTFLOAT CTestScene1::s_fCameraFOV = 45.0f;
const MRTFLOAT CTestScene1::s_fCameraRenderRectSize = 8.0f;
const MRTFLOAT CTestScene1::s_fWorldGridSize = 10.0f;
const int CTestScene1::s_nScreenGridCellCount = 16;

const Vec3 CTestScene2::s_CameraPos(1.0f,8.0f,-70.0f);
const Vec3 CTestScene2::s_TargetPos(0.0f,0.0f, 0.0f);
const Vec3 CTestScene2::s_CameraUp(0.0f,1.0f,0.0f);
const MRTFLOAT CTestScene2::s_fCameraFOV = 45.0f;
const MRTFLOAT CTestScene2::s_fCameraRenderRectSize = 8.0f;
const MRTFLOAT CTestScene2::s_fWorldGridSize = 10.0f;
const int CTestScene2::s_nScreenGridCellCount = 16;

const Vec3 CTestScene3::s_CameraPos(1.0f,8.0f,-70.0f);
const Vec3 CTestScene3::s_TargetPos(-1.5f,0.0f,20.0f);
const Vec3 CTestScene3::s_CameraUp(0.0f,1.0f,0.0f);
const MRTFLOAT CTestScene3::s_fCameraFOV = 45.0f;
const MRTFLOAT CTestScene3::s_fCameraRenderRectSize = 8.0f;
const MRTFLOAT CTestScene3::s_fWorldGridSize = 11.0f;
const int CTestScene3::s_nScreenGridCellCount = 16;

CTestScene1::CTestScene1()
{
	m_pScene = 0;
	m_pCamera = 0;
	m_fGameTime = 0.0f;
	m_pMovingSphere = 0;
	m_pAnimatingSphere = 0;

	m_bMoveObjects = true;
	m_bAnimateTextures = true;
}

void CTestScene1::Populate(int nScreenWidth, int nScreenHeight)
{
	m_pCamera = CameraManager.AddCamera(s_CameraPos,         //eye
									   s_TargetPos,          //target
									   s_CameraUp,           //up
									   nScreenWidth,         //pixel width
									   nScreenHeight,        //pixel height
									   s_fCameraFOV,         //FOV
									   s_fCameraRenderRectSize,
									   s_nScreenGridCellCount);

	m_pScene = SceneManager.AddScene(s_fWorldGridSize, Vec3(-50,-30,-50), Vec3(50,30,50));

	m_pCamera->SetScene(m_pScene);

	//g_pScene1->SetBackgroundColor(Vec3(0.1f,0.4f,0.9f));

	CPrimitiveBase *pPrimitive;

	//ground quad made up of 2 triangles
	Vec3 pointA(-411.0f,-6.0f, 411.0f);
	Vec3 pointB( 411.0f,-6.0f, 411.0f);
	Vec3 pointC( 411.0f,-6.0f,-411.0f);
	Vec3 pointD(-411.0f,-6.0f,-411.0f);
	pPrimitive = m_pScene->CreateTriangle(pointA,pointB,pointC);
	pPrimitive->SetReflection(0.0f);
	pPrimitive->SetDiffuse(1.0f);
	pPrimitive->SetTexture("Assets/NonFinalCommon/wood.png");
	pPrimitive->SetTextureScale(0.025f,0.025f);
	pPrimitive = m_pScene->CreateTriangle(pointA,pointC,pointD);
	pPrimitive->SetReflection(0.0f);
	pPrimitive->SetDiffuse(1.0f);
	pPrimitive->SetTexture("Assets/NonFinalCommon/wood.png");
	pPrimitive->SetTextureScale(0.025f,0.025f);

	pPrimitive = m_pScene->CreateSphere(Vec3(0.0f, 0.0f, 0.0f), 5.0f);
	pPrimitive->SetEmissiveColor(Vec3(0.1f,0.0f,0.4f));

	//spheres
	pPrimitive = m_pScene->CreateSphere(Vec3(-3.0f, 2.0f, 0.0f), 5.0f);
	pPrimitive->SetColor(Vec3(0.7f,0.35f,0.35f));
	pPrimitive->SetRefraction(1.0f);
	pPrimitive->SetRefractionIndex(1.5f);
	m_pMovingSphere = pPrimitive;

	pPrimitive = m_pScene->CreateSphere(Vec3(15.0f, 2.0f, 15.0f), 3.0f);
	pPrimitive->SetColor(Vec3(0.9f,0.3f,0.4f));
	pPrimitive->SetReflection(1.0f);
	pPrimitive->SetTexture("Assets/NonFinalCommon/wood.png");
	pPrimitive->SetTextureScale(0.2f,0.2f);

	//pPrimitive = g_pScene1->AddBox(Vec3(-8.0f,4.0f,7.0f),Vec3(11.0f,14.0f,7.0f));
	pPrimitive = m_pScene->CreateSphere(Vec3(-8.0f,4.0f,7.0f),7.0f);
	pPrimitive->SetDiffuseColor(Vec3(1.0f,1.0f,1.0f));
	pPrimitive->SetSpecularColor(Vec3(0.2f,0.5f,1.0f));
	pPrimitive->SetDiffuse(0.3f);
	pPrimitive->SetReflection(0.3f);
	pPrimitive->SetTexture("Assets/NonFinalCommon/wood.png");
	pPrimitive->SetTextureScale(1.5f,1.5f);
	m_pAnimatingSphere = pPrimitive;		

	/*
	pPrimitive = g_pScene1->AddSphere(Vec3(-5.0f,0.0f,25.0f),14.0f);
	pPrimitive->SetDiffuseColor(Vec3(1.0f,1.0f,1.0f));
	pPrimitive->SetSpecularColor(Vec3(0.0f,1.0f,0.0f));
	pPrimitive->SetDiffuse(0.3f);
	pPrimitive->SetReflection(0.0f);
	pPrimitive->SetTextureFunction(BlackWhiteGridTextureFunction);
	g_AnimatingSphere1 = pPrimitive;*/

	//lights
	pPrimitive = m_pScene->CreateSphere(Vec3(0.0f,25.0f,5.0f), 0.1f);
	pPrimitive->SetColor(Vec3(0.6f,0.6f,0.6f));
	pPrimitive->SetIsLight(true);

	pPrimitive = m_pScene->CreateSphere(Vec3(2.0f,5.0f,1.0f), 0.1f);
	pPrimitive->SetColor(Vec3(0.7f,0.7f,0.9f));
	pPrimitive->SetIsLight(true);
}

void CTestScene1::Update(MRTFLOAT fFrameTime)
{
	m_fGameTime += fFrameTime;

	if(m_bMoveObjects)
	{
		MRTFLOAT fXPos = sinf(m_fGameTime / 2.0f) * 20.0f;
		MRTFLOAT fZPos = cosf(m_fGameTime / 2.0f) * 10.0f + 5.0f;

		Vec3 vPos = m_pMovingSphere->GetPosition();
		vPos.m_fX = fXPos;
		vPos.m_fZ = fZPos;
		m_pMovingSphere->SetPosition(vPos);
	}

	if(m_bAnimateTextures)
	{
		m_pAnimatingSphere->SetTextureOffset(m_fGameTime/1.0f,m_fGameTime/2.0f);
	}

}

void CTestScene1::OnView(bool bGainingFocus)
{
	if(bGainingFocus)
		CRayTracer::GetSingleton().SetCamera(m_pCamera);
}

void CTestScene1::OnKeyUp(unsigned char nKey)
{
	if(nKey=='O')
		m_bMoveObjects = !m_bMoveObjects;

	if(nKey=='T')
		m_bAnimateTextures = !m_bAnimateTextures;
}



CTestScene2::CTestScene2()
{
	m_pScene = 0;
	m_pCamera = 0;
	m_fGameTime = 0.0f;
	m_pMovingBox = 0;
	m_pAnimatingPlane1 = 0;
	m_pAnimatingPlane2 = 0;

	m_bMoveObjects = true;
	m_bAnimateTextures = true;
}

void CTestScene2::Populate(int nScreenWidth, int nScreenHeight)
{
	m_pCamera = CameraManager.AddCamera(s_CameraPos,         //eye
									   s_TargetPos,          //target
									   s_CameraUp,           //up
									   nScreenWidth,         //pixel width
									   nScreenHeight,        //pixel height
									   s_fCameraFOV,         //FOV
									   s_fCameraRenderRectSize,
									   s_nScreenGridCellCount);

	m_pScene = SceneManager.AddScene(s_fWorldGridSize, Vec3(-50,-30,-50), Vec3(50,30,50));

	m_pCamera->SetScene(m_pScene);

	//g_pScene2->SetAmbientLight(Vec3(0.1f,0.05f,0.05f));
	//g_pScene2->SetBackgroundColor(Vec3(1.0f,0.05f,0.05f));

	//g_pScene2->SetBackgroundColor(Vec3(0.1f,0.4f,0.9f));
	m_pScene->SetBackgroundColor(Vec3(0.1f,0.1f,0.1f));

	CPrimitiveBase *pPrimitive;

	//ground quad made up of 2 triangles
	Vec3 pointA(-500.0f,-4.0f, 500.0f);
	Vec3 pointB( 500.0f,-4.0f, 500.0f);
	Vec3 pointC( 500.0f,-4.0f,-500.0f);
	Vec3 pointD(-500.0f,-4.0f,-500.0f);
	pPrimitive = m_pScene->CreateTriangle(pointA,pointB,pointC);
	pPrimitive->SetReflection(0.0f);
	pPrimitive->SetDiffuse(1.0f);
	pPrimitive->SetColor(Vec3(0.4f,0.3f,0.9f));
	pPrimitive->SetTextureFunction(BlackWhiteGridTextureFunction);
	pPrimitive->SetTextureScale(0.075f,0.075f);
	pPrimitive->SetReflection(0.8f);
	m_pAnimatingPlane1 = pPrimitive;
	pPrimitive = m_pScene->CreateTriangle(pointA,pointC,pointD);
	pPrimitive->SetReflection(0.0f);
	pPrimitive->SetDiffuse(1.0f);
	pPrimitive->SetColor(Vec3(0.4f,0.9f,0.3f));		
	pPrimitive->SetTextureFunction(BlackWhiteGridTextureFunction);
	pPrimitive->SetTextureScale(0.075f,0.075f);
	pPrimitive->SetReflection(0.8f);
	m_pAnimatingPlane2 = pPrimitive;
    
	//blue sphere
	pPrimitive = m_pScene->CreateSphere(Vec3(-4.0f, -2.0f, 3.0f), 2.0f);
	pPrimitive->SetReflection(0.5f);
	pPrimitive->SetDiffuse(0.2f);
	pPrimitive->SetColor(Vec3(0.3f,0.3f,0.9f));

	//yellow sphere
	pPrimitive = m_pScene->CreateSphere(Vec3(6.0f, -2.0f, 3.0f), 2.0f);
	pPrimitive->SetReflection(0.5f);
	pPrimitive->SetDiffuse(0.2f);
	pPrimitive->SetColor(Vec3(0.9f,0.9f,0.3f));  

	//green sphere
	pPrimitive = m_pScene->CreateSphere(Vec3(0.0f, -2.0f, 0.0f), 2.0f);
	pPrimitive->SetReflection(0.5f);
	pPrimitive->SetDiffuse(0.2f);
	pPrimitive->SetColor(Vec3(0.3f,0.9f,0.3f));  

	//clear sphere (formerly red sphere)
	//pPrimitive = g_pScene2->AddSphere(Vec3(0.0f, 2.0f, 0.0f), 5.0f);
	pPrimitive = m_pScene->CreateBox(Vec3(0.0f, 2.0f, -5.0f), Vec3(15.0f,10.0f,5.0f));
	/*pPrimitive->SetReflection(1.0f);
	pPrimitive->SetDiffuse(0.4f);
	pPrimitive->SetDiffuseColor(Vec3(0.1f,0.8,0.1f)); 
	pPrimitive->SetSpecularColor(Vec3(1.0f,0.0f,1.0f));*/
	pPrimitive->SetColor(Vec3(0.0f,0.0f,0.0f));
	pPrimitive->SetRefraction(1.0f);
	pPrimitive->SetRefractionIndex(1.9f);
	pPrimitive->SetLightAbsorption(Vec3(0.0f,1.0f,1.0f),0.2f);
	m_pMovingBox = pPrimitive;

	/*
	pPrimitive->SetDiffuseColor(Vec3(1.0f,1.0,1.0f)); 
	pPrimitive->SetSpecularColor(Vec3(0.0f,0.0f,0.0f));
	pPrimitive->SetTexture("Assets/NonFinalCommon/wood.png");
	pPrimitive->SetTextureScale(0.25f,0.25f);*/


	/*
	pPrimitive = g_pScene2->AddSphere(Vec3(0.0f, 2.0f, 0.0f), 5.0f);
	pPrimitive->SetRefraction(1.0f);
	pPrimitive->SetRefractionIndex(1.2f);
	pPrimitive->SetLightAbsorption(Vec3(1.0f,0.0f,1.0f),0.3f);
	g_MovingSphere3 = pPrimitive;

	pPrimitive = g_pScene2->AddSphere(Vec3(0.0f, 2.0f, 0.0f), 5.0f);
	pPrimitive->SetRefraction(1.0f);
	pPrimitive->SetRefractionIndex(1.3f);
	pPrimitive->SetLightAbsorption(Vec3(1.0f,1.0f,0.0f),0.4f);
	g_MovingSphere4 = pPrimitive;*/
  
	//magenta sphere
	pPrimitive = m_pScene->CreateSphere(Vec3(-12.0f, -2.0f, 10.0f), 2.0f);
	pPrimitive->SetReflection(0.5f);
	pPrimitive->SetDiffuse(0.2f);
	pPrimitive->SetColor(Vec3(0.9f,0.3f,0.9f));    

	//teal sphere
	pPrimitive = m_pScene->CreateSphere(Vec3(4.0f, 5.0f, 10.0f), 2.0f);
	pPrimitive->SetReflection(0.5f);
	pPrimitive->SetDiffuse(0.2f);
	pPrimitive->SetColor(Vec3(0.3f,0.9f,0.9f));      

	//reflective back quad
	pointA = Vec3(-500.0f, 500.0f, 41.0f);
	pointB = Vec3( 500.0f, 500.0f, 41.0f);
	pointC = Vec3( 500.0f,-500.0f, 41.0f);
	pointD = Vec3(-500.0f,-500.0f, 41.0f);
	pPrimitive = m_pScene->CreateTriangle(pointA,pointB,pointC);
	pPrimitive->SetReflection(1.0f);
	pPrimitive->SetDiffuse(0.0f);
	pPrimitive->SetDiffuseColor(Vec3(1.0f,1.0f,1.0f));
	pPrimitive->SetSpecularColor(Vec3(0.0f,0.0f,0.0f));  
	pPrimitive = m_pScene->CreateTriangle(pointA,pointC,pointD);
	pPrimitive->SetReflection(1.0f);
	pPrimitive->SetDiffuse(0.0f);
	pPrimitive->SetDiffuseColor(Vec3(1.0f,1.0f,1.0f));    
	pPrimitive->SetSpecularColor(Vec3(0.0f,0.0f,0.0f));

	//lights
	pPrimitive = m_pScene->CreateSphere(Vec3(0.0f,5.0f,5.0f), 0.1f);
	pPrimitive->SetColor(Vec3(0.6f,0.6f,0.6f));
	pPrimitive->SetIsLight(true);

	pPrimitive = m_pScene->CreateSphere(Vec3(2.0f,5.0f,1.0f), 0.1f);
	pPrimitive->SetColor(Vec3(0.7f,0.7f,0.9f));
	pPrimitive->SetIsLight(true);

	//AABB &aabb = g_pScene2->GetAABB();
}

void CTestScene2::Update(MRTFLOAT fFrameTime)
{
	m_fGameTime += fFrameTime;

	if(m_bMoveObjects)
	{
		MRTFLOAT fXPos = sinf(m_fGameTime / 2.0f) * 20.0f;
		MRTFLOAT fZPos = cosf(m_fGameTime / 2.0f) * 10.0f + 5.0f;

		Vec3 vPos = m_pMovingBox->GetPosition();
		vPos.m_fX = fXPos;
		vPos.m_fZ = fZPos;
		m_pMovingBox->SetPosition(vPos);
	}

	if(m_bAnimateTextures)
	{
		m_pAnimatingPlane1->SetTextureOffset(m_fGameTime/16.0f,m_fGameTime/16.0f);
		m_pAnimatingPlane2->SetTextureOffset(m_fGameTime/16.0f,m_fGameTime/16.0f);
	}

}

void CTestScene2::OnView(bool bGainingFocus)
{
	if(bGainingFocus)
		CRayTracer::GetSingleton().SetCamera(m_pCamera);
}

void CTestScene2::OnKeyUp(unsigned char nKey)
{
	if(nKey=='O')
		m_bMoveObjects = !m_bMoveObjects;

	if(nKey=='T')
		m_bAnimateTextures = !m_bAnimateTextures;
}

Vec3 CTestScene2::BlackWhiteGridTextureFunction3d(const SCollisionInfo &CollisionInfo, const CPrimitiveBase *pPrimitive, const Vec3 &vDiffuseColor)
{
	MRTFLOAT fModX = fmodf(CollisionInfo.m_vCollisionPoint.m_fX*0.5f,1.0f);
	MRTFLOAT fModY = fmodf(CollisionInfo.m_vCollisionPoint.m_fY*0.5f,1.0f);
	MRTFLOAT fModZ = fmodf(CollisionInfo.m_vCollisionPoint.m_fZ*0.5f,1.0f);

	if(fModX < 0.0f)
		fModX += 1.0f;

	if(fModY < 0.0f)
		fModY += 1.0f;

	if(fModZ < 0.0f)
		fModZ += 1.0f;

	bool bX = fModX > 0.5f;
	bool bY = fModY > 0.5f;
	bool bZ = fModZ > 0.5f;

	if(bX ^ bY ^ bZ)
	{
		return Vec3(1.0f,1.0f,1.0f);
	}
	else
	{
		return Vec3(0.0f,0.0f,0.0f);
	}
}

Vec3 CTestScene2::BlackWhiteGridTextureFunction(const SCollisionInfo &CollisionInfo, const CPrimitiveBase *pPrimitive, const Vec3 &vDiffuseColor)
{
	MRTFLOAT fModU = fmodf(CollisionInfo.m_fTextureU*0.5f,1.0f);
	MRTFLOAT fModV = fmodf(CollisionInfo.m_fTextureV*0.5f,1.0f);

	if(fModU < 0.0f)
		fModU += 1.0f;

	if(fModV < 0.0f)
		fModV += 1.0f;

	bool bA = fModU > 0.5f;
	bool bB = fModV > 0.5f;

	if(bA != bB)
	{
		return Vec3(1.0f,1.0f,1.0f) * vDiffuseColor;
	}
	else
	{
		return Vec3(0.0f,0.0f,0.0f) * vDiffuseColor;
	}
}


CTestScene3::CTestScene3()
{
	m_pScene = 0;
	m_pCamera = 0;
	m_bLeftKeyPressed = false;
	m_bRightKeyPressed = false;
	m_fCameraPhase = 0.0f;
}

void CTestScene3::Populate(int nScreenWidth, int nScreenHeight)
{
	m_pCamera = CameraManager.AddCamera(s_CameraPos,         //eye
									   s_TargetPos,          //target
									   s_CameraUp,           //up
									   nScreenWidth,         //pixel width
									   nScreenHeight,        //pixel height
									   s_fCameraFOV,         //FOV
									   s_fCameraRenderRectSize,
									   s_nScreenGridCellCount);

	m_pScene = SceneManager.AddScene(s_fWorldGridSize, Vec3(-50,-30,-50), Vec3(50,30,50));
	m_pScene->SetBackgroundColor(Vec3(0.2f,0.1f,0.1f));
	m_pScene->SetAmbientLight(Vec3(0.1f,0.1f,0.1f));

	m_pCamera->SetScene(m_pScene);

	//CPrimitiveBase *pPrimitive = m_pScene->CreateSphere(Vec3(-1.5f, 0.0f, 20.0f), 10.0f);
	CPrimitiveBase *pPrimitive = m_pScene->CreateBox(Vec3(-1.5f, 0.0f, 20.0f), Vec3(20.0f,20.0f,20.0f));
	pPrimitive->SetTexture("Assets/NonFinalCommon/wood.png");
	pPrimitive->SetTextureScale(0.1f,0.1f);

	CPrimitiveBase *pPrimitive2 = m_pScene->CreateSphere(Vec3(-8.0f, 2.0f, 0.0f), 5.0f, false);
	CPrimitiveBase *pPrimitive1 = m_pScene->CreateSphere(Vec3(-5.0f, 2.0f, 0.0f), 5.0f, false);
	pPrimitive = m_pScene->CreateCompositePrimitive(pPrimitive1,pPrimitive2,CPrimitiveComposite::kSubtract);

	//pPrimitive = m_pScene->CreateBox(Vec3(0.0f, 2.0f, -5.0f), Vec3(15.0f,10.0f,5.0f));
	/*pPrimitive->SetReflection(1.0f);
	pPrimitive->SetDiffuse(0.4f);
	pPrimitive->SetDiffuseColor(Vec3(0.1f,0.8,0.1f)); 
	pPrimitive->SetSpecularColor(Vec3(1.0f,0.0f,1.0f));*/
	pPrimitive->SetColor(Vec3(0.0f,0.0f,0.0f));
	pPrimitive->SetRefraction(1.0f);
	pPrimitive->SetRefractionIndex(1.9f);
	pPrimitive->SetLightAbsorption(Vec3(0.0f,1.0f,1.0f),0.2f);

	//lights
	pPrimitive = m_pScene->CreateSphere(Vec3(0.0f,25.0f,-15.0f), 0.1f);
	pPrimitive->SetColor(Vec3(0.6f,0.6f,0.6f));
	pPrimitive->SetIsLight(true);

	m_pScene->AddDirectionalLight(Vec3(-1.0f,0.0f,-1.0f),Vec3(0.2f,0.2f,0.6f));

	Update(0.0f);
}

void CTestScene3::OnView(bool bGainingFocus)
{
	if(bGainingFocus)
		CRayTracer::GetSingleton().SetCamera(m_pCamera);
}

void CTestScene3::Update(MRTFLOAT fFrameTime)
{
	if(m_bLeftKeyPressed != m_bRightKeyPressed)
	{
		if(m_bRightKeyPressed)
		{
			m_fCameraPhase -= fFrameTime;
		}
		else
		{
			m_fCameraPhase += fFrameTime;
		}

		Vec3 vCameraPos = s_TargetPos + Vec3(sinf(m_fCameraPhase)*75.0f,8.0f,cosf(m_fCameraPhase)*75.0f);
		m_pCamera->LookAt(vCameraPos,s_TargetPos,s_CameraUp);
	}
}

void CTestScene3::OnKeyUp(unsigned char nKey)
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

void CTestScene3::OnKeyDown(unsigned char nKey)
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