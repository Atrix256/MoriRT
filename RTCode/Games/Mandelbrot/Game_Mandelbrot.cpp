/*======================================================

Game_Mandelbrot.cpp

The logic for the mandelbrot viewer

======================================================*/

#include "Game_Mandelbrot.h"
#include "UISheet_Mandelbrot.h"
#include "../../RayTracer.h"
#include "../../CommonUI/UIStack.h"

CGameMandelbrot::CGameMandelbrot()
:m_pUI(NULL)
{

}

void CGameMandelbrot::Populate(int nScreenWidth, int nScreenHeight)
{
	m_pUI = CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_Mandelbrot>();
}

void CGameMandelbrot::OnView(bool bGainingFocus)
{

}

void CGameMandelbrot::Update(MRTFLOAT fFrameTime)
{

}

void CGameMandelbrot::OnLeftClick()
{

}
