/*======================================================

UISheet_Dots2dLobby.cpp

======================================================*/

#include "UISheet_Dots2dLobby.h"
#include "UISheet_Dots2dTitle.h"
#include "../../RayTracer.h"
#include <stdio.h>
#include "../../CommonUI/UIStack.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../CommonUI/TextBox.h"
#include "../../CommonUI/StaticText.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../WebService.h"
#include "../../StringBuffer.h"

CUISheet_Dots2dLobby::CUISheet_Dots2dLobby()
{
	CUIControl_TextButton *pTextButton = AddControl<CUIControl_TextButton>();
	pTextButton->SetPos(390,470);
	pTextButton->SetText("Logout");
	pTextButton->SetWidth(100);
	pTextButton->SetClickCallback(USE_MEMBERCALLBACK(OnClickLogout));

	StringBuffer<256> buffer;
	buffer.Format("Logged in as %s",WebService::GetSingleton().GetUserName());
	CStaticText *pStaticText = AddControl<CStaticText>();
	pStaticText->SetAlignment(CUIStack::kAlignmentRight);
	pStaticText->SetPos(380,475);
	pStaticText->SetText(buffer.Str());
	Vec3 vBlack(0.0f,0.0f,0.0f);
	pStaticText->SetColor(vBlack);

	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(10,10);
	pStaticText->SetText("Create match");

	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(20,35);
	pStaticText->SetText("Name:");

	CUIControl_TextBox *pTextBox = AddControl<CUIControl_TextBox>();
	pTextBox->SetPos(70,30);
	pTextBox->SetWidth(150);
	m_pCreateMatchName = pTextBox;

	CUIControl_TextButton *pButton = AddControl<CUIControl_TextButton>();
	pButton->SetText("Create");
	pButton->SetPos(230,30);
	pButton->SetWidth(75);
	pButton->SetClickCallback(USE_MEMBERCALLBACK(OnClickCreateMatch));



	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(10,70);
	pStaticText->SetText("Join match");

	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(20,95);
	pStaticText->SetText("Name:");

	pTextBox = AddControl<CUIControl_TextBox>();
	pTextBox->SetPos(70,90);
	pTextBox->SetWidth(150);
	m_pJoinMatchName = pTextBox;

	pButton = AddControl<CUIControl_TextButton>();
	pButton->SetText("Join");
	pButton->SetPos(230,90);
	pButton->SetWidth(75);
	pButton->SetClickCallback(USE_MEMBERCALLBACK(OnClickJoinMatch));

	pButton = AddControl<CUIControl_TextButton>();
	pButton->SetText("Quick Match");
	pButton->SetPos(10,130);
	pButton->SetWidth(100);
	pButton->SetClickCallback(USE_MEMBERCALLBACK(OnClickQuickMatch));
}

void CUISheet_Dots2dLobby::OnClickLogout()
{
	//log out from the web service
	WebService::GetSingleton().Logout();

	//go back to main menu
	Close();
	CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_Dots2dTitle>();
}

void CUISheet_Dots2dLobby::OnClickCreateMatch()
{
	//start our create match request
	WebService::GetSingleton().CreateMatch(m_pCreateMatchName->GetText(),4,false,USE_MEMBERCALLBACK(OnCreateMatchRequestComplete),"Creating Match");
}

void CUISheet_Dots2dLobby::OnClickJoinMatch()
{
	//int ijkl = 0;
}

void CUISheet_Dots2dLobby::OnClickQuickMatch()
{
	//int ijkl = 0;
}

void CUISheet_Dots2dLobby::OnCreateMatchRequestComplete(WebService::EWebServiceResponse eFinishCode)
{
	//int ijkl = 0;
}

void CUISheet_Dots2dLobby::OnCreateMatchCancel()
{
	WebService::GetSingleton().CancelCreateMatch();
}