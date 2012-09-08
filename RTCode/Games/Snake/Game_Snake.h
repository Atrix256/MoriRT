/*======================================================

Game_Snake.h

The logic for the snake game

======================================================*/

#ifndef GAME_SNAKE_H
#define GAME_SNAKE_H

#include "../../GameBase.h"
#include "../../SceneManager.h"
#include "../../Scene.h"
#include "../../List.h"

//forward declarations
class CPrimitiveBox;
class CPrimitiveSphere;
class CUISheet;
class CUISheet_SnakeGameUI;
class CTexture;

class CGameSnake : public CGameBase
{
public:
	CGameSnake();
	virtual ~CGameSnake();

	virtual void Populate(int nScreenWidth, int nScreenHeight);
	virtual void OnView(bool bGainingFocus);
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void OnKeyDown(unsigned char nKey);
	virtual void OnKeyUp(unsigned char nKey);

	void OnCloseUI_Lost();
	static void StaticOnCloseUI_Lost(void *pGameSnake);

	void OnCloseUI_Intro();
	static void StaticOnCloseUI_Intro(void *pGameSnake);

	void OnSet3dMode(void);
	void ApplyQualitySettings(void);

private:
	enum EDirection
	{
		kNone,
		kUp,
		kRight,
		kDown,
		kLeft
	};

	enum ECellContents
	{
		kEmpty,
		kSnake,
		kApple,
		kOffBoard
	};

	enum EGameState
	{
		kPlaying,
		kLost
	};

	void ColorizeApple(void);
	void ColorizeSnakeSphere(CPrimitiveBase *pSnakeBall);
	void ColorizeAllSnakeSpheres();
	void SetSnakeSphereHeight(CPrimitiveBase *pSnakeBall);

	static Vec3 BlackWhiteGridTextureFunction(const SCollisionInfo &CollisionInfo, const CPrimitiveBase *pPrimitive, const Vec3 &vDiffuseColor);

	Vec3 GameGridToWorld(int nCellX, int nCellY);
	void WorldToGameGrid(Vec3 vPos, int &nCellX, int &nCellY);

	void ResetGame();

	void ConditionallyPlaceApple();
	void EatApple();

	void LoseGame();

	ECellContents GetCellContents(int nCellX, int nCellY);
	void SetCellContents(int nCellX, int nCellY, ECellContents eCellContents);

	void FixedUpdate();

	static const Vec3 s_CameraPos;
	static const Vec3 s_TargetPos;
	static const Vec3 s_CameraUp;

	static const int s_nSnakeSizeStart = 3;
	static const int s_nSnakeSizeAppleGrowth = 3;
	static const int s_nScreenGridCellCount = 15;
	static const MRTFLOAT s_fCameraFov;
	static const MRTFLOAT s_fCameraRenderRectSize;
	static const MRTFLOAT s_fWorldGridSize;

	static const MRTFLOAT s_fUpdateTime;
	static const MRTFLOAT s_fGameGridCellSize;
	static const int s_nNumGameGridCellsX;
	static const int s_nNumGameGridCellsY;

	CCamera *m_pCamera;
	CScene *m_pBreakoutScene;

	bool m_bLeftKeyPressed;
	bool m_bRightKeyPressed;
	bool m_bUpKeyPressed;
	bool m_bDownKeyPressed;

	bool m_bWKeyPressed;
	bool m_bAKeyPressed;
	bool m_bSKeyPressed;
	bool m_bDKeyPressed;

	MRTFLOAT m_fGameTime;
	MRTFLOAT m_fTimeBucket;

	RTDynamicList<CPrimitiveSphere *> m_SnakeBody;
	CPrimitiveBase *m_pApplePrimitive;

	EDirection m_eDirection;
	EDirection m_eQueuedDirection;
	int m_nQueuedBodyPeices;
	int m_nCurrentBodyHeadIndex;

	EGameState m_eGameState;

	RTDynamicList<ECellContents> m_eGameGrid;

	CUISheet *m_pLostDialog;
	CUISheet *m_pIntroDialog;

	CUISheet_SnakeGameUI *m_pGameUI;

	CTexture *m_pGridDetailTexture;
};

#endif //GAME_SNAKE_H