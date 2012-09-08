/*======================================================

UISheet_Dots2dLobby.h

======================================================*/

#ifndef UISHEET_DOTS2DLOBBY_H
#define UISHEET_DOTS2DLOBBY_H

#include "../../CommonUI/UISheet.h"
#include "../../Callback.h"
#include "../../WebService.h"

class CUIControl_TextBox;

class CUISheet_Dots2dLobby : public CUISheet
{
public:
	CUISheet_Dots2dLobby();
	virtual ~CUISheet_Dots2dLobby() {}
private:
	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dLobby,OnClickLogout)
	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dLobby,OnClickCreateMatch)
	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dLobby,OnClickJoinMatch)
	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dLobby,OnClickQuickMatch)

	DEFINE_MEMBERCALLBACK(CUISheet_Dots2dLobby,OnCreateMatchCancel)
	DEFINE_MEMBERCALLBACK_ONEARG(CUISheet_Dots2dLobby,OnCreateMatchRequestComplete,WebService::EWebServiceResponse)

	CUIControl_TextBox *m_pCreateMatchName;
	CUIControl_TextBox *m_pJoinMatchName;
};

#endif //UISHEET_DOTS2DLOBBY_H