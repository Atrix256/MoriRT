/*======================================================

Game_Breakout.h

The logic for the breakout game

======================================================*/

#ifndef GAME_BREAKOUT_H
#define GAME_BREAKOUT_H

#include "../../GameBase.h"
#include "../../SceneManager.h"
#include "../../Scene.h"

//forward declarations
class CPrimitiveBox;
class CPrimitiveSphere;

class CGameBreakout : public CGameBase
{
public:
	CGameBreakout();

	virtual void Populate(int nScreenWidth, int nScreenHeight);
	virtual void OnView(bool bGainingFocus);
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void OnKeyDown(unsigned char nKey);
	virtual void OnKeyUp(unsigned char nKey);

private:
	void MakeBoard(void);

	static const Vec3 s_CameraPos;
	static const Vec3 s_TargetPos;
	static const Vec3 s_CameraUp;

	static const Vec3 s_vBallStartPos;
	static const Vec3 s_vPaddleScale;
	static const Vec3 s_vPaddleStartPos;

	static const int s_nScreenGridCellCount = 15;
	static const MRTFLOAT s_fCameraFov;
	static const MRTFLOAT s_fCameraRenderRectSize;
	static const MRTFLOAT s_fWorldGridSize;

	static const MRTFLOAT s_fMaxPaddleSpeed;
	static const MRTFLOAT s_fPaddleAcceleration;
	static const MRTFLOAT s_fPaddleDrag;
	static const MRTFLOAT s_fPaddleBounceFactor;

	static const MRTFLOAT s_fBallRadius;

	CCamera *m_pCamera;
	CScene *m_pBreakoutScene;

	bool m_bLeftKeyPressed;
	bool m_bRightKeyPressed;

	MRTFLOAT m_fGameTime;

	CPrimitiveBox *m_pPaddleRectangle;
	CPrimitiveSphere *m_pBall;

	Vec3 m_vBallVelocity;
	MRTFLOAT m_fPaddleSpeed;
};

#endif //GAME_BREAKOUT_H