/*======================================================

UISheet_Lost.h

The functionality for the dialog you see when you lose

======================================================*/

#ifndef UISHEET_LOST_H
#define UISHEET_LOST_H

#include "../../CommonUI/UISheet.h"

//forward declarations
class CTexture;

class CUISheet_Lost : public CUISheet
{
public:
	CUISheet_Lost();
	virtual ~CUISheet_Lost();

	virtual bool OnKeyDown(unsigned char nKey);
	virtual bool OnKeyUp(unsigned char nKey);
	virtual bool OnLeftClick();

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

protected:
	CTexture *m_pTexture;
};

#endif //UISHEET_LOST_H