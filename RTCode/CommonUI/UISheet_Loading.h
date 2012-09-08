/*======================================================

UISheet_Loading.h

The functionality for a generic "loading" / "please wait" screen

======================================================*/

#ifndef UISHEET_LOADING_H
#define UISHEET_LOADING_H

#include "Dialog.h"
#include "../Callback.h"
#include "UIControl_TextButton.h"
#include <string>

//forward declarations
class CStaticText;

class CUISheet_Loading : public CDialog
{
public:
	CUISheet_Loading();
	virtual ~CUISheet_Loading() {}
	virtual void Update(MRTFLOAT fFrameTime);

	void SetMessage(const char *szMessage);

	void SetCancelCallback(ButtonClickCallback pCallback, void *pUserData);

private:
	MRTFLOAT m_fCurrentTime;
	CStaticText *m_pAnimatedText;
	CStaticText *m_pMessageText;
	CUIControl_TextButton *m_pCancelButton;
};

#endif //UISHEET_LOADING_H