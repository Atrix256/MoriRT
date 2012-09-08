/*======================================================

NewAccountDialog.cpp

The logic for the new account dialog

======================================================*/

#include "NewAccountDialog.h"
#include "../../CommonUI/UIStack.h"
#include "../../CommonUI/UIControl_TextButton.h"
#include "../../CommonUI/TextBox.h"
#include "../../CommonUI/StaticText.h"
#include "../../RayTracer.h"
#include "../../WebService.h"

CNewAccountDialog::CNewAccountDialog()
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

	//add the username box and label and set the dialog's focus to the username text box
	CStaticText *pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(130,55);
	Vec3 vBlack(0.0f,0.0f,0.0f);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentRight);
	pStaticText->SetText("Username:");
	CUIControl_TextBox *pTextBox = AddControl<CUIControl_TextBox>();
	pTextBox->SetPos(140,50);
	pTextBox->SetWidth(235);
	SetFocus(pTextBox);
	m_pUsername = pTextBox;

	//add the password box and label
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(130,85);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentRight);
	pStaticText->SetText("Password:");
	pTextBox = AddControl<CUIControl_TextBox>();
	pTextBox->SetPos(140,80);
	pTextBox->SetWidth(235);
	pTextBox->SetPasswordMode(true);
	m_pPassword1 = pTextBox;

	//add the confirm password box and label
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(130,115);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentRight);
	pStaticText->SetText("Confirm password:");
	pTextBox = AddControl<CUIControl_TextBox>();
	pTextBox->SetPos(140,110);
	pTextBox->SetWidth(235);
	pTextBox->SetPasswordMode(true);
	m_pPassword2 = pTextBox;

	//add the email box and label
	pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(130,145);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentRight);
	pStaticText->SetText("Email (Optional):");
	pTextBox = AddControl<CUIControl_TextBox>();
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

void CNewAccountDialog::OnCancelClicked()
{
	Close();
}

void CNewAccountDialog::OnOkClicked()
{
	//make a request to create the account;
	WebService::GetSingleton().CreateAccount(m_pUsername->GetText(),m_pPassword1->GetText(),m_pPassword2->GetText(),m_pEmail->GetText(),USE_MEMBERCALLBACK(OnCreateAccountRequestComplete),"Creating Account");
}

void CNewAccountDialog::OnCreateAccountCancel()
{
	//cancel the login request
	WebService::GetSingleton().CancelCreateAccount();
}

void CNewAccountDialog::OnCreateAccountRequestComplete(WebService::EWebServiceResponse eFinishCode)
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

bool CNewAccountDialog::OnKeyTyped(unsigned char nKey)
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