/*======================================================

GameBase.h

The base class interface that all games use

======================================================*/

#ifndef GAMEBASE_H
#define GAMEBASE_H

#include "Defines.h"

class CGameBase
{
public:
	CGameBase() {}
	virtual ~CGameBase() {}

	virtual void Populate(int nScreenWidth, int nScreenHeight) {}
	virtual void OnView(bool bGainingFocus) {}
	virtual void Update(MRTFLOAT fFrameTime) {}
	virtual void OnLeftClick() {}
	virtual void OnKeyDown(unsigned char nKey) {}
	virtual void OnKeyUp(unsigned char nKey) {}
private:
};

#endif //GAMEBASE_H