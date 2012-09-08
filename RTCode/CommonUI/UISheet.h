/*======================================================

UISheet.h

The base functionality of the UI Sheets

======================================================*/

#ifndef UISHEET_H
#define UISHEET_H

#include "../Defines.h"
#include <vector>

//forward declarations
class CUIStack;
class CUIControl;

//typedefs
typedef std::vector<CUIControl *> UIControlList;
typedef void(*CallbackFunction)(void *pArg);

class CUISheet
{
public:
	CUISheet();
	virtual ~CUISheet();

	virtual bool OnKeyTyped(unsigned char nKey);
	virtual bool OnKeyDown(unsigned char nKey);
	virtual bool OnKeyUp(unsigned char nKey);
	virtual bool OnLeftClick();
	virtual void Update(MRTFLOAT fFrameTime);
	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

	void SetFocus(CUIControl *pFocusControl);
	void SetModal(bool bModal) {m_bModal = bModal;}
	bool GetModal() const {return m_bModal;}

	void Close() {m_bWantsToClose = true;}
	bool WantsToClose() const {return m_bWantsToClose;}

	void SetOnCloseCallback(CallbackFunction pCloseCallback, void *pArg)
	{
		m_pCloseCallback = pCloseCallback;
		m_pCloseCallbackArg = pArg;
	}

	void OnEvent_Close()
	{
		if(m_pCloseCallback)
		{
			m_pCloseCallback(m_pCloseCallbackArg);
		}
	}

	bool HasFocus() const;

	int GetPositionX() const {return m_nPosX;}
	int GetPositionY() const {return m_nPosY;}

	void SetPosition(int nPosX,int nPosY)
	{
		m_nPosX = nPosX;
		m_nPosY = nPosY;
	}

	void SetDimensions(int nWidth, int nHeight)
	{
		m_nWidth = nWidth;
		m_nHeight = nHeight;
	}

	void OnControlDeleted(CUIControl *pControl);

	CUIControl *GetFocusedControl();

protected:
	void CycleFocus();

	virtual bool Internal_OnKeyTyped(unsigned char nKey) {return false;}
	virtual bool Internal_OnKeyDown(unsigned char nKey) {return false;}
	virtual bool Internal_OnKeyUp(unsigned char nKey) {return false;}
	virtual bool Internal_OnLeftClick() {return false;}
	virtual void Internal_Update(MRTFLOAT fFrameTime) {}

	template <class T>T *AddControl()
	{
		T *pNewControl = new T();

		m_UIControlls.push_back(pNewControl);
		pNewControl->SetParentSheet(this);

		return pNewControl;
	}

	UIControlList m_UIControlls;

	bool m_bWantsToClose;
	CallbackFunction m_pCloseCallback;
	void *m_pCloseCallbackArg;

	CUIControl *m_pFocusedControl;

	bool m_bModal;

	int m_nPosX;
	int m_nPosY;
	int m_nWidth;
	int m_nHeight;
};

#endif //UISHEET_H