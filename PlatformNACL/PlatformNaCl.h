/*======================================================

PlatformNaCl.h

NaCl platform stuff

======================================================*/

#ifndef PLATFORMNACL_H
#define PLATFORMNACL_H

#define FORCE_SINGLE_THREADED 0

#include "../RTCode/PlatformBase.h"
#include "pong.h"
#include "PosixEvent.h"
#include <vector>
#include <semaphore.h>
#include "NaClHTTP.h"

//forward declarations
class CCamera;

//typedefs
typedef std::vector<struct SThreadParameters> ThreadParameterList;

struct SThreadSafeUINT
{
public:
	SThreadSafeUINT(unsigned int nInitialValue)
	{
		m_nValue = nInitialValue;
		pthread_mutex_init(&m_cs_mutex,NULL);
	}

	unsigned int GetValue()
	{
		pthread_mutex_lock( &m_cs_mutex );
		unsigned int nRet = m_nValue;
		pthread_mutex_unlock( &m_cs_mutex );

		return nRet;
	}

	void SetValue(unsigned int nNewValue)
	{
		pthread_mutex_lock( &m_cs_mutex );
		m_nValue = nNewValue;
		pthread_mutex_unlock( &m_cs_mutex );
	}

	bool GetNextValueDownIfgt0(unsigned int &nNextValue)
	{
		bool bRet = true;
		pthread_mutex_lock( &m_cs_mutex );
		if(m_nValue > 0)
		{
			m_nValue--;
			nNextValue = m_nValue;
		}
		else
		{
			bRet = false;
		}
		pthread_mutex_unlock( &m_cs_mutex );

		return bRet;
	}

private:
	volatile unsigned int m_nValue;
	pthread_mutex_t m_cs_mutex;
};

struct SThreadParameters
{
public:
	int m_nThreadIndex;
	int m_nTotalThreads;
	class CPlatformNaCl *m_pPlatform;

	pthread_t m_RenderThread;

	pthread_mutex_t *m_pWakeUpMutex; 
	pthread_cond_t *m_pWakeUpEvent;
	sem_t *m_pBackToSleepSemaphore;
};

class CPlatformNaCl : public CPlatformBase
{
public:
	CPlatformNaCl();
	~CPlatformNaCl();

	void Init(pp::Instance* pInstance);

	virtual void LogText(const char *szText);

	virtual void GetMousePosition(int &nMouseX, int &nMouseY);

	void SetMousePosition(int &nMouseX, int &nMouseY);

	virtual int GetNumberRenderThreads() {return m_nNumProcessors;}
	virtual int GetNumberActiveRenderThreads() {return m_nNumActiveProcessors;}
	virtual void SetNumberActiveRenderThreads(int nNum) {m_nNumActiveProcessors = nNum;}

	virtual void Render(CCamera *pCamera, int nScreenWidth, int nScreenHeight, int nMaxRayBounce);

	virtual void SetResolution(int nWidth, int nHeight);

	virtual void MakeHTTPRequest(const char *szURL,HTTPCallback callback,void *pUserData,EHttpRequestType eRequestType = kGeneral);
	virtual void CancelHTTPRequestsOfType(EHttpRequestType eRequestType);

private:
	pp::Instance* m_pInstance;

	int m_nMouseX;
	int m_nMouseY;

	int m_nNumProcessors;
	volatile int m_nNumActiveProcessors;

	SThreadSafeUINT m_nNextCellToRender;

	CCamera *m_pCurrentCamera;

	//render thread stuff
	static void *RenderThreadFunc(void *pParams);
	ThreadParameterList m_ThreadParameterList;

	pthread_mutex_t m_WakeUpMutex; 
	pthread_cond_t m_WakeUpEvent;

	sem_t m_BackToSleepSemaphore;

	//http interface
	CNaClHttp m_Http;
};

#endif //PLATFORMNACL_H
