/*======================================================

UISheet_MainMenu.h

The functionality for the dots game main menu

======================================================*/

#ifndef UISHEET_MAINMENU_H
#define UISHEET_MAINMENU_H

#include "../../CommonUI/UISheet.h"

class CUISheet_MainMenu : public CUISheet
{
public:
	CUISheet_MainMenu();
	virtual ~CUISheet_MainMenu();

	void OnView(bool bGainingFocus) {m_bHasFocus = bGainingFocus;}

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

protected:
	
	bool m_bHasFocus;
};

#endif //UISHEET_MAINMENU_H