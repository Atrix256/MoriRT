/*======================================================

Game_Dots.h

The logic for the dots game

======================================================*/

#ifndef GAME_DOTS_H
#define GAME_DOTS_H

#include "../../GameBase.h"
#include "../../SceneManager.h"
#include "../../Scene.h"

class CUISheet_GameUI;

enum ELinePlayed
{
	kUnplayed,
	kPlayer1,
	kPlayer2
};

struct SGridCell
{
	SGridCell()
	:m_pPrimitive(0)
	,m_pBoxCompletedPrimitive(0)
	,m_eLineHoriz(kUnplayed)
	,m_eLineVert(kUnplayed)
	,m_pPrimitiveHoriz(0)
	,m_pPrimitiveVert(0)
	{
	}

	CPrimitiveBase *m_pPrimitive;
	CPrimitiveBase *m_pBoxCompletedPrimitive;
	ELinePlayed m_eLineHoriz;
	ELinePlayed m_eLineVert;
	CPrimitiveBase *m_pPrimitiveHoriz;
	CPrimitiveBase *m_pPrimitiveVert;
};

class CGameDots : public CGameBase
{
public:
	CGameDots();

	virtual void Populate(int nScreenWidth, int nScreenHeight);
	virtual void OnView(bool bGainingFocus);
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void OnLeftClick();

private:

	bool CheckBoardForBoxCompletions();
	void OnTurnPlayed();
	void MakeGrid();
	bool CheckCellForCompletion(int nX, int nY);

	static const int s_nGridCellsX = 10;
	static const int s_nGridCellsY = 10;
	static const int s_nScreenGridCellCount = 15;

	static const MRTFLOAT s_fCellWidth;
	static const MRTFLOAT s_fCellHeight;

	static const MRTFLOAT s_fCameraFov;
	static const MRTFLOAT s_fCameraRenderRectSize;

	static const MRTFLOAT s_fWorldGridSize;

	static const Vec3 s_CameraPos;
	static const Vec3 s_TargetPos;
	static const Vec3 s_CameraUp;

	CCamera *m_pCamera;
	CScene *m_pDotsScene;
	
	SGridCell m_GridCells[s_nGridCellsX][s_nGridCellsY];

	CPrimitiveBox *m_pRectangleSelectionPrimitive;

	bool m_bPlayer1sTurn;
	bool m_bHorizontalLineSelected;
	int m_nSelectedLineIndexX;
	int m_nSelectedLineIndexY;

	MRTFLOAT m_fGameTime;

	CUISheet_GameUI *m_pGameUI;
};

#endif //GAME_DOTS_H