/*======================================================

Game_Mandelbrot.h

The logic for the mandelbrot viewer

======================================================*/

#ifndef GAME_MANDELBROT_H
#define GAME_MANDELBROT_H

#include "../../GameBase.h"

//forward declarations
class CUISheet_Mandelbrot;

class CGameMandelbrot : public CGameBase
{
public:
	CGameMandelbrot();

	virtual void Populate(int nScreenWidth, int nScreenHeight);
	virtual void OnView(bool bGainingFocus);
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void OnLeftClick();

private:

	CUISheet_Mandelbrot *m_pUI;
};

#endif //GAME_MANDELBROT_H