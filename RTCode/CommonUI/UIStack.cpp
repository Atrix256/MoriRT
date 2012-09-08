/*======================================================

UIStack.cpp

The functionality of the UI Stack

======================================================*/

#include "UIStack.h"
#include "UISheet.h"
#include <algorithm>
#include "../RayTracer.h"
#include "../PlatformBase.h"
#include "../Texture.h"
#include "../TextureManager.h"
#include "MessageBox.h"

CUIStack::CUIStack()
:m_pPixels(NULL)
,m_nRowPitch(0)
,m_nScreenWidth(0)
,m_nScreenHeight(0)
{
	m_pFontTexture = 0;

	for(unsigned int nIndex = 0; nIndex < MRTARRAYSIZE(m_nFontCharacterWidths); ++nIndex)
	{
		m_nFontCharacterWidths[nIndex] = 0;
	}
}

CUIStack::~CUIStack()
{
	//free the memory for each UI Sheet
	for(UISheetList::iterator it = m_UIStack.begin(); it != m_UIStack.end(); ++it)
	{
		delete (*it);
	}

	//empty the ui sheet list
	m_UIStack.clear();

	TextureManager.FreeTexture(m_pFontTexture);
	m_pFontTexture = 0;
}

void CUIStack::RemoveUISheet(CUISheet * pSheet)
{
	UISheetList::iterator it = std::find(m_UIStack.begin(),m_UIStack.end(),pSheet);
	if(it != m_UIStack.end())
	{
		(*it)->OnEvent_Close();
		delete *it;
		m_UIStack.erase(it);
	}
}

void CUIStack::OnKeyTyped(unsigned char nKey)
{
	for(UISheetList::reverse_iterator it = m_UIStack.rbegin(); it != m_UIStack.rend(); ++it)
	{
		if((*it)->OnKeyTyped(nKey))
			return;

		//if this sheet is modal, stop here
		if((*it)->GetModal())
		{
			return;
		}
	}
}

bool CUIStack::OnKeyDown(unsigned char nKey)
{
	for(UISheetList::reverse_iterator it = m_UIStack.rbegin(); it != m_UIStack.rend(); ++it)
	{
		if((*it)->OnKeyDown(nKey))
			return true;

		//if this sheet is modal, stop here
		if((*it)->GetModal())
		{
			return true;
		}
	}

	return false;
}

bool CUIStack::OnKeyUp(unsigned char nKey)
{
	for(UISheetList::reverse_iterator it = m_UIStack.rbegin(); it != m_UIStack.rend(); ++it)
	{
		if((*it)->OnKeyUp(nKey))
			return true;

		//if this sheet is modal, stop here
		if((*it)->GetModal())
		{
			return true;
		}
	}

	//special handling for escape
	//if(m_UIStack.size() == 0 && nKey == 27)
	{
		//CRayTracer::GetSingleton().GetPlatform().ExitApplication();
	}


	return false;
}

void CUIStack::SetFocus(CUIControl *pFocusControl)
{
	if(!m_UIStack.empty())
	{
		(*m_UIStack.rbegin())->SetFocus(pFocusControl);
	}
}

bool CUIStack::OnLeftClick()
{
	for(UISheetList::reverse_iterator it = m_UIStack.rbegin(); it != m_UIStack.rend(); ++it)
	{
		if((*it)->OnLeftClick())
			return true;

		//if this sheet is modal, stop here
		if((*it)->GetModal())
		{
			return true;
		}
	}

	return false;
}

void CUIStack::Update(MRTFLOAT fFrameTime)
{
	//remove anyone who wants to be removed, give everyone else an update
	for(int nIndex = m_UIStack.size()-1; nIndex >= 0; --nIndex)
	{
		if(m_UIStack[nIndex]->WantsToClose())
		{
			RemoveUISheet(m_UIStack[nIndex]);
		}
		else
		{
			m_UIStack[nIndex]->Update(fFrameTime);
		}
	}
}

