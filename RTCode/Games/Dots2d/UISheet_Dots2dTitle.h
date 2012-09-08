/*======================================================

UISheet_Dots2dTitle.h

The functionality for the title screen in the dots 2d game

======================================================*/

#ifndef UISHEET_DOTS2DTITLE_H
#define UISHEET_DOTS2DTITLE_H

#include "../../CommonUI/UISheet.h"
#include "../../Callback.h"
#include "../../PlatformBase.h"
#include "../../WebService.h"

//forward declarations
class CTexture;
class CUIStack;
class CUIControl_TextBox;

class CUISheet_Dots2dTitle : public CUISheet
{
public:
	CUISheet_Dots2dTitle();
	virtual ~CUISheet_Dots2dTitle();

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

	virtual void Update(MRTFLOAT fFrameTime);

	virtual bool OnKeyTyped(unsigned char nKey);
private:

	DEFINE_MEMBERCALLBACK_ONEARG(CUISheet_Dots2dTitle,OnLoginRequestComplete,WebService::EWebServiceResponse)

	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dTitle,OnLoginCancel)
	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dTitle,OnClickNewAccount)
	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dTitle,OnClickLogin)
	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dTitle,OnClickForgotLogin)

	CTexture *m_pTitleImage;

	CUIControl_TextBox *m_pUserName;
	CUIControl_TextBox *m_pPassword;

	MRTFLOAT m_fCurrentTime;
};

#endif //UISHEET_SNAKEGAMEUI_H