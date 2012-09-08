/*======================================================

PlatformNaCl.cpp

NaCl platform stuff

======================================================*/

#include "platformnacl.h"

#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/point.h"
#include <stdio.h>
#include <string.h>
#include "NaClHTTP.h"

#include "../RTCode/RayTracer.h"
#include "../RTCode/Camera.h"

CPlatformNaCl::CPlatformNaCl()
:m_nNextCellToRender(0)
{
	m_pCurrentCamera = 0;
	m_pInstance = 0;
	m_nMouseX = 0;
	m_nMouseY = 0;

	//get the number of logical processors on this system
	m_nNumProcessors = sysconf(_SC_NPROCESSORS_ONLN);
	m_nNumActiveProcessors = m_nNumProcessors;

	#if FORCE_SINGLE_THREADED
		m_nNumActiveProcessors = 1;
	#endif

	sem_init(&m_BackToSleepSemaphore, 0, 0);
	pthread_mutex_init(&m_WakeUpMutex,NULL);
	pthread_cond_init(&m_WakeUpEvent,NULL);

	//make our list of thread params
	m_ThreadParameterList.resize(m_nNumProcessors);

	//render thread stuff
	for(unsigned int nIndex = 0; nIndex < m_ThreadParameterList.size(); ++nIndex)
	{
		SThreadParameters &ThreadParams = m_ThreadParameterList[nIndex];
		ThreadParams.m_nThreadIndex = nIndex;
		ThreadParams.m_nTotalThreads = m_nNumProcessors;
		ThreadParams.m_pPlatform = this;
		ThreadParams.m_pBackToSleepSemaphore = &m_BackToSleepSemaphore;
		ThreadParams.m_pWakeUpMutex = &m_WakeUpMutex;
		ThreadParams.m_pWakeUpEvent = &m_WakeUpEvent;
		pthread_create( &ThreadParams.m_RenderThread, NULL, &RenderThreadFunc, &ThreadParams);
	}
}

CPlatformNaCl::~CPlatformNaCl()
{
	
}

void CPlatformNaCl::Init(pp::Instance* pInstance)
{
	m_pInstance = pInstance;
}

void CPlatformNaCl::LogText(const char *szText)
{
	if(m_pInstance)
	{
		char buffer[1024];
		sprintf(buffer,"ERROR: %s",szText);
		m_pInstance->PostMessage(pp::Var(buffer));
	}
}

void CPlatformNaCl::GetMousePosition(int &nMouseX, int &nMouseY)
{
	nMouseX = m_nMouseX;
	nMouseY = m_nMouseY;
}

void CPlatformNaCl::SetMousePosition(int &nMouseX, int &nMouseY)
{
	m_nMouseX = nMouseX;
	m_nMouseY = nMouseY;
}

void *CPlatformNaCl::RenderThreadFunc(void *pParams)
{
	SThreadParameters *pThreadParameters = (SThreadParameters *)pParams;
	CRayTracer &RayTracer = CRayTracer::GetSingleton();
	CPlatformNaCl *pPlatform = pThreadParameters->m_pPlatform;

	while(1)
	{
		//wait to be woken up by the main thread
		pthread_mutex_lock( pThreadParameters->m_pWakeUpMutex );
		pthread_cond_wait( pThreadParameters->m_pWakeUpEvent, pThreadParameters->m_pWakeUpMutex );
		pthread_mutex_unlock( pThreadParameters->m_pWakeUpMutex );

		//only contribute to rendering if we should
		if(pThreadParameters->m_nThreadIndex < pPlatform->GetNumberActiveRenderThreads())
		{
			//render all the cells we should render
			int nRayBounceCount = RayTracer.GetRayBounceCount();
			unsigned int nNextCellToRender;
			while(pPlatform->m_nNextCellToRender.GetNextValueDownIfgt0(nNextCellToRender))
			{
				pThreadParameters->m_pPlatform->m_pCurrentCamera->RenderCell(nNextCellToRender,nRayBounceCount,CCamera::kRenderNormal);
			}
		}

		//tell the main thread that we are done
		sem_post(pThreadParameters->m_pBackToSleepSemaphore);
	}

	return 0;
}

void CPlatformNaCl::Render(CCamera *pCamera, int nScreenWidth, int nScreenHeight, int nMaxRayBounce)
{
	//uncomment below for single threaded rendering
	//CPlatformBase::Render(pPixelBuffer, pCamera, nScreenWidth, nScreenHeight, nMaxRayBounce);
	//return;

	//set the current camera;
	m_pCurrentCamera = pCamera;

	//set the # of the next cell to render
	m_nNextCellToRender.SetValue(m_pCurrentCamera->GetNumRenderCells());

	#if FORCE_SINGLE_THREADED
		//render all the cells we should render
		int nStopCellIndex = m_pCurrentCamera->GetNumRenderCells();
		int nRayBounceCount = CRayTracer::GetSingleton().GetRayBounceCount();

		unsigned int nNextCellToRender;
		while(m_nNextCellToRender.GetNextValueDownIfgt0(nNextCellToRender))
		{
			m_pCurrentCamera->RenderCell(nNextCellToRender,nRayBounceCount,CCamera::kRenderNormal);
		}
		return;
	#endif

	//wake up all of our threads
	pthread_cond_broadcast( &m_WakeUpEvent );

	//wait for the threads to be done
	for(ThreadParameterList::iterator it = m_ThreadParameterList.begin(); it != m_ThreadParameterList.end(); ++it)
	{
		sem_wait(&m_BackToSleepSemaphore);
	}
}

void CPlatformNaCl::SetResolution(int nWidth, int nHeight)
{
	CRayTracer::GetSingleton().SetResolution(nWidth,nHeight);
	
	static_cast<Pong *>(m_pInstance)->ClearBuffer();

	//char buffer[1024];
	//sprintf(buffer,"SIZE: %i %i",nWidth,nHeight);
	//m_pInstance->PostMessage(pp::Var(buffer));
}

void CPlatformNaCl::MakeHTTPRequest(const char *szURL,HTTPCallback callback,void *pUserData,EHttpRequestType eRequestType /*= kGeneral*/)
{
	if(strstr(szURL,"http://") == szURL)
		szURL = &szURL[7];
	m_Http.Request(m_pInstance,szURL,callback,pUserData,eRequestType);
}

void CPlatformNaCl::CancelHTTPRequestsOfType(EHttpRequestType eRequestType)
{
	m_Http.CancelRequestsOfType(eRequestType);
}
