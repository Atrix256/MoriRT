/*======================================================

UISheet_Dots2dGB.h

The functionality for the scrolling background image of dots2d
======================================================*/

#ifndef UISHEET_DOTS2DBG_H
#define UISHEET_DOTS2DBG_H

#include "../../CommonUI/UISheet.h"
#include "../../Callback.h"
#include "../../PlatformBase.h"
#include "../../WebService.h"

//forward declarations
class CTexture;
class CUIStack;
class CUIControl_TextBox;

class CUISheet_Dots2dBG : public CUISheet
{
public:
	CUISheet_Dots2dBG();
	virtual ~CUISheet_Dots2dBG();

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

	virtual void Update(MRTFLOAT fFrameTime);

private:

	CTexture *m_pBackgroundImage;

	MRTFLOAT m_fCurrentTime;
};

#endif //UISHEET_DOTS2DBG_H