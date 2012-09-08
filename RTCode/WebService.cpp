/*======================================================

WebService.cpp

A singleton to help interface with the web service

======================================================*/

#include "WebService.h"
#include "RayTracer.h"
#include "PlatformBase.h"
#include "../RTCode/StringBuffer.h"
#include "CommonUI/UISheet_Loading.h"
#include "CommonUI/UIControl_TextButton.h"
#include <string.h>

#define WEBSERVICEBASE "http://services.moaicloud.com/atrix256/naclgames/?r=1"
//#define WEBSERVICEBASE "http://moai.demofox.org/?webservicename=naclgames&account=atrix256"

//response parsing helper functions
namespace
{
	const char *SkipWhitespace(const char *text)
	{
		while(text[0] && (text[0] == ' '||text[0] == '\r'||text[0] == '\n'))
			text++;

		return text;
	}
}

WebService::WebService()
{
	m_fTimeSinceMatchPing  = 0.0f;
	m_bMatchPingInFlight = false;
}

bool WebService::EnterMatch(const char *szMatchId)
{
	if(IsInMatch() || !szMatchId || !szMatchId[0])
		return false;

	m_matchid = szMatchId;
	m_fTimeSinceMatchPing = WEBSERVICE_MATCHPINGTIME;
	m_bMatchPingInFlight = false;
	return true;
}

void WebService::LeaveMatch()
{
	m_matchid = "";
}

WebService &WebService::GetSingleton()
{
	static WebService webService;
	return webService;
}

void WebService::Logout()
{
	//make sure all web requests are cancelled
	CRayTracer::GetSingleton().GetPlatform().CancelHTTPRequestsOfType(CPlatformBase::kAll);

	m_userid = "";
	m_username = "";
	m_matchid = "";
}

void WebService::Update(float fElapsedSeconds)
{
	//if we are logged in and in a match
	if(IsLoggedIn() && IsInMatch())
	{
		//periodically do a MatchPing
		m_fTimeSinceMatchPing += fElapsedSeconds;
		if(m_fTimeSinceMatchPing > WEBSERVICE_MATCHPINGTIME && !m_bMatchPingInFlight)
		{
			m_bMatchPingInFlight = true;
			MatchPing(NULL,NULL);
		}
	}
}

void WebService::MatchPing(WebServiceCallback pCreateAccountCallback, void *pUserData, const char *szLoadingText /*= NULL*/)
{
	//make sure no match pings are already in progress
	CancelMatchPing();
	
	StringBuffer<1024> szURL;
	szURL.Format("%s&func=matchping&game=%s&userid=%s&matchid=%s",WEBSERVICEBASE,m_gamename.c_str(),m_userid.c_str(),m_matchid.c_str());
	CRayTracer::GetSingleton().GetPlatform().MakeHTTPRequest(szURL.Str(),USE_SINGLETONCALLBACK(OnMatchPingHTTPDone),new SCallbackData(pCreateAccountCallback,pUserData,szLoadingText,StaticCB_CancelMatchPing,NULL),CPlatformBase::kMatchPing);
}

void WebService::OnMatchPingHTTPDone(void *pUserData, ERequestFinishCode eFinishCode, const char *szResponse)
{
	//if the request was canceled, ignore the response
	if(eFinishCode == kHttpRequestCancel)
	{
		return;
	}

	//handle response code

	//handle new messages from server.  put messages into queue and have game process the queue?

	//remeber that we don't have a match ping in flight anymore
	m_bMatchPingInFlight = false;
	m_fTimeSinceMatchPing = 0.0;
}

void WebService::CreateMatch(const char *szMatchName, int nNumPlayers, bool bPrivateMatch, WebServiceCallback pCreateAccountCallback, void *pUserData, const char *szLoadingText /*= NULL*/)
{
	StringBuffer<1024> szURL;
	szURL.Format("%s&func=creatematch&game=%s&matchname=%s&userid=%s&private=%s&numplayers=%i",WEBSERVICEBASE,m_gamename.c_str(),szMatchName,m_userid.c_str(),bPrivateMatch?"true":"false",nNumPlayers);
	CRayTracer::GetSingleton().GetPlatform().MakeHTTPRequest(szURL.Str(),USE_SINGLETONCALLBACK(OnCreateMatchHTTPDone),new SCallbackData(pCreateAccountCallback,pUserData,szLoadingText,StaticCB_CancelCreateMatch,NULL),CPlatformBase::kCreateMatch);
}

void WebService::OnCreateMatchHTTPDone(void *pUserData, ERequestFinishCode eFinishCode, const char *szResponse)
{
	//translate the http response into a web service response
	EWebServiceResponse eResponse = MapHttpErrorToWebServiceError(eFinishCode);

	//if everything is ok so far, try and parse the response
	if(eResponse == kOK)
	{
		if(strstr(szResponse,"exists") == szResponse)
		{
			eResponse = kAlreadyExists;
		}
		else if(strstr(szResponse,"ok") != szResponse)
		{
			eResponse = kError;
		}
		else
		{
			const char *szMatchId = SkipWhitespace(&szResponse[2]);
			if(!EnterMatch(szMatchId))
			{
				eResponse = kError;
			}
		}
	}

	HandleCallback(pUserData,eResponse);
}

