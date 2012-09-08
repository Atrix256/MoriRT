/*======================================================

UISheet_Dots2dTitle.cpp

The functionality for the title screen in the dots 2d game

======================================================*/

#include "UISheet_Dots2dTitle.h"
#include <stdio.h>
#include "../../RayTracer.h"
#include "../../PlatformBase.h"
#include "../../TextureManager.h"
#include "../../CommonUI/UIStack.h"
#include "../../CommonUI/TextBox.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../CommonUI/StaticText.h"
#include "UISheet_Dots2dLobby.h"
#include "NewAccountDialog.h"
#include "../../StringBuffer.h"
#include "../../WebService.h"

CUISheet_Dots2dTitle::CUISheet_Dots2dTitle()
:m_pUserName(NULL)
,m_pPassword(NULL)
,m_fCurrentTime(0)
{
	//add the background and title image
	m_pTitleImage = TextureManager.LoadTexture("Assets/Dots2d/title.png");
	
	//add the username box and set focus to it
	m_pUserName = AddControl<CUIControl_TextBox>();
	m_pUserName->SetPos(210,250);
	m_pUserName->SetWidth(150);
	SetFocus(m_pUserName);

	//add the username label text
	CStaticText *pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(200,255);
	Vec3 vWhite(1.0f,1.0f,1.0f);
	pStaticText->SetColor(vWhite);
	pStaticText->SetAlignment(CUIStack::kAlignmentRight);
	pStaticText->SetText("Username:");

	//add the password box
	m_pPassword = AddControl<CUIControl_TextBox>();
	m_pPassword->SetPos(210,290);
	m_pPassword->SetWidth(150);
	m_pPassword->SetPasswordMode(true);

	//add the username label text
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(200,295);
	pStaticText->SetColor(vWhite);
	pStaticText->SetAlignment(CUIStack::kAlignmentRight);
	pStaticText->SetText("Password:");

	//add a new account button
	CUIControl_TextButton *pTextButton = AddControl<CUIControl_TextButton>();
	pTextButton->SetPos(165,340);
	pTextButton->SetText("New Account");
	pTextButton->SetWidth(100);
	pTextButton->SetClickCallback(USE_MEMBERCALLBACK(OnClickNewAccount));

	//add a login box
	pTextButton = AddControl<CUIControl_TextButton>();
	pTextButton->SetPos(275,340);
	pTextButton->SetText("Login");
	pTextButton->SetWidth(60);
	pTextButton->SetClickCallback(USE_MEMBERCALLBACK(OnClickLogin));

	/*
	//add the forgot login button
	pTextButton = AddControl<CUIControl_TextButton>();
	pTextButton->SetPos(200,370);
	pTextButton->SetText("Forgot Login");
	pTextButton->SetWidth(100);
	pTextButton->SetClickCallback(USE_MEMBERCALLBACK(OnClickForgotLogin));
	*/
}

CUISheet_Dots2dTitle::~CUISheet_Dots2dTitle()
{
	TextureManager.FreeTexture(m_pTitleImage);
}

void CUISheet_Dots2dTitle::Render(int nOffsetX, int nOffsetY, CUIStack *pUIStack)
{
	float fTitleMinX = pUIStack->GetScreenWidth() * 0.2f;
	float fTitleWidth = pUIStack->GetScreenWidth() * 0.6f;
	float fTitleMinY = pUIStack->GetScreenHeight() * 0.1f;
	float fTitleHeight = fTitleWidth * 0.5f;

	pUIStack->RenderTextureAlphaBlend((int)fTitleMinX,(int)fTitleMinY,(int)fTitleWidth,(int)fTitleHeight,m_pTitleImage,Vec3(1.0f,1.0f,1.0f));

	CUISheet::Render(nOffsetX, nOffsetY, pUIStack);
}

void CUISheet_Dots2dTitle::Update(MRTFLOAT fFrameTime)
{
	CUISheet::Update(fFrameTime);

	m_fCurrentTime += fFrameTime;
}

void CUISheet_Dots2dTitle::OnClickNewAccount()
{
	//add the new account dialog to the UI stack
	CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CNewAccountDialog>();
}

void CUISheet_Dots2dTitle::OnLoginCancel()
{
	//cancel the login request
	WebService::GetSingleton().CancelLogin();
}

void CUISheet_Dots2dTitle::OnLoginRequestComplete(WebService::EWebServiceResponse eFinishCode)
{
	switch(eFinishCode)
	{
		case WebService::kError:
		{
			CRayTracer::GetSingleton().GetUIStack().MessageBox("An unknown error occured, please try again later.");
			break;
		}
		case WebService::kInvalidLogin:
		{
			CRayTracer::GetSingleton().GetUIStack().MessageBox("Invalid Username or Password.");
			break;
		}
		case WebService::kCancel:
		{
			//do nothing
			break;
		}
		case WebService::kOK:
		{
			//go to the lobby
			Close();
			CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_Dots2dLobby>();
			break;
		}
		case WebService::kPasswordMismatch:
		case WebService::kAlreadyExists:
		{
			break;
		}
	}
}

void CUISheet_Dots2dTitle::OnClickLogin()
{
	//make a login request
	WebService::GetSingleton().Login(m_pUserName->GetText(),m_pPassword->GetText(),USE_MEMBERCALLBACK(OnLoginRequestComplete),"Logging in");
}

void CUISheet_Dots2dTitle::OnClickForgotLogin()
{
	/*
	//make a login request
	WebService::GetSingleton().Login(m_pUserName->GetText(),m_pPassword->GetText(),USE_MEMBERCALLBACK(OnLoginRequestComplete));
	*/
}

bool CUISheet_Dots2dTitle::OnKeyTyped(unsigned char nKey)
{
	if(nKey == '\r')
	{
		OnClickLogin();
		return true;
	}
	else if(CUISheet::OnKeyTyped(nKey))
		return true;
	else
		return false;
}