void CUIStack::Render()
{
	for(UISheetList::iterator it = m_UIStack.begin(); it != m_UIStack.end(); ++it)
	{
		(*it)->Render(0,0,this);
	}
}

void CUIStack::SetRenderBuffer(unsigned char *pPixels, int nRowPitch, int nScreenWidth,int nScreenHeight)
{
	m_pPixels = pPixels;
	m_nRowPitch = nRowPitch;
	m_nScreenWidth = nScreenWidth;
	m_nScreenHeight = nScreenHeight;
}

void CUIStack::Draw2dBoxOutline(int nX1, int nY1, int nX2, int nY2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB)
{
	DrawHLine(nY1,nX1,nX2,fR,fG,fB);
	DrawHLine(nY2,nX1,nX2,fR,fG,fB);
	DrawVLine(nX1,nY1,nY2,fR,fG,fB);
	DrawVLine(nX2,nY1,nY2,fR,fG,fB);
}

void CUIStack::Draw2dBox(int nX1, int nY1, int nX2, int nY2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB)
{
	//bail if completely off screen
	if(nX1 >= m_nScreenWidth ||
	   nY1 >= m_nScreenHeight ||
	   nX2 < 0 ||
	   nY2 < 0)
		return;

	//make the color
	unsigned int nColor;
	unsigned char *pColor = (unsigned char *)&nColor;
	pColor[0] = (unsigned char)(fB * 255.0f);
	pColor[1] = (unsigned char)(fG * 255.0f);
	pColor[2] = (unsigned char)(fR * 255.0f);
	pColor[3] = 255;

	//clip box coordinates to the screen
	if(nX1 < 0)
		nX1 = 0;
	else if(nX1 >= m_nScreenWidth)
		nX1 = m_nScreenWidth - 1;

	if(nX2 < 0)
		nX2 = 0;
	else if(nX2 >= m_nScreenWidth)
		nX2 = m_nScreenWidth - 1;

	if(nY1 < 0)
		nY1 = 0;
	else if(nY1 >= m_nScreenHeight)
		nY1 = m_nScreenHeight - 1;

	if(nY2 < 0)
		nY2 = 0;
	else if(nY2 >= m_nScreenHeight)
		nY2 = m_nScreenHeight - 1;

	//calculate some values to render the box
	unsigned char *pCurrentRow = &m_pPixels[(nY1 * m_nRowPitch + nX1 * 4)];
	int nWidth = (nX2 - nX1) + 1;
	int nHeight = (nY2 - nY1) + 1;

	//render the box
	for(int nIndexY = 0; nIndexY < nHeight; ++nIndexY)
	{
		unsigned int *pCurrentPixel = (unsigned int *)pCurrentRow;

		for(int nIndexX = 0; nIndexX < nWidth; ++nIndexX)
		{
			//this looks nice but it's very very very slow!
			//unsigned int nOldColor = *pCurrentPixel;
			//unsigned int nNewColor = BlendColors(nOldColor,nColor,0.6f);
			//*pCurrentPixel = nNewColor;

			*pCurrentPixel = nColor;
			pCurrentPixel++;
		}

		pCurrentRow += m_nRowPitch;
	}
}

void CUIStack::DrawVLine(int nX, int nY1, int nY2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB)
{
	Draw2dBox(nX,nY1,nX,nY2,fR,fG,fB);
}

void CUIStack::DrawHLine(int nY, int nX1, int nX2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB)
{
	Draw2dBox(nX1,nY,nX2,nY,fR,fG,fB);
}

