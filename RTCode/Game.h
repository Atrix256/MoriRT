/*======================================================

Game.h

This is where all the game specific stuff goes like scene intialization
and per frame game logic

======================================================*/

#ifndef GAME_H
#define GAME_H

#include "Defines.h"

class CGameBase;

void GameInit(int nScreenWidth, int nScreenHeight);
void GameUpdate(MRTFLOAT fFrameTime);
void GameOnKeyDown(unsigned char nKey);
void GameOnKeyUp(unsigned char nKey);
void GameOnLeftClick();
CGameBase *GetCurrentGame();

#endif //GAME_H