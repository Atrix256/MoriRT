/*======================================================

NewAccountDialog.h

The logic for the new account dialog

======================================================*/

#ifndef NEWACCOUNTDIALOG_H
#define NEWACCOUNTDIALOG_H

#include "../../CommonUI/Dialog.h"
#include "../../Callback.h"
#include "../../WebService.h"

//forward declarations
class CUIControl_TextBox;

class CNewAccountDialog : public CDialog
{
public:
	CNewAccountDialog();
	virtual ~CNewAccountDialog() {}

	virtual bool OnKeyTyped(unsigned char nKey);

private:
	DEFINE_MEMBERCALLBACK(CNewAccountDialog,OnCancelClicked)
	DEFINE_MEMBERCALLBACK(CNewAccountDialog,OnOkClicked)
	DEFINE_MEMBERCALLBACK(CNewAccountDialog,OnCreateAccountCancel)
	DEFINE_MEMBERCALLBACK_ONEARG(CNewAccountDialog,OnCreateAccountRequestComplete,WebService::EWebServiceResponse)

	CUIControl_TextBox *m_pUsername;
	CUIControl_TextBox *m_pPassword1;
	CUIControl_TextBox *m_pPassword2;
	CUIControl_TextBox *m_pEmail;
};

#endif //NEWACCOUNTDIALOG_H