void CUIStack::RenderTextureAlphaBlend(int nX, int nY, int nW, int nH, CTexture *pTexture, Vec3 vColorModulator, MRTFLOAT fMinU /*= 0.0f*/, MRTFLOAT fMinV /*= 0.0f*/, MRTFLOAT fMaxU /*= 1.0f*/, MRTFLOAT fMaxV /*= 1.0f*/)
{
	MRTFLOAT fStartPercentX = fMinU;
	MRTFLOAT fStartPercentY = fMinV;

	/*
	if(nX < 0)
	{
		int nPixelChop = nX *-1;
		MRTFLOAT fChopPercent = ((MRTFLOAT)nPixelChop) / ((MRTFLOAT)nW);
		nX = 0;
		nW -= nPixelChop;
		fMinU += (fMaxU - fMinU) * fChopPercent;
	}*/

	MRTFLOAT fPercentIncrementX = (fMaxU - fMinU) / (MRTFLOAT)nW;
	MRTFLOAT fPercentIncrementY = (fMaxV - fMinV) / (MRTFLOAT)nH;

	//calculate some values to render the box
	unsigned char *pCurrentRow = &m_pPixels[(nY * m_nRowPitch + nX * 4)];

	//render the box
	MRTFLOAT fPercentY = fStartPercentY;
	for(int nIndexY = 0; nIndexY < nH; ++nIndexY)
	{
		unsigned int *pCurrentPixel = (unsigned int *)pCurrentRow;

		MRTFLOAT fPercentX = fStartPercentX;
		for(int nIndexX = 0; nIndexX < nW; ++nIndexX)
		{
			//sample the texel
			MRTFLOAT fAlpha = 0.0f;
			Vec3 vColor = pTexture->GetTexelBilinear(fPercentX,fPercentY,&fAlpha);

			//multiply by the color modulator
			vColor = vColor * vColorModulator;

			unsigned int nColor;
			unsigned char *pColor = (unsigned char *)&nColor;

			if(CRayTracer::GetSingleton().GetRedBlue3dModeOn())
			{
				MRTFLOAT fMonoChrome = vColor.GetMonochromeFromRGB();

				pColor[0] = (unsigned char)(fMonoChrome*255.0f);
				pColor[1] = 0;
				pColor[2] = (unsigned char)(fMonoChrome*255.0f);
				pColor[3] = 255;
			}
			else
			{
				pColor[0] = (unsigned char)(vColor.m_fZ * 255.0f);
				pColor[1] = (unsigned char)(vColor.m_fY * 255.0f);
				pColor[2] = (unsigned char)(vColor.m_fX * 255.0f);
				pColor[3] = 255;
			}

			//alpha blend
			unsigned int nOldColor = *pCurrentPixel;
			unsigned int nNewColor = BlendColors(nOldColor,nColor,fAlpha);

			//write the pixel
			*pCurrentPixel = nNewColor;

			//move to the next pixel
			pCurrentPixel++;
			fPercentX += fPercentIncrementX;
		}

		//move to the next row
		pCurrentRow += m_nRowPitch;
		fPercentY += fPercentIncrementY;
	}
}

void CUIStack::RenderTexture(int nX, int nY, int nW, int nH, CTexture *pTexture, MRTFLOAT fMinU /*= 0.0f*/, MRTFLOAT fMinV /*= 0.0f*/, MRTFLOAT fMaxU /*= 1.0f*/, MRTFLOAT fMaxV /*= 1.0f*/)
{
	MRTFLOAT fStartPercentX = fMinU;
	MRTFLOAT fStartPercentY = fMinV;

	MRTFLOAT fPercentIncrementX = (fMaxU - fMinU) / (MRTFLOAT)nW;
	MRTFLOAT fPercentIncrementY = (fMaxV - fMinV) / (MRTFLOAT)nH;

	//calculate some values to render the box
	unsigned char *pCurrentRow = &m_pPixels[(nY * m_nRowPitch + nX * 4)];

	//render the box
	MRTFLOAT fPercentY = fStartPercentY;
	for(int nIndexY = 0; nIndexY < nH; ++nIndexY)
	{
		unsigned int *pCurrentPixel = (unsigned int *)pCurrentRow;

		MRTFLOAT fPercentX = fStartPercentX;
		for(int nIndexX = 0; nIndexX < nW; ++nIndexX)
		{
			//this looks nice but it's very very very slow!
			//unsigned int nOldColor = *pCurrentPixel;
			//unsigned int nNewColor = BlendColors(nOldColor,nColor,0.6f);
			//*pCurrentPixel = nNewColor;

			Vec3 vColor = pTexture->GetTexelBilinear(fPercentX,fPercentY);
			unsigned int nColor;
			unsigned char *pColor = (unsigned char *)&nColor;

			if(CRayTracer::GetSingleton().GetRedBlue3dModeOn())
			{
				MRTFLOAT fMonoChrome = vColor.GetMonochromeFromRGB();

				pColor[0] = (unsigned char)(fMonoChrome*255.0f);
				pColor[1] = 0;
				pColor[2] = (unsigned char)(fMonoChrome*255.0f);
				pColor[3] = 255;
			}
			else
			{
				pColor[0] = (unsigned char)(vColor.m_fZ * 255.0f);
				pColor[1] = (unsigned char)(vColor.m_fY * 255.0f);
				pColor[2] = (unsigned char)(vColor.m_fX * 255.0f);
				pColor[3] = 255;
			}

			*pCurrentPixel = nColor;

			pCurrentPixel++;

			fPercentX += fPercentIncrementX;
		}

		pCurrentRow += m_nRowPitch;
		fPercentY += fPercentIncrementY;
	}

}

