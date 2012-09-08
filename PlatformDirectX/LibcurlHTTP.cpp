/*======================================================

LibcurlHTTP.cpp

HTTP request queue using libcurl

======================================================*/

#include "LibcurlHTTP.h"
#include "curl/curl.h"

CLibCurlHttp::CLibCurlHttp()
{
	m_pMultiHandle = curl_multi_init();
}

CLibCurlHttp::~CLibCurlHttp()
{
}

void CLibCurlHttp::Update(void)
{
	//give curl a time slice
	int nRunningHandles=-1;
	while(curl_multi_perform(m_pMultiHandle,&nRunningHandles) == CURLM_CALL_MULTI_PERFORM);

	//look at the msg queue to get info about handles that are finished
	CURLMsg *pMsg;
	int nNumMessages;
	while((pMsg = curl_multi_info_read(m_pMultiHandle,&nNumMessages)) != NULL)
	{
		if(pMsg->msg == CURLMSG_DONE)
		{
			OnRequestDone(pMsg->easy_handle);
		}
	}
}

void CLibCurlHttp::CancelRequestsOfType(int nRequestType)
{
	for(int nIndex = m_ActiveRequests.size() - 1; nIndex >= 0; --nIndex)
	{
		if(nRequestType == CPlatformBase::kAll || m_ActiveRequests[nIndex]->GetRequestType() == nRequestType)
		{
			m_ActiveRequests[nIndex]->OnRequestDone(m_pMultiHandle,kHttpRequestCancel);
			delete m_ActiveRequests[nIndex];
			m_ActiveRequests.erase(m_ActiveRequests.begin() + nIndex);
			return;
		}
	}
}

void CLibCurlHttp::Request(const char *szURL,HTTPCallback callback,void *pUserData,int nRequestType)
{
	SHttpRequest *pNewRequest = new SHttpRequest();
	
	//if we could initialize the request, add it to the queue
	if(pNewRequest->Init(m_pMultiHandle,szURL,callback,pUserData,nRequestType))
	{
		m_ActiveRequests.push_back(pNewRequest);
	}
	//else call the callback if there is one and free our request
	else
	{
		if(callback)
		{
			callback(pUserData,kHttpRequestCommsError,NULL);
		}

		delete pNewRequest;
	}
}

void CLibCurlHttp::OnRequestDone(CURL *pEasyHandle)
{
	for(HttpRequestList::iterator it = m_ActiveRequests.begin(), end = m_ActiveRequests.end(); it != end; ++it)
	{
		if((*it)->GetEasyHandle() == pEasyHandle)
		{
			(*it)->OnRequestDone(m_pMultiHandle,kHttpRequestOK);
			delete (*it);
			m_ActiveRequests.erase(it);
			return;
		}
	}
}

SHttpRequest::SHttpRequest()
:m_pCallback(NULL)
,m_pUserData(NULL)
,m_pResponseData(NULL)
,m_pCurlHandle(NULL)
,m_nResponseDataSize(0)
,m_nRequestType(0)
{
}

SHttpRequest::~SHttpRequest()
{
	if(m_pResponseData)
		free(m_pResponseData);
}

bool SHttpRequest::Init(CURLM *pMultiHandle,const char *szURL,HTTPCallback callback,void *pUserData,int nRequestType)
{
	m_nRequestType = nRequestType;
	m_URL = szURL;
	m_pCallback = callback;
	m_pUserData = pUserData;
	m_pCurlHandle = curl_easy_init();
	
	//set the url
	if(curl_easy_setopt(m_pCurlHandle, CURLOPT_URL, m_URL.c_str()) != CURLE_OK)
	{
		curl_easy_cleanup(m_pCurlHandle);
		return false;
	}

	//set the data received callback
	if(curl_easy_setopt(m_pCurlHandle, CURLOPT_WRITEFUNCTION, Static_OnDataReceived) != CURLE_OK)
	{
		curl_easy_cleanup(m_pCurlHandle);
		return false;
	}

	//set the data received callback user data
	if(curl_easy_setopt(m_pCurlHandle, CURLOPT_WRITEDATA, this) != CURLE_OK)
	{
		curl_easy_cleanup(m_pCurlHandle);
		return false;
	}

	//add it to the multi handle
	if(curl_multi_add_handle(pMultiHandle,m_pCurlHandle) != CURLM_OK)
	{
		curl_easy_cleanup(m_pCurlHandle);
		return false;
	}

	return true;
}

void SHttpRequest::OnRequestDone(CURLM *pMultiHandle, ERequestFinishCode eFinishCode)
{
	//call the callback if there is one
	if(m_pCallback)
	{
		//if the finish code is "OK" so far, check the http response code
		if(eFinishCode == kHttpRequestOK)
		{
			//see if the request was successful or not - based on the http response code
			//any 2xx http response code is success, all else is failure
			int nResponseCode = -1;
			if((curl_easy_getinfo(m_pCurlHandle,CURLINFO_RESPONSE_CODE,&nResponseCode) == CURLE_OK && (nResponseCode / 100 == 2)) == false)
				eFinishCode = kHttpRequestServerError;
		}

		//call the callback
		m_pCallback(m_pUserData,eFinishCode,m_pResponseData);
	}

	//clean up
	curl_multi_remove_handle(pMultiHandle,m_pCurlHandle);
	curl_easy_cleanup(m_pCurlHandle);
}

size_t SHttpRequest::OnDataReceived( char *ptr, size_t size, size_t nmemb)
{
	//make sure and reserve 1 byte for the null terminator at the end of the string
	if(!m_pResponseData)
	{
		m_nResponseDataSize = 1;
	}

	//calculate how big the new data is
	size_t nNewDataSize = size*nmemb;

	//make space for the new data if we can
	m_nResponseDataSize += size*nmemb;
	char *pNewMemory = (char *)realloc(m_pResponseData,m_nResponseDataSize);
	if(!pNewMemory)
		return CURL_READFUNC_ABORT;
	m_pResponseData = pNewMemory;

	//copy the new data into our response buffer
	memcpy(&m_pResponseData[m_nResponseDataSize - nNewDataSize - 1],ptr,nNewDataSize);

	//make sure the buffer is null terminated
	m_pResponseData[m_nResponseDataSize - 1] = 0;

	//return how much data we processed
	return nNewDataSize;
}

size_t SHttpRequest::Static_OnDataReceived( char *ptr, size_t size, size_t nmemb, void *userdata)
{
	return ((SHttpRequest *)userdata)->OnDataReceived(ptr,size,nmemb);
}
