/*======================================================

UISheet_Loading.cpp

The functionality for a generic "loading" / "please wait" screen

======================================================*/

#include "UISheet_Loading.h"
#include "StaticText.h"
#include "UIControl_TextButton.h"
#include <math.h>
#include <string.h>

CUISheet_Loading::CUISheet_Loading()
:m_fCurrentTime(0)
{
	//mark this dialog as modal (don't let events down through this dialog)
	SetModal(true);

	//set size and position
	m_nWidth = 300;
	m_nHeight = 160;
	m_nPosX = 100;
	m_nPosY = 190;

	//add our title text
	CStaticText *pStaticText = AddControl<CStaticText>();
	pStaticText->SetPos(m_nWidth/2,40);
	Vec3 vBlack(0.0f,0.0f,0.0f);
	pStaticText->SetColor(vBlack);
	pStaticText->SetAlignment(CUIStack::kAlignmentCentered);
	pStaticText->SetText("");
	m_pMessageText = pStaticText;

	//add our animated loading text
	m_pAnimatedText = AddControl<CStaticText>();
	m_pAnimatedText->SetPos(110,80);
	m_pAnimatedText->SetColor(vBlack);
	m_pAnimatedText->SetAlignment(CUIStack::kAlignmentLeft);
	m_pAnimatedText->SetText("Please wait.");

	//add our cancel button
	m_pCancelButton = AddControl<CUIControl_TextButton>();
	m_pCancelButton->SetWidth(100);
	m_pCancelButton->SetText("Cancel");
	m_pCancelButton->SetPos(100,120);
}

void CUISheet_Loading::SetMessage(const char *szMessage)
{
	m_pMessageText->SetText(szMessage);
}

void CUISheet_Loading::Update(MRTFLOAT fFrameTime)
{
	CUISheet::Update(fFrameTime);

	m_fCurrentTime = fmodf(m_fCurrentTime + fFrameTime,1.0f);

	int nNumDots = (int)(m_fCurrentTime * 4.0f);
	char Buffer[32];
	strcpy(Buffer,"Please wait.");
	while(nNumDots)
	{
		strcat(Buffer,".");
		nNumDots--;
	}

	m_pAnimatedText->SetText(Buffer);
}

void CUISheet_Loading::SetCancelCallback(ButtonClickCallback pCallback, void *pUserData)
{
	m_pCancelButton->SetClickCallback(pCallback,pUserData);
}