void CUIStack::Draw3dShaded2dBox(int nX1, int nY1, int nX2, int nY2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB, bool bOutset /*= true*/)
{
	MRTFLOAT fRLight = fR + 0.1f;
	MRTFLOAT fGLight = fG + 0.1f;
	MRTFLOAT fBLight = fB + 0.1f;

	MRTFLOAT fRDark = fR / 3.0f;
	MRTFLOAT fGDark = fG / 3.0f;
	MRTFLOAT fBDark = fB / 3.0f;

	if(fRLight > 1.0f)
		fRLight = 1.0f;

	if(fGLight > 1.0f)
		fGLight = 1.0f;

	if(fBLight > 1.0f)
		fBLight = 1.0f;

	if(fRDark < 0.0f)
		fRDark = 0.0f;

	if(fGDark < 0.0f)
		fGDark = 0.0f;

	if(fBDark < 0.0f)
		fBDark = 0.0f;

	Draw2dBox(nX1,nY1,nX2,nY2,fR,fG,fB);

	if(bOutset)
	{
		DrawHLine(nY1,nX1,nX2,fRLight,fGLight,fBLight);
		DrawVLine(nX1,nY1,nY2,fRLight,fGLight,fBLight);
		DrawHLine(nY2,nX1,nX2,fRDark,fGDark,fBDark);
		DrawVLine(nX2,nY1,nY2,fRDark,fGDark,fBDark);
	}
	else
	{
		DrawHLine(nY1,nX1,nX2,fRDark,fGDark,fBDark);
		DrawVLine(nX1,nY1,nY2,fRDark,fGDark,fBDark);
		DrawHLine(nY2,nX1,nX2,fRLight,fGLight,fBLight);
		DrawVLine(nX2,nY1,nY2,fRLight,fGLight,fBLight);
	}
}

unsigned int CUIStack::BlendColors(unsigned int nColor1, unsigned int nColor2, MRTFLOAT fBlend)
{
	unsigned int nDestColor;
	unsigned char *pDestColor = (unsigned char *)&nDestColor;
	unsigned char *pColor1 = (unsigned char *)&nColor1;
	unsigned char *pColor2 = (unsigned char *)&nColor2;

	pDestColor[3] = 255;
	for(int nIndex = 0; nIndex < 3; ++nIndex)
	{
		pDestColor[nIndex] = (unsigned char)((MRTFLOAT)pColor1[nIndex]  * (1.0f - fBlend) + (MRTFLOAT)pColor2[nIndex] * fBlend);
	}

	return nDestColor;
}

int CUIStack::GetTextHeight() const
{
	return 16;
}

int CUIStack::GetCharacterWidth(unsigned char c) const
{
	return m_nFontCharacterWidths[c];
}

void CUIStack::GetTextRect(const char *szRect, int &nX1, int &nY1, int &nX2, int &nY2, ETextAlignment eAlign) const
{
	nX1 = 0;
	nY1 = 0;
	nX2 = GetTextWidth(szRect);
	nY2 = GetTextHeight();
}

