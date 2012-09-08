/*======================================================

MessageBox.h

The functionality for a generic message box dialog

======================================================*/

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "Dialog.h"
#include "../Callback.h"
#include "UIControl_TextButton.h"
#include <string>

//forward declarations
class CStaticText;

class CMessageBox : public CDialog
{
public:
	CMessageBox();
	virtual ~CMessageBox() {}
	void SetText(const char *szText);
private:
	DEFINE_MEMBERCALLBACK(CMessageBox,OnOkClicked)

	CStaticText *m_pText;
};

#endif //MESSAGEBOX_H