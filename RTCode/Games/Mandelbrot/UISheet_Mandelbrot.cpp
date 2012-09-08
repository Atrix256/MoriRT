/*======================================================

UISheet_Mandelbrot.cpp

The place where the mandelbrot is actually rendered etc

======================================================*/

#include "UISheet_Mandelbrot.h"
#include "../../CommonUI/UIStack.h"

CUISheet_Mandelbrot::CUISheet_Mandelbrot()
{
}

CUISheet_Mandelbrot::~CUISheet_Mandelbrot()
{
}

void CUISheet_Mandelbrot::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	CUISheet::Render(nOffsetX,nOffsetY,pUIStack);

	int nWidth;
	int nHeight;
	int nRowPitch;
	unsigned char *pPixels = pUIStack->GetRawPixels(nWidth,nHeight,nRowPitch);

	static unsigned int nPixelOffset = 0;
	nPixelOffset+=640;

	unsigned int nPixel = 0;
	unsigned char *pCurrentRow = pPixels;
	for(int nIndexY = 0; nIndexY < nHeight; ++nIndexY)
	{
		unsigned char *pCurrentPixel = pCurrentRow;
		for(int nIndexX = 0; nIndexX < nWidth; ++nIndexX)
		{
			((unsigned int *)pCurrentPixel)[0] = nPixel + nPixelOffset;

			//pCurrentPixel[0] = 0; //blue
			//pCurrentPixel[1] = 0; //green
			//pCurrentPixel[2] = 0; //red
			//pCurrentPixel[3] = 255; //alpha

			pCurrentPixel += 4;
			nPixel++;
		}

		pCurrentRow += nRowPitch;
	}
}