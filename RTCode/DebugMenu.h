/*======================================================

DebugMenu.h

The functionality for the debug menu brought up by pressing ~

======================================================*/

#ifndef DEBUGMENU_H
#define DEBUGMENU_H

#include "Defines.h"

//forward declarations
class CUISheet;
class CUIStack;

class CDebugMenu
{
public:
	CDebugMenu();
	~CDebugMenu();

	bool OnKeyDown(unsigned char nKey);
	bool OnKeyUp(unsigned char nKey);
	bool OnLeftClick();

	void UpdateAndRender(CUIStack *pUIStack, MRTFLOAT fFrameTime);

	bool IsMenuOpen() const {return m_pDebugMenuSheet != 0;}

	void ToggleShowFPS() {m_bShowFPS = !m_bShowFPS;}
	void ToggleShowCameraInfo() {m_bShowCameraInfo = !m_bShowCameraInfo;}
	void ToggleShowCellStats() {m_bShowCellStats = !m_bShowCellStats;}
	void ToggleShowMousePos() {m_bShowMousePos = !m_bShowMousePos;}

	bool GetShowCellStats() {return m_bShowCellStats;}

	MRTFLOAT GetAvgFPS() const {return m_fAvgFPSLastValue;}

private:

	void OpenMenu();
	void CloseMenu();

	bool m_bShowFPS;
	bool m_bShowCameraInfo;
	bool m_bShowCellStats;
	bool m_bShowMousePos;

	MRTFLOAT m_fAvgFPSTimeBucket;
	int m_nAvgFPSFrameBucket;
	MRTFLOAT m_fAvgFPSLastValue;

	CUISheet *m_pDebugMenuSheet;
};

#endif //DEBUGMENU_H