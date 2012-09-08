/*======================================================

TestGames.h

The logic for the test / demo "games"

======================================================*/

#ifndef TESTGAMES_H
#define TESTGAMES_H

#include "../../GameBase.h"
#include "../../Vec3.h"

//forward declarations
class CScene;
class CCamera;
class CPrimitiveBase;
struct SCollisionInfo;

class CTestScene1 : public CGameBase
{
public:
	CTestScene1();

	virtual void Populate(int nScreenWidth, int nScreenHeight);
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void OnView(bool bGainingFocus);
	virtual void OnKeyUp(unsigned char nKey);

private:
	static const Vec3 s_CameraPos;
	static const Vec3 s_TargetPos;
	static const Vec3 s_CameraUp;
	static const MRTFLOAT s_fCameraFOV;
	static const MRTFLOAT s_fCameraRenderRectSize;
	static const MRTFLOAT s_fWorldGridSize;
	static const int s_nScreenGridCellCount;

	CScene *m_pScene;
	CCamera *m_pCamera;

	CPrimitiveBase *m_pMovingSphere;
	CPrimitiveBase *m_pAnimatingSphere;

	bool m_bMoveObjects;
	bool m_bAnimateTextures;

	MRTFLOAT m_fGameTime;
};

class CTestScene2 : public CGameBase
{
public:
	CTestScene2();

	virtual void Populate(int nScreenWidth, int nScreenHeight);
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void OnView(bool bGainingFocus);
	virtual void OnKeyUp(unsigned char nKey);

private:
	static Vec3 BlackWhiteGridTextureFunction(const SCollisionInfo &CollisionInfo, const CPrimitiveBase *pPrimitive, const Vec3 &vDiffuseColor);
	static Vec3 BlackWhiteGridTextureFunction3d(const SCollisionInfo &CollisionInfo, const CPrimitiveBase *pPrimitive, const Vec3 &vDiffuseColor);

	static const Vec3 s_CameraPos;
	static const Vec3 s_TargetPos;
	static const Vec3 s_CameraUp;
	static const MRTFLOAT s_fCameraFOV;
	static const MRTFLOAT s_fCameraRenderRectSize;
	static const MRTFLOAT s_fWorldGridSize;
	static const int s_nScreenGridCellCount;

	CScene *m_pScene;
	CCamera *m_pCamera;

	CPrimitiveBase *m_pAnimatingPlane1;
	CPrimitiveBase *m_pAnimatingPlane2;
	CPrimitiveBase *m_pMovingBox;

	bool m_bMoveObjects;
	bool m_bAnimateTextures;

	MRTFLOAT m_fGameTime;
};

class CTestScene3 : public CGameBase
{
public:
	CTestScene3();

	virtual void Populate(int nScreenWidth, int nScreenHeight);
	virtual void OnView(bool bGainingFocus);
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void OnKeyUp(unsigned char nKey);
	virtual void OnKeyDown(unsigned char nKey);

private:
	CScene *m_pScene;
	CCamera *m_pCamera;

	static const Vec3 s_CameraPos;
	static const Vec3 s_TargetPos;
	static const Vec3 s_CameraUp;
	static const MRTFLOAT s_fCameraFOV;
	static const MRTFLOAT s_fCameraRenderRectSize;
	static const MRTFLOAT s_fWorldGridSize;
	static const int s_nScreenGridCellCount;

	bool m_bLeftKeyPressed;
	bool m_bRightKeyPressed;
	MRTFLOAT m_fCameraPhase;
};

#endif //TESTGAMES_H
