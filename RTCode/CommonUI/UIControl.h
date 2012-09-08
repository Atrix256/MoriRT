/*======================================================

UIControl.h

The base functionality for UI controls

======================================================*/

#ifndef UICONTROL_H
#define UICONTROL_H

#include "../Defines.h"

//forward declarations
class CUIStack;
class CUISheet;

/*
struct SEventInfo
{
	enum EEventType
	{
		kEvent_MouseButton
	};

	enum EMouseButton
	{
		kLeftMouse,
		kRightMouse,
		kMiddleMouse
	};

	struct SMouseButton
	{
		bool m_bDown;
		EMouseButton m_eMouseButton;
	};

	union
	{
		SMouseButton m_MouseButton;
	} m_uEventData;
};
*/

class CUIControl
{
public:
	CUIControl();
	virtual ~CUIControl();

	virtual bool OnKeyTyped(unsigned char nKey) {return false;}
	virtual bool OnKeyDown(unsigned char nKey) {return false;}
	virtual bool OnKeyUp(unsigned char nKey) {return false;}
	virtual bool OnLeftClick() {return false;}
	virtual void Update(MRTFLOAT fFrameTime) {}
	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack) {}

	virtual void SetPos(int nX, int nY) {m_nPosX = nX; m_nPosY = nY;}

	virtual bool CanAcceptFocus() {return false;}
	bool HasFocus();

	void SetParentSheet(CUISheet *pParentSheet) {m_pParentSheet = pParentSheet;}
	CUISheet *GetParentSheet() const {return m_pParentSheet;}

protected:

	CUISheet *m_pParentSheet;

	int m_nPosX;
	int m_nPosY;
};

#endif //UICONTROL_H
