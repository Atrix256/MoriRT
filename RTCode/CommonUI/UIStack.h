/*======================================================

UIStack.h

The functionality of the UI Stack

======================================================*/

#ifndef UISTACK_H
#define UISTACK_H

#include <vector>
#include "../Vec3.h"

//forward declarations
class CUISheet;
class CTexture;
class CUIControl;

//typedefs
typedef std::vector<CUISheet *> UISheetList;

class CUIStack
{
public:
	CUIStack();
	~CUIStack();

	enum ETextAlignment
	{
		kAlignmentLeft,
		kAlignmentCentered,
		kAlignmentRight
	};

	void Init(void);

	void OnKeyTyped(unsigned char nKey);
	bool OnKeyDown(unsigned char nKey);
	bool OnKeyUp(unsigned char nKey);
	bool OnLeftClick();

	void SetFocus(CUIControl *pFocusControl);

	void Update(MRTFLOAT fFrameTime);
	void SetRenderBuffer(unsigned char *pPixels, int nRowPitch, int nScreenWidth,int nScreenHeight);
	void Render();

	void MessageBox(const char *szText);

	template <class T>T *AddUISheetToTopOfStack()
	{
		T *pNewSheet = new T();

		m_UIStack.push_back(pNewSheet);

		return pNewSheet;
	}

	CUISheet *GetTopOfStack()
	{
		return *m_UIStack.rbegin();
	}

	void Draw2dBox(int nX1, int nY1, int nX2, int nY2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB);
	void Draw2dBoxOutline(int nX1, int nY1, int nX2, int nY2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB);
	void DrawVLine(int nX, int nY1, int nY2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB);
	void DrawHLine(int nY, int nX1, int nX2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB);
	void Draw3dShaded2dBox(int nX1, int nY1, int nX2, int nY2, MRTFLOAT fR, MRTFLOAT fG, MRTFLOAT fB, bool bOutset = true);

	void RenderTexture(int nX, int nY, int nW, int nH, CTexture *pTexture, MRTFLOAT fMinU = 0.0f, MRTFLOAT fMinV = 0.0f, MRTFLOAT fMaxU = 1.0f, MRTFLOAT fMaxV = 1.0f);
	void RenderTextureAlphaBlend(int nX, int nY, int nW, int nH, CTexture *pTexture, Vec3 vColorModulator, MRTFLOAT fMinU = 0.0f, MRTFLOAT fMinV = 0.0f, MRTFLOAT fMaxU = 1.0f, MRTFLOAT fMaxV = 1.0f);

	//text rendering stuff
	int GetTextHeight() const;
	int GetTextWidth(const char *szText) const;
	int GetCharacterWidth(unsigned char c) const;
	void GetTextRect(const char *szRect, int &nX1, int &nY1, int &nX2, int &nY2, ETextAlignment eAlign) const;
	void RenderText(int x, int y, const char *szText, Vec3 vColor, ETextAlignment eAlign, int nMaxLength = -1);

	unsigned char *GetRawPixels(int &nWidth, int &nHeight, int &nRowPitch);

	int GetScreenWidth() const {return m_nScreenWidth;}
	int GetScreenHeight() const {return m_nScreenHeight;}

private:
	void RemoveUISheet(CUISheet *pSheet);
	static unsigned int BlendColors(unsigned int nColor1, unsigned int nColor2, MRTFLOAT fBlend);

	UISheetList m_UIStack;

	//the info about where to render stuff
	unsigned char *m_pPixels;
	int m_nRowPitch;
	int m_nScreenWidth;
	int m_nScreenHeight;

	//text rendering members
	CTexture *m_pFontTexture;
	int m_nFontCharacterWidths[256];
};

#endif //UISTACK_H