/*======================================================

UIControl_ImageButton.h

An image button

======================================================*/

#ifndef UICONTROL_IMAGEBUTTON_H
#define UICONTROL_IMAGEBUTTON_H

#include "UIControl.h"
#include <string>

//forward declarations
class CUIStack;
class CTexture;

//typedefs
typedef void(*ButtonClickCallback)(void *);

class CUIControl_ImageButton : public CUIControl
{
public:
	CUIControl_ImageButton();
	virtual ~CUIControl_ImageButton() {}

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

	void SetImageIdle(const char *szFileName);
	void SetImageOver(const char *szFileName);
	void SetImageSelected(const char *szFileName);

	void SetClickCallback(ButtonClickCallback pCallback, void *pUserData) {m_pCallback = pCallback;m_pUserData = pUserData;}

	void SetScale(MRTFLOAT fScale) {m_fScale = fScale;}

	void SetSelected(bool bSelected) {m_bSelected = bSelected;}
	bool GetSelected() const {return m_bSelected;}

	virtual bool OnLeftClick();

private:

	MRTFLOAT m_fScale;

	bool m_bSelected;

	ButtonClickCallback m_pCallback;
	void *m_pUserData;
	
	CTexture *m_pTextureIdle;
	CTexture *m_pTextureOver;
	CTexture *m_pTextureSelected;
};

#endif //UICONTROL_IMAGEBUTTON_H
