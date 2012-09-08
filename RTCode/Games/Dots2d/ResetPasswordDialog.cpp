/*======================================================

ResetPasswordDialog.cpp

The logic for the reset password dialog

======================================================*/

#include "ResetPasswordDialog.h"
#include "../../CommonUI/UIStack.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../CommonUI/TextBox.h"
#include "../../CommonUI/StaticText.h"
#include "../../RayTracer.h"
#include "../../WebService.h"

CResetPasswordDialog::CResetPasswordDialog()
{
	//mark this dialog as modal (don't let events down through this dialog)
	SetModal(true);

	m_nWidth = 400;
	m_nHeight = 295;
	m_nPosX = 50;
	m_nPosY = 100;

	//add the ok and cancel button
	CUIControl_TextButton *pTextButton = AddControl<CUIControl_TextButton>();
	pTextButton->SetText("OK");
	pTextButton->SetWidth(60);
	pTextButton->SetPos(m_nWidth - 70, m_nHeight - 30);
	pTextButton->SetClickCallback(USE_MEMBERCALLBACK(OnOkClicked));

	pTextButton = AddControl<CUIControl_TextButton>();
	pTextButton->SetText("Cancel");
	pTextButton->SetWidth(60);
	pTextButton->SetPos(m_nWidth - 140, m_nHeight - 30);
	pTextButton->SetClickCallback(USE_MEMBERCALLBACK(OnCancelClicked));

	//add the email box and label
	CStaticText *pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(130,145);
	Vec3 vBlack(0.0f,0.0f,0.0f);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentRight);
	pStaticText->SetText("Email (Optional):");
	CUIControl_TextBox *pTextBox = AddControl<CUIControl_TextBox>();
	pTextBox->SetPos(140,140);
	pTextBox->SetWidth(235);
	m_pEmail = pTextBox;

	//add the "Create New Account" header text
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(10,10);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentLeft);
	pStaticText->SetText("Create New Account");

	//add the info about the email usage
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(10,175);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentLeft);
	pStaticText->SetText("Note: Entering your email address is optional, but without it,");
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(10,195);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentLeft);
	pStaticText->SetText("you won't be able to reset your password if you ever need to.");
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(10,215);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentLeft);
	pStaticText->SetText("Your email address will not be used for anything else - we will");
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(10,235);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentLeft);
	pStaticText->SetText("never send you spam or sell your address to others!");

}

void CResetPasswordDialog::OnCancelClicked()
{
	Close();
}

void CResetPasswordDialog::OnOkClicked()
{
	//make a request to create the account;
	//WebService::GetSingleton().CreateAccount(m_pUsername->GetText(),m_pPassword1->GetText(),m_pPassword2->GetText(),m_pEmail->GetText(),USE_MEMBERCALLBACK(OnCreateAccountRequestComplete),"Sending reset password email");
}

void CResetPasswordDialog::OnCreateAccountCancel()
{
	//cancel the login request
	WebService::GetSingleton().CancelCreateAccount();
}

void CResetPasswordDialog::OnCreateAccountRequestComplete(WebService::EWebServiceResponse eFinishCode)
{
	switch(eFinishCode)
	{
		case WebService::kError:
		{
			CRayTracer::GetSingleton().GetUIStack().MessageBox("An unknown error occured, please try again later.");
			break;
		}
		case WebService::kAlreadyExists:
		{
			CRayTracer::GetSingleton().GetUIStack().MessageBox("That username is already taken, please try again.");
			break;
		}
		case WebService::kPasswordMismatch:
		{
			CRayTracer::GetSingleton().GetUIStack().MessageBox("Your passwords do not match, please try again.");
			break;
		}
		case WebService::kCancel:
		{
			break;
		}
		case WebService::kOK:
		{
			Close();
			CRayTracer::GetSingleton().GetUIStack().MessageBox("Account Created!");
			break;
		}
		case WebService::kInvalidLogin:
		{
			break;
		}
	}
}

bool CResetPasswordDialog::OnKeyTyped(unsigned char nKey)
{
	if(CUISheet::OnKeyTyped(nKey))
		return true;

	//if enter is pressed
	if(nKey == '\n')
	{
		//call "ok" callback function here
		return true;
	}
	//if escape is pressed
	else if(nKey == 27)
	{
		//call "cancel" callback function here
		return true;
	}
	else
	{
		return false;
	}
}