void WebService::CreateAccount(const char *szUsername, const char *szPassword1, const char *szPassword2, const char *szEmail, WebServiceCallback pCreateAccountCallback, void *pUserData, const char *szLoadingText /*= NULL*/)
{
	//if the passwords don't match, bail out here, calling the callback
	if(strcmp(szPassword1,szPassword2))
	{
		if(pCreateAccountCallback)
		{
			pCreateAccountCallback(pUserData,kPasswordMismatch);
		}
		return;
	}

	StringBuffer<1024> szURL;
	szURL.Format("%s&func=createaccount&username=%s&password=%s&email=%s",WEBSERVICEBASE,szUsername,szPassword1,szEmail);
	CRayTracer::GetSingleton().GetPlatform().MakeHTTPRequest(szURL.Str(),USE_SINGLETONCALLBACK(OnCreateAccountHTTPDone),new SCallbackData(pCreateAccountCallback,pUserData,szLoadingText,StaticCB_CancelCreateAccount,NULL),CPlatformBase::kCreateAccount);
}

void WebService::OnCreateAccountHTTPDone(void *pUserData, ERequestFinishCode eFinishCode, const char *szResponse)
{
	//translate the http response into a web service response
	EWebServiceResponse eResponse = MapHttpErrorToWebServiceError(eFinishCode);

	//if everything is ok so far, try and parse the response
	if(eResponse == kOK)
	{
		if(strstr(szResponse,"exists") == szResponse)
		{
			eResponse = kAlreadyExists;
		}
		else if(strstr(szResponse,"ok") != szResponse)
		{
			eResponse = kError;
		}
	}

	HandleCallback(pUserData,eResponse);
}

void WebService::Login(const char *szUsername, const char *szPassword, WebServiceCallback pLoginCallback, void *pUserData, const char *szLoadingText /*= NULL*/)
{
	//store the username
	m_username = szUsername;

	//make an http request to try and login
	StringBuffer<1024> szURL;
	szURL.Format("%s&func=login&username=%s&password=%s",WEBSERVICEBASE,szUsername,szPassword);
	CRayTracer::GetSingleton().GetPlatform().MakeHTTPRequest(szURL.Str(),USE_SINGLETONCALLBACK(OnLoginHTTPDone),new SCallbackData(pLoginCallback,pUserData,szLoadingText,StaticCB_CancelLogin,NULL),CPlatformBase::kLogin);
}

void WebService::OnLoginHTTPDone(void *pUserData, ERequestFinishCode eFinishCode, const char *szResponse)
{
	//translate the http response into a web service response
	EWebServiceResponse eResponse = MapHttpErrorToWebServiceError(eFinishCode);

	//if everything is ok so far, try and parse the response
	if(eResponse == kOK)
	{
		if(strstr(szResponse,"invalid")==szResponse)
		{
			eResponse = kInvalidLogin;
		}
		else if(strstr(szResponse,"ok")==szResponse)
		{
			const char *szUserId = SkipWhitespace(&szResponse[2]);
			if(szUserId[0])
			{
				m_userid = szUserId;
			}
			else
			{
				eResponse = kError;
			}
		}
		else
		{
			eResponse = kError;
		}
	}

	HandleCallback(pUserData,eResponse);
}

WebService::EWebServiceResponse WebService::MapHttpErrorToWebServiceError(ERequestFinishCode eCode)
{
	//translate the http response into a web service response
	EWebServiceResponse eResponse = kError;

	switch(eCode)
	{
		case kHttpRequestOK:
		{
			eResponse = kOK;
			break;
		}
		case kHttpRequestCommsError:
		case kHttpRequestServerError:
		case kHttpRequestUnknownResponse:
		{
			eResponse = kError;
			break;
		}
		case kHttpRequestCancel:
		{
			eResponse = kCancel;
			break;
		}
	}

	return eResponse;
}

void WebService::HandleCallback(void *pData, EWebServiceResponse eResponse)
{
	//get our callback data
	SCallbackData *pCBData = (SCallbackData *)pData;

	//close the loading dialog if there is one
	if(pCBData->m_pLoadingDialog)
	{
		pCBData->m_pLoadingDialog->Close();
	}

	//call the callback if there is one
	if(pCBData->m_pCallback)
	{
		pCBData->m_pCallback(pCBData->m_pUserData,eResponse);
	}

	//free the callback data
	delete pCBData;
}

WebService::SCallbackData::SCallbackData(WebServiceCallback pCallback, void *pUserData, const char *szLoadingText, ButtonClickCallback pCancelCallback, void *pCancelCallbackUserData)
:m_pCallback(pCallback)
,m_pUserData(pUserData)
,m_pLoadingDialog(NULL)
{
	if(szLoadingText)
	{
		m_pLoadingDialog = CRayTracer::GetSingleton().GetUIStack().AddUISheetToTopOfStack<CUISheet_Loading>();
		m_pLoadingDialog->SetCancelCallback(pCancelCallback,pCancelCallbackUserData);
		m_pLoadingDialog->SetMessage(szLoadingText);
	}
}