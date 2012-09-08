/*======================================================

UISheet_Mandelbrot.h

The place where the mandelbrot is actually rendered etc

======================================================*/

#ifndef UISHEET_MANDELBROT_H
#define UISHEET_MANDELBROT_H

#include "../../CommonUI/UISheet.h"

class CUISheet_Mandelbrot : public CUISheet
{
public:
	CUISheet_Mandelbrot();
	virtual ~CUISheet_Mandelbrot();

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);
};

#endif //UISHEET_MANDELBROT_H