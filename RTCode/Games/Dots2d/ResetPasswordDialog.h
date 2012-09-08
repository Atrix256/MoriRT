/*======================================================

ResetPasswordDialog.h

The logic for the reset password dialog

======================================================*/

#ifndef RESETPASSWORDDIALOG_H
#define RESETPASSWORDDIALOG_H

#include "../../CommonUI/Dialog.h"
#include "../../Callback.h"
#include "../../WebService.h"

//forward declarations
class CUIControl_TextBox;

class CResetPasswordDialog : public CDialog
{
public:
	CResetPasswordDialog();
	virtual ~CResetPasswordDialog() {}

	virtual bool OnKeyTyped(unsigned char nKey);

private:
	DEFINE_MEMBERCALLBACK(CResetPasswordDialog,OnCancelClicked)
	DEFINE_MEMBERCALLBACK(CResetPasswordDialog,OnOkClicked)
	DEFINE_MEMBERCALLBACK(CResetPasswordDialog,OnCreateAccountCancel)

	DEFINE_MEMBERCALLBACK_ONEARG(CResetPasswordDialog,OnCreateAccountRequestComplete,WebService::EWebServiceResponse)

	CUIControl_TextBox *m_pEmail;
};

#endif //RESETPASSWORDDIALOG_H