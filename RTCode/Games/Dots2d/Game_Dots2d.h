/*======================================================

Game_Dots2d.h

The logic for the dots 2d game

======================================================*/

#ifndef GAME_DOTS2D_H
#define GAME_DOTS2D_H

#include "../../GameBase.h"
#include "../../SceneManager.h"
#include "../../Scene.h"

//forward declarations
class CUISheet;
class CUISheet_Dots2dTitle;

class CGameDots2d : public CGameBase
{
public:
	CGameDots2d();

	virtual void Populate(int nScreenWidth, int nScreenHeight);
	virtual void OnView(bool bGainingFocus) {}
	virtual void Update(MRTFLOAT fFrameTime) {}
};

#endif //GAME_DOTS2D_H