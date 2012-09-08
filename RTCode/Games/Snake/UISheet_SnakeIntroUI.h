/*======================================================

UISheet_SnakeIntroUI.h

The functionality for the dialog you see when you start the game

======================================================*/

#ifndef UISHEET_SNAKEINTROUI_H
#define UISHEET_SNAKEINTROUI_H

#include "../../CommonUI/UISheet.h"

//forward declarations
class CTexture;

class CUISheet_SnakeIntro : public CUISheet
{
public:
	CUISheet_SnakeIntro();
	virtual ~CUISheet_SnakeIntro();

	virtual bool OnKeyDown(unsigned char nKey);
	virtual bool OnKeyUp(unsigned char nKey);
	virtual bool OnLeftClick();

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

protected:
	CTexture *m_pTexture;
};

#endif //UISHEET_SNAKEINTROUI_H