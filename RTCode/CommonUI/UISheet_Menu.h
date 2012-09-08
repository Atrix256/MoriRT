/*======================================================

UISheet_Menu.h

The functionality for the main menu

======================================================*/

#ifndef UISHEET_MENU_H
#define UISHEET_MENU_H

#include "UISheet.h"
#include <vector>

//forward declarations
class CUIControl_TextButton;

//typedefs
typedef std::vector<CUIControl_TextButton *> TextButtonList;

class CUISheet_Menu : public CUISheet
{
public:
	CUISheet_Menu();
	virtual ~CUISheet_Menu();

	virtual void Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack);

protected:
	virtual bool Internal_OnKeyDown(unsigned char nKey);
	virtual bool Internal_OnKeyUp(unsigned char nKey);
	virtual bool Internal_OnLeftClick();
	virtual void Internal_Update(MRTFLOAT fFrameTime);

	static void OnTogglePhong(void *pUserData);
	static void OnToggleShadows(void *pUserData);
	static void OnToggleReflections(void *pUserData);
	static void OnToggleTextures(void *pUserData);
	static void OnToggleRedBlue3d(void *pUserData);
	static void OnToggleRefractions(void *pUserData);
	static void OnToggleFPS(void *pUserData);
	static void OnToggleCamera(void *pUserData);
	static void OnToggleCellStats(void *pUserData);
	static void OnToggleWorldGrid(void *pUserData);
	static void OnToggleMousePos(void *pUserData);
	static void OnClickThreads(void *pUserData);
	static void OnClickRayBounce(void *pUserData);
	static void OnToggleRecording(void *pUserData);
	static void OnExit(void *pUserData);

	void NonStaticOnClickThreads();
	void NonStaticOnClickRayBounce();
	void NonStaticOnToggleRecording();
	void NonStaticOnToggleShadows();

	TextButtonList m_TextButtons;

	int m_nSheetWidth;
	int m_nSheetHeight;
	int m_nSheetPosX;
	int m_nSheetPosY;

	bool m_bRecording;

	CUIControl_TextButton *m_pRayBounceButton;
	CUIControl_TextButton *m_pThreadsButton;
	CUIControl_TextButton *m_pRecordingButton;
	CUIControl_TextButton *m_pShadowsButton;
};

#endif //UISHEET_MENU_H