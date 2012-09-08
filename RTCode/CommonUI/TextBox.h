/*======================================================

TextBox.h

A single line text input box

======================================================*/

#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "UIControl.h"
#include <string>

//forward declarations
class CUIStack;
class CTexture;

//typedefs
typedef void(*ButtonClickCallback)(void *);

class CUIControl_TextBox : public CUIControl
{
public:
	CUIControl_TextBox();
	virtual ~CUIControl_TextBox();

	virtual void Update(MRTFLOAT fFrameTime);
	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

	void SetWidth(MRTFLOAT fWidth) {m_fWidth = fWidth;}
	MRTFLOAT GetWidth() const {return m_fWidth;}
	MRTFLOAT GetHeight() const;

	void SetText(const char *szText) {m_szString = szText;}
	const char *GetText() const {return m_szString.c_str();}

	virtual bool CanAcceptFocus() {return true;}

	virtual bool OnLeftClick();
	virtual bool OnKeyTyped(unsigned char nKey);

	void SetPasswordMode(bool bEnable) {m_bPasswordMode = bEnable;}

private:
	std::string m_szString;
	MRTFLOAT m_fWidth;
	MRTFLOAT m_fHeight;
	MRTFLOAT m_fTime;

	int m_nTextCursorPos;

	bool m_bPasswordMode;
};

#endif //TEXTBOX_H