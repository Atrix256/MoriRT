/*======================================================

UISheet_SnakeGameUI.h

The functionality for the game UI in the snake game

======================================================*/

#ifndef UISHEET_SNAKEGAMEUI_H
#define UISHEET_SNAKEGAMEUI_H

#include "../../CommonUI/UISheet.h"

//forward declarations
class CTexture;
class CUIControl_ImageButton;
class CGameSnake;

class CUISheet_SnakeGameUI : public CUISheet
{
public:
	CUISheet_SnakeGameUI();
	virtual ~CUISheet_SnakeGameUI();

	void SetGamePointer(CGameSnake *pSnakeGame) {m_pSnakeGame = pSnakeGame;}

	void OnView(bool bGainingFocus) {m_bHasFocus = bGainingFocus;}
	void SetScore(int nScore);

	int GetQualitySetting() const {return m_nQualitySetting;}

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

protected:
	void OnClickQualityLow();
	void OnClickQualityMedium();
	void OnClickQualityHigh();

	void OnClickScreenSmall();
	void OnClickScreenMedium();
	void OnClickScreenLarge();

	void OnClick3dMode();

	static void StaticOnClickQualityLow(void *pArg);
	static void StaticOnClickQualityMedium(void *pArg);
	static void StaticOnClickQualityHigh(void *pArg);

	static void StaticOnClickScreenSmall(void *pArg);
	static void StaticOnClickScreenMedium(void *pArg);
	static void StaticOnClickScreenLarge(void *pArg);

	static void StaticOnClick3dMode(void *pArg);

	void PlaceIcons();

	bool m_bHasFocus;
	int m_nScore;
	int m_nHighScore;

	CUIControl_ImageButton *m_pScreenSmall;
	CUIControl_ImageButton *m_pScreenMedium;
	CUIControl_ImageButton *m_pScreenLarge;

	int m_nQualitySetting;

	CUIControl_ImageButton *m_pQualityLow;
	CUIControl_ImageButton *m_pQualityMedium;
	CUIControl_ImageButton *m_pQualityHigh;

	CUIControl_ImageButton *m_p3dMode;

	CGameSnake *m_pSnakeGame;
};

#endif //UISHEET_SNAKEGAMEUI_H