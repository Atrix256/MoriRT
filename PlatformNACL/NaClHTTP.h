/*======================================================

NaClHTTP.h

HTTP request queue using NaCl http stuff

======================================================*/

#ifndef NACLHTTP_H
#define NACLHTTP_H

#include "../RTCode/PlatformBase.h"
#include <string>
#include <vector>
#include "ppapi/cpp/instance.h"

class GetURLHandler;
typedef void(*NaClHTTPCallback)(void *pUserData, GetURLHandler *pRequest, ERequestFinishCode eFinishCode, const char *szResponse);

class CNaClHttp
{
public:
	CNaClHttp() {}
	~CNaClHttp() {}

	void CancelRequestsOfType(int nRequestType);
	void Request(pp::Instance* instance, const char *szURL,HTTPCallback callback,void *pUserData, int nRequestType);

private:
	static void OnHttpRequestDone(void *pUserData, GetURLHandler *pRequest, ERequestFinishCode eFinishCode, const char *szResponse);
	void OnHttpRequestDone(GetURLHandler *pRequest, ERequestFinishCode eFinishCode, const char *szResponse);

	//void OnRequestDone(CURL *pEasyHandle);
	//CURLM *m_pMultiHandle;
	//HttpRequestList m_ActiveRequests;
};

#endif //NACLHTTP_H