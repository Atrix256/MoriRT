/*======================================================

Game.cpp

This is where all the game specific stuff goes like scene intialization
and per frame game logic

======================================================*/

#include "Game.h"
#include "SceneManager.h"
#include "Scene.h"
#include "PrimitiveBase.h"
#include "PrimitiveBox.h"
#include "PrimitiveSphere.h"
#include "PrimitiveTriangle.h"
#include "CameraManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Games/Dots/Game_Dots.h"
#include "Games/Breakout/Game_Breakout.h"
#include "Games/Tests/TestGames.h"
#include "Games/Snake/Game_Snake.h"
#include "Games/Mandelbrot/Game_Mandelbrot.h"
#include "Games/Dots2d/Game_Dots2d.h"

namespace
{
	#if FINALGAME == GAME_SNAKE
		CGameSnake g_SnakeGame;
	#elif FINALGAME == GAME_MANDELBROT
		CGameMandelbrot g_MandelbrotGame;
	#elif FINALGAME == GAME_DOTS2D
		CGameDots2d g_Dots2dGame;
	#else
		CGameDots g_DotsGame;
		CGameBreakout g_BreakoutGame;
		CTestScene1 g_TestScene1;
		CTestScene2 g_TestScene2;
		CTestScene3 g_TestScene3;
	#endif

	CGameBase *pCurrentGame = 0;
}

CGameBase *GetCurrentGame()
{
	return pCurrentGame;
}

void GameInit(int nScreenWidth, int nScreenHeight)
{
	#if FINALGAME == GAME_SNAKE
		g_SnakeGame.Populate(nScreenWidth,nScreenHeight);
		pCurrentGame = &g_SnakeGame;
	#elif FINALGAME == GAME_MANDELBROT
		g_MandelbrotGame.Populate(nScreenWidth,nScreenHeight);
		pCurrentGame = &g_MandelbrotGame;
	#elif FINALGAME == GAME_DOTS2D
		g_Dots2dGame.Populate(nScreenWidth,nScreenHeight);
		pCurrentGame = &g_Dots2dGame;
	#else
		g_TestScene1.Populate(nScreenWidth,nScreenHeight);
		g_TestScene2.Populate(nScreenWidth,nScreenHeight);
		g_TestScene3.Populate(nScreenWidth,nScreenHeight);
		g_DotsGame.Populate(nScreenWidth,nScreenHeight);
		g_BreakoutGame.Populate(nScreenWidth,nScreenHeight);
		pCurrentGame = &g_TestScene1;
	#endif

	//start the game
	pCurrentGame->OnView(true);
}

void GameUpdate(MRTFLOAT fFrameTime)
{
	pCurrentGame->Update(fFrameTime);
}

void GameOnKeyDown(unsigned char nKey)
{
	pCurrentGame->OnKeyDown(nKey);
}

void GameOnKeyUp(unsigned char nKey)
{
	#if FINALGAME != GAME_NONE
	#else
		if(nKey == '1')
		{
			pCurrentGame->OnView(false);
			pCurrentGame = &g_TestScene1;
			pCurrentGame->OnView(true);
		}
		if(nKey == '2')
		{
			pCurrentGame->OnView(false);
			pCurrentGame = &g_TestScene2;
			pCurrentGame->OnView(true);
		}
		if(nKey == '3')
		{
			pCurrentGame->OnView(false);
			pCurrentGame = &g_DotsGame;
			pCurrentGame->OnView(true);
		}
		else if(nKey == '4')
		{
			pCurrentGame->OnView(false);
			pCurrentGame = &g_BreakoutGame;
			pCurrentGame->OnView(true);
		}
		else if(nKey == '5')
		{
			pCurrentGame->OnView(false);
			pCurrentGame = &g_TestScene3;
			pCurrentGame->OnView(true);
		}
	#endif

	pCurrentGame->OnKeyUp(nKey);
}

void GameOnLeftClick()
{
	pCurrentGame->OnLeftClick();
}