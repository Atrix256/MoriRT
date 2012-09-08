/*======================================================

UISheet_GameUI.h

The functionality for the game UI in the dots game

======================================================*/

#ifndef UISHEET_GAMEUI_H
#define UISHEET_GAMEUI_H

#include "../../CommonUI/UISheet.h"

class CUISheet_GameUI : public CUISheet
{
public:
	CUISheet_GameUI();
	virtual ~CUISheet_GameUI();

	void SetPlayer1sTurn(bool bIsPlayer1sTurn) {m_bIsPlayer1sTurn = bIsPlayer1sTurn;}

	void SetTotalTiles(int nNumTiles)
	{
		m_nNumTiles = nNumTiles;
	}

	void OnPlayerScore(bool bPlayer1)
	{
		if(bPlayer1)
			m_nScore[0]++;
		else
			m_nScore[1]++;
	}

	void OnView(bool bGainingFocus) {m_bHasFocus = bGainingFocus;}

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

protected:

	int m_nScreenWidth;
	int m_nScreenHeight;

	bool m_bIsPlayer1sTurn;
	int m_nNumTiles;
	int m_nScore[2];

	bool m_bHasFocus;
};

#endif //UISHEET_GAMEUI_H