int CUIStack::GetTextWidth(const char *szText) const
{

	int nWidth = 0;
	const char *pIndex = szText;

	while(*pIndex)
	{
		nWidth += GetCharacterWidth(*pIndex);
		++pIndex;
	}

	return nWidth;
}

void CUIStack::RenderText(int x, int y, const char *szText, Vec3 vColor, ETextAlignment eAlign, int nMaxLength /*= -1*/)
{
	int nStartX = x;

	switch(eAlign)
	{
		//nothing to do for left alignment
		case kAlignmentLeft:
		{
			if(szText[0])
			{
				int nCharacterWidth = GetCharacterWidth(szText[0]);
				int nRenderOffset = (16 - nCharacterWidth) / 2;
				x += nRenderOffset / 2;
			}

			break;
		}
		//if centered, need to move x left, by half of the width of the string
		case kAlignmentCentered:
		{
			x -= GetTextWidth(szText) / 2;
			break;
		}
		//if right aligned, need to move x left, by the full width of the string
		case kAlignmentRight:
		{
			x -= GetTextWidth(szText);
			break;
		}
	}

	//for each character in the string
	const char *pIndex = szText;
	while(*pIndex)
	{
		//calculate the X and Y location of the glyph
		int nIndexX = (*pIndex) % 16;
		int nIndexY = (*pIndex) / 16;

		//calculate UV coordinates of the tile
		MRTFLOAT fMinU,fMinV,fMaxU,fMaxV;
		fMinU = ((MRTFLOAT)nIndexX) / 16.0f;
		fMinV = ((MRTFLOAT)nIndexY) / 16.0f;
		fMaxU = fMinU + 1.0f / 16.0f;
		fMaxV = fMinV + 1.0f / 16.0f;

		//calculate some metrics for the character
		int nCharacterWidth = GetCharacterWidth(*pIndex);
		int nRenderOffset = (16 - nCharacterWidth) / 2;

		if(nMaxLength > 0 && x - nRenderOffset + nCharacterWidth > nStartX + nMaxLength)
		{
			return;
		}

		//render the character
		RenderTextureAlphaBlend(x - nRenderOffset,y,16,16,m_pFontTexture,vColor,fMinU,fMinV,fMaxU,fMaxV);

		//move based on our character width
		x+= nCharacterWidth;

		//move to the next character in the string
		++pIndex;
	}
}

unsigned char *CUIStack::GetRawPixels(int &nWidth, int &nHeight, int &nRowPitch)
{
	nWidth = m_nScreenWidth;
	nHeight = m_nScreenHeight;
	nRowPitch = m_nRowPitch;
	return m_pPixels;
}

