/*======================================================

StaticText.h

Static text

======================================================*/

#ifndef STATICTEXT_H
#define STATICTEXT_H

#include "UIControl.h"
#include "UIStack.h"
#include <string>

//forward declarations
class CUIStack;

class CStaticText : public CUIControl
{
public:
	CStaticText();
	virtual ~CStaticText() {}

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);
	void SetText(const char *szText);

	void GetSize(int &nX1, int &nY1, int &nX2, int &nY2) const;
	int GetWidth() const;
	int GetHeight() const;

	void SetAlignment(CUIStack::ETextAlignment eAlignment) {m_eAlignment = eAlignment;}
	CUIStack::ETextAlignment GetAlignment() const {return m_eAlignment;}

	void SetColor(Vec3 &vColor) {m_vColor = vColor;}
	Vec3 GetColor() const {return m_vColor;}

private:
	Vec3 m_vColor;
	std::string m_szText;
	CUIStack::ETextAlignment m_eAlignment;
};

#endif //UICONTROL_TEXTBUTTON_H
