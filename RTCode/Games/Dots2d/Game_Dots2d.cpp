/*======================================================

Game_Dots.cpp

The logic for the dots game

======================================================*/

#include "Game_Dots2d.h"
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
#include "UISheet_Dots2dTitle.h"
#include "UISheet_Dots2dBG.h"
#include "../../WebService.h"

CGameDots2d::CGameDots2d()
{
}

void CGameDots2d::Populate(int nScreenWidth, int nScreenHeight)
{
	//tell the webservice what game we are
	WebService::GetSingleton().SetGameName("dots");

	//create our game UI and tell it some info
	CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_Dots2dBG>();
	CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_Dots2dTitle>();
}