void CUIStack::Init(void)
{
	//load the texture
	m_pFontTexture = TextureManager.LoadTexture("Assets/Common/Fonts/Arial.tga");

	//set the font widths.  coppied from the ini file
	m_nFontCharacterWidths[0]=8;
	m_nFontCharacterWidths[1]=15;
	m_nFontCharacterWidths[2]=8;
	m_nFontCharacterWidths[3]=8;
	m_nFontCharacterWidths[4]=8;
	m_nFontCharacterWidths[5]=8;
	m_nFontCharacterWidths[6]=8;
	m_nFontCharacterWidths[7]=8;
	m_nFontCharacterWidths[8]=8;
	m_nFontCharacterWidths[9]=48;
	m_nFontCharacterWidths[10]=0;
	m_nFontCharacterWidths[11]=8;
	m_nFontCharacterWidths[12]=8;
	m_nFontCharacterWidths[13]=0;
	m_nFontCharacterWidths[14]=8;
	m_nFontCharacterWidths[15]=8;
	m_nFontCharacterWidths[16]=8;
	m_nFontCharacterWidths[17]=8;
	m_nFontCharacterWidths[18]=8;
	m_nFontCharacterWidths[19]=8;
	m_nFontCharacterWidths[20]=8;
	m_nFontCharacterWidths[21]=8;
	m_nFontCharacterWidths[22]=8;
	m_nFontCharacterWidths[23]=8;
	m_nFontCharacterWidths[24]=8;
	m_nFontCharacterWidths[25]=8;
	m_nFontCharacterWidths[26]=8;
	m_nFontCharacterWidths[27]=8;
	m_nFontCharacterWidths[28]=0;
	m_nFontCharacterWidths[29]=0;
	m_nFontCharacterWidths[30]=0;
	m_nFontCharacterWidths[31]=0;
	m_nFontCharacterWidths[32]=4;
	m_nFontCharacterWidths[33]=5;
	m_nFontCharacterWidths[34]=5;
	m_nFontCharacterWidths[35]=8;
	m_nFontCharacterWidths[36]=8;
	m_nFontCharacterWidths[37]=12;
	m_nFontCharacterWidths[38]=9;
	m_nFontCharacterWidths[39]=3;
	m_nFontCharacterWidths[40]=5;
	m_nFontCharacterWidths[41]=5;
	m_nFontCharacterWidths[42]=5;
	m_nFontCharacterWidths[43]=8;
	m_nFontCharacterWidths[44]=4;
	m_nFontCharacterWidths[45]=5;
	m_nFontCharacterWidths[46]=4;
	m_nFontCharacterWidths[47]=4;
	m_nFontCharacterWidths[48]=8;
	m_nFontCharacterWidths[49]=8;
	m_nFontCharacterWidths[50]=8;
	m_nFontCharacterWidths[51]=8;
	m_nFontCharacterWidths[52]=8;
	m_nFontCharacterWidths[53]=8;
	m_nFontCharacterWidths[54]=8;
	m_nFontCharacterWidths[55]=8;
	m_nFontCharacterWidths[56]=8;
	m_nFontCharacterWidths[57]=8;
	m_nFontCharacterWidths[58]=4;
	m_nFontCharacterWidths[59]=4;
	m_nFontCharacterWidths[60]=8;
	m_nFontCharacterWidths[61]=8;
	m_nFontCharacterWidths[62]=8;
	m_nFontCharacterWidths[63]=8;
	m_nFontCharacterWidths[64]=14;
	m_nFontCharacterWidths[65]=9;
	m_nFontCharacterWidths[66]=9;
	m_nFontCharacterWidths[67]=10;
	m_nFontCharacterWidths[68]=10;
	m_nFontCharacterWidths[69]=9;
	m_nFontCharacterWidths[70]=9;
	m_nFontCharacterWidths[71]=11;
	m_nFontCharacterWidths[72]=9;
	m_nFontCharacterWidths[73]=3;
	m_nFontCharacterWidths[74]=6;
	m_nFontCharacterWidths[75]=9;
	m_nFontCharacterWidths[76]=8;
	m_nFontCharacterWidths[77]=11;
	m_nFontCharacterWidths[78]=9;
	m_nFontCharacterWidths[79]=11;
	m_nFontCharacterWidths[80]=9;
	m_nFontCharacterWidths[81]=11;
	m_nFontCharacterWidths[82]=10;
	m_nFontCharacterWidths[83]=9;
	m_nFontCharacterWidths[84]=9;
	m_nFontCharacterWidths[85]=9;
	m_nFontCharacterWidths[86]=9;
	m_nFontCharacterWidths[87]=13;
	m_nFontCharacterWidths[88]=8;
	m_nFontCharacterWidths[89]=9;
	m_nFontCharacterWidths[90]=8;
	m_nFontCharacterWidths[91]=4;
	m_nFontCharacterWidths[92]=4;
	m_nFontCharacterWidths[93]=4;
	m_nFontCharacterWidths[94]=5;
	m_nFontCharacterWidths[95]=8;
	m_nFontCharacterWidths[96]=5;
	m_nFontCharacterWidths[97]=8;
	m_nFontCharacterWidths[98]=8;
	m_nFontCharacterWidths[99]=7;
	m_nFontCharacterWidths[100]=8;
	m_nFontCharacterWidths[101]=8;
	m_nFontCharacterWidths[102]=4;
	m_nFontCharacterWidths[103]=8;
	m_nFontCharacterWidths[104]=8;
	m_nFontCharacterWidths[105]=3;
	m_nFontCharacterWidths[106]=3;
	m_nFontCharacterWidths[107]=7;
	m_nFontCharacterWidths[108]=3;
	m_nFontCharacterWidths[109]=11;
	m_nFontCharacterWidths[110]=8;
	m_nFontCharacterWidths[111]=8;
	m_nFontCharacterWidths[112]=8;
	m_nFontCharacterWidths[113]=8;
	m_nFontCharacterWidths[114]=5;
	m_nFontCharacterWidths[115]=7;
	m_nFontCharacterWidths[116]=4;
	m_nFontCharacterWidths[117]=8;
	m_nFontCharacterWidths[118]=7;
	m_nFontCharacterWidths[119]=9;
	m_nFontCharacterWidths[120]=6;
	m_nFontCharacterWidths[121]=7;
	m_nFontCharacterWidths[122]=6;
	m_nFontCharacterWidths[123]=5;
	m_nFontCharacterWidths[124]=3;
	m_nFontCharacterWidths[125]=5;
	m_nFontCharacterWidths[126]=8;
	m_nFontCharacterWidths[127]=3;
	m_nFontCharacterWidths[128]=8;
	m_nFontCharacterWidths[129]=0;
	m_nFontCharacterWidths[130]=3;
	m_nFontCharacterWidths[131]=8;
	m_nFontCharacterWidths[132]=5;
	m_nFontCharacterWidths[133]=14;
	m_nFontCharacterWidths[134]=8;
	m_nFontCharacterWidths[135]=8;
	m_nFontCharacterWidths[136]=5;
	m_nFontCharacterWidths[137]=14;
	m_nFontCharacterWidths[138]=9;
	m_nFontCharacterWidths[139]=5;
	m_nFontCharacterWidths[140]=14;
	m_nFontCharacterWidths[141]=0;
	m_nFontCharacterWidths[142]=8;
	m_nFontCharacterWidths[143]=0;
	m_nFontCharacterWidths[144]=0;
	m_nFontCharacterWidths[145]=3;
	m_nFontCharacterWidths[146]=3;
	m_nFontCharacterWidths[147]=5;
	m_nFontCharacterWidths[148]=5;
	m_nFontCharacterWidths[149]=5;
	m_nFontCharacterWidths[150]=8;
	m_nFontCharacterWidths[151]=14;
	m_nFontCharacterWidths[152]=4;
	m_nFontCharacterWidths[153]=14;
	m_nFontCharacterWidths[154]=7;
	m_nFontCharacterWidths[155]=5;
	m_nFontCharacterWidths[156]=13;
	m_nFontCharacterWidths[157]=0;
	m_nFontCharacterWidths[158]=6;
	m_nFontCharacterWidths[159]=9;
	m_nFontCharacterWidths[160]=4;
	m_nFontCharacterWidths[161]=5;
	m_nFontCharacterWidths[162]=8;
	m_nFontCharacterWidths[163]=8;
	m_nFontCharacterWidths[164]=8;
	m_nFontCharacterWidths[165]=8;
	m_nFontCharacterWidths[166]=3;
	m_nFontCharacterWidths[167]=8;
	m_nFontCharacterWidths[168]=5;
	m_nFontCharacterWidths[169]=10;
	m_nFontCharacterWidths[170]=4;
	m_nFontCharacterWidths[171]=8;
	m_nFontCharacterWidths[172]=8;
	m_nFontCharacterWidths[173]=5;
	m_nFontCharacterWidths[174]=10;
	m_nFontCharacterWidths[175]=8;
	m_nFontCharacterWidths[176]=6;
	m_nFontCharacterWidths[177]=8;
	m_nFontCharacterWidths[178]=5;
	m_nFontCharacterWidths[179]=5;
	m_nFontCharacterWidths[180]=5;
	m_nFontCharacterWidths[181]=8;
	m_nFontCharacterWidths[182]=8;
	m_nFontCharacterWidths[183]=5;
	m_nFontCharacterWidths[184]=5;
	m_nFontCharacterWidths[185]=5;
	m_nFontCharacterWidths[186]=5;
	m_nFontCharacterWidths[187]=8;
	m_nFontCharacterWidths[188]=12;
	m_nFontCharacterWidths[189]=12;
	m_nFontCharacterWidths[190]=12;
	m_nFontCharacterWidths[191]=9;
	m_nFontCharacterWidths[192]=9;
	m_nFontCharacterWidths[193]=9;
	m_nFontCharacterWidths[194]=9;
	m_nFontCharacterWidths[195]=9;
	m_nFontCharacterWidths[196]=9;
	m_nFontCharacterWidths[197]=9;
	m_nFontCharacterWidths[198]=14;
	m_nFontCharacterWidths[199]=10;
	m_nFontCharacterWidths[200]=9;
	m_nFontCharacterWidths[201]=9;
	m_nFontCharacterWidths[202]=9;
	m_nFontCharacterWidths[203]=9;
	m_nFontCharacterWidths[204]=4;
	m_nFontCharacterWidths[205]=4;
	m_nFontCharacterWidths[206]=4;
	m_nFontCharacterWidths[207]=4;
	m_nFontCharacterWidths[208]=10;
	m_nFontCharacterWidths[209]=10;
	m_nFontCharacterWidths[210]=11;
	m_nFontCharacterWidths[211]=11;
	m_nFontCharacterWidths[212]=11;
	m_nFontCharacterWidths[213]=11;
	m_nFontCharacterWidths[214]=11;
	m_nFontCharacterWidths[215]=8;
	m_nFontCharacterWidths[216]=11;
	m_nFontCharacterWidths[217]=10;
	m_nFontCharacterWidths[218]=10;
	m_nFontCharacterWidths[219]=10;
	m_nFontCharacterWidths[220]=10;
	m_nFontCharacterWidths[221]=9;
	m_nFontCharacterWidths[222]=9;
	m_nFontCharacterWidths[223]=9;
	m_nFontCharacterWidths[224]=8;
	m_nFontCharacterWidths[225]=8;
	m_nFontCharacterWidths[226]=8;
	m_nFontCharacterWidths[227]=8;
	m_nFontCharacterWidths[228]=8;
	m_nFontCharacterWidths[229]=8;
	m_nFontCharacterWidths[230]=12;
	m_nFontCharacterWidths[231]=7;
	m_nFontCharacterWidths[232]=8;
	m_nFontCharacterWidths[233]=8;
	m_nFontCharacterWidths[234]=8;
	m_nFontCharacterWidths[235]=8;
	m_nFontCharacterWidths[236]=4;
	m_nFontCharacterWidths[237]=4;
	m_nFontCharacterWidths[238]=4;
	m_nFontCharacterWidths[239]=4;
	m_nFontCharacterWidths[240]=8;
	m_nFontCharacterWidths[241]=8;
	m_nFontCharacterWidths[242]=8;
	m_nFontCharacterWidths[243]=8;
	m_nFontCharacterWidths[244]=8;
	m_nFontCharacterWidths[245]=8;
	m_nFontCharacterWidths[246]=8;
	m_nFontCharacterWidths[247]=8;
	m_nFontCharacterWidths[248]=8;
	m_nFontCharacterWidths[249]=8;
	m_nFontCharacterWidths[250]=8;
	m_nFontCharacterWidths[251]=8;
	m_nFontCharacterWidths[252]=8;
	m_nFontCharacterWidths[253]=7;
	m_nFontCharacterWidths[254]=8;
	m_nFontCharacterWidths[255]=7;
}

void CUIStack::MessageBox(const char *szText)
{
	CMessageBox *pMessageBox = AddUISheetToTopOfStack<CMessageBox>();
	pMessageBox->SetText(szText);
}