/*======================================================

UIControl_TextButton.h

A text button

======================================================*/

#ifndef UICONTROL_TEXTBUTTON_H
#define UICONTROL_TEXTBUTTON_H

#include "UIControl.h"
#include <string>

//forward declarations
class CUIStack;

//typedefs
typedef void(*ButtonClickCallback)(void *);

class CUIControl_TextButton : public CUIControl
{
public:
	CUIControl_TextButton();
	virtual ~CUIControl_TextButton() {}

	virtual bool OnKeyDown(unsigned char nKey) {return false;}
	virtual bool OnKeyUp(unsigned char nKey) {return false;}
	virtual bool OnLeftClick();
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

	void SetText(const char *szText);

	void GetSize(int &nX1, int &nY1, int &nX2, int &nY2) const;
	
	int GetWidth() const;
	void SetWidth(int nWidth) {m_nForcedWidth = nWidth;}

	int GetHeight() const;

	int GetMargin() const {return m_nMargin;}

	void SetClickCallback(ButtonClickCallback pCallback, void *pUserData) {m_pCallback = pCallback;m_pUserData = pUserData;}

private:

	enum EState
	{
		kNormal,
		kMouseOver,
		kMouseDown
	};

	std::string m_szButtonText;
	int m_nMargin;
	int m_nForcedWidth;
	EState m_eButtonState;
	ButtonClickCallback m_pCallback;
	void *m_pUserData;
};

#endif //UICONTROL_TEXTBUTTON_H
