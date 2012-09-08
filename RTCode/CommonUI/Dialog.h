/*======================================================

Dialog.h

The base class for dialogs

======================================================*/

#ifndef DIALOG_H
#define DIALOG_H

#include "UISheet.h"

class CDialog : public CUISheet
{
public:
	CDialog();
	virtual ~CDialog();

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);
};

#endif //DIALOG_H