/*======================================================

NaClHTTP.cpp

HTTP request queue using NaCl http stuff

======================================================*/

#include "NaClHTTP.h"
#include "geturl_handler.h"
#include "../RTCode/Raytracer.h"

void CNaClHttp::CancelRequestsOfType(int nRequestType)
{
}

void CNaClHttp::Request(pp::Instance* instance, const char *szURL,HTTPCallback callback,void *pUserData, int nRequestType)
{
	GetURLHandler *pNewRequest = GetURLHandler::Create(instance,szURL,OnHttpRequestDone,this);
	pNewRequest->Start();
}

void CNaClHttp::OnHttpRequestDone(void *pUserData, GetURLHandler *pRequest, ERequestFinishCode eFinishCode, const char *szResponse)
{
	((CNaClHttp*)pUserData)->OnHttpRequestDone(pRequest,eFinishCode,szResponse);
}

#include <string.h>
#include <stdio.h>

void CNaClHttp::OnHttpRequestDone(GetURLHandler *pRequest, ERequestFinishCode eFinishCode, const char *szResponse)
{
	if(eFinishCode == kHttpRequestOK)
	{
		if(szResponse && szResponse[0])
		{
			CRayTracer::GetSingleton().GetPlatform().LogText(szResponse);
			/*
			char buffer[256];
			memcpy(buffer,szResponse,50);
			buffer[50] = 0;
			
			//sprintf(buffer,"response first char = %i",szResponse[0]);
			CRayTracer::GetSingleton().GetPlatform().LogText(buffer);
			*/

			//CRayTracer::GetSingleton().GetPlatform().LogText(szResponse);
			/*
			CRayTracer::GetSingleton().GetPlatform().LogText("Non null response");
			char buffer[256];
			sprintf(buffer,"response len = %i",strlen(szResponse));
			CRayTracer::GetSingleton().GetPlatform().LogText(buffer);
			*/
		}
		else
		{
			CRayTracer::GetSingleton().GetPlatform().LogText("No error, but null response");
		}
	}
	else
	{
		CRayTracer::GetSingleton().GetPlatform().LogText("Error - waaaaa");
	}
}