/*======================================================

LibcurlHTTP.h

HTTP request queue using libcurl

======================================================*/

#ifndef LIBCURLHTTP_H
#define LIBCURLHTTP_H

#include "../RTCode/PlatformBase.h"
#include <string>
#include <vector>

//forward declarations
typedef void CURLM;
typedef void CURL;
struct SHttpRequest;

//typedefs
typedef std::vector<SHttpRequest *> HttpRequestList;

//an HTTP request
struct SHttpRequest
{
public:
	SHttpRequest();
	~SHttpRequest();
	bool Init(CURLM *pMultiHandle,const char *szURL,HTTPCallback callback,void *pUserData,int nRequestType);
	CURL *GetEasyHandle() const {return m_pCurlHandle;}
	void OnRequestDone(CURLM *pMultiHandle, ERequestFinishCode eFinishCode);
	int GetRequestType() const {return m_nRequestType;}

private:
	size_t OnDataReceived( char *ptr, size_t size, size_t nmemb);
	static size_t Static_OnDataReceived( char *ptr, size_t size, size_t nmemb, void *userdata);

	CURL *m_pCurlHandle;
	std::string m_URL;
	HTTPCallback m_pCallback;
	void *m_pUserData;

	size_t m_nResponseDataSize;
	char *m_pResponseData;

	int m_nRequestType;
};

class CLibCurlHttp
{
public:
	CLibCurlHttp();
	~CLibCurlHttp();

	void CancelRequestsOfType(int nRequestType);
	void Request(const char *szURL,HTTPCallback callback,void *pUserData, int nRequestType);
	void Update(void);

private:
	void OnRequestDone(CURL *pEasyHandle);

	CURLM *m_pMultiHandle;

	HttpRequestList m_ActiveRequests;
};

#endif