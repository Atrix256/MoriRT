/*======================================================

PlatformDirectX.h

DirectX platform stuff

======================================================*/

#ifndef PLATFORMDIRECTX_H
#define PLATFORMDIRECTX_H

#define FORCE_SINGLE_THREADED 0

#include "..\RTCode\PlatformBase.h"
#include <d3dx9.h>
#include <vector>
#include "LibcurlHTTP.h"

struct SThreadSafeUINT
{
public:
	SThreadSafeUINT(unsigned int nInitialValue)
	:m_nValue(nInitialValue)
	{
		InitializeCriticalSection( &cs );
	}

	unsigned int GetValue()
	{
		EnterCriticalSection( &cs );
		unsigned int nRet = m_nValue;
		LeaveCriticalSection( &cs );

		return nRet;
	}

	void SetValue(unsigned int nNewValue)
	{
		EnterCriticalSection( &cs );
		m_nValue = nNewValue;
		LeaveCriticalSection( &cs );
	}

	bool GetNextValueDownIfgt0(unsigned int &nNextValue)
	{
		bool bRet = true;
		EnterCriticalSection( &cs );
		if(m_nValue > 0)
		{
			m_nValue--;
			nNextValue = m_nValue;
		}
		else
		{
			bRet = false;
		}
		LeaveCriticalSection( &cs );

		return bRet;
	}

private:
	volatile unsigned int m_nValue;
	CRITICAL_SECTION cs;
};

struct SThreadParameters
{
public:
	int m_nThreadIndex;
	int m_nTotalThreads;
	class CPlatformDirectX *m_pPlatform;

	HANDLE m_hWakeUpEvent;
	HANDLE m_hBackToSleepEvent;
};

//typedefs
typedef std::vector<HANDLE> HandleList;
typedef std::vector<SThreadParameters> ThreadParametersList;
typedef std::vector<CRITICAL_SECTION> CriticalSectionList;

class CPlatformDirectX : public CPlatformBase
{
public:
	CPlatformDirectX();
	virtual ~CPlatformDirectX();

	virtual void GetMousePosition(int &nMouseX, int &nMouseY) {nMouseX = m_nMouseX; nMouseY = m_nMouseY;}
	void SetMousePosition(int nMouseX, int nMouseY) {m_nMouseX = nMouseX; m_nMouseY = nMouseY;}

	virtual void LogText(const char *szText);

	void Init(LPDIRECT3DDEVICE9 pD3DDevice, HWND hWnd, D3DPRESENT_PARAMETERS &d3dpp,int *pWindowMarginX,int *pWindowMarginY);

	void FreeVolatileResources();
	void InitVolatileResources();

	virtual void Render(CCamera *pCamera, int nScreenWidth, int nScreenHeight, int nMaxRayBounce);

	virtual int CriticalSectionAdd();
	virtual void CriticalSectionEnter(int nCriticalSectionIndex);
	virtual void CriticalSectionLeave(int nCriticalSectionIndex);

	virtual void ExitApplication();

	virtual int GetNumberRenderThreads() {return m_nNumProcessors;}
	virtual int GetNumberActiveRenderThreads() {return m_nNumActiveProcessors;}
	virtual void SetNumberActiveRenderThreads(int nNum) {m_nNumActiveProcessors = nNum;}

	virtual void StartRecordingVideo();
	virtual void StopRecordingVideo();
	virtual bool IsRecordingVideo() {return m_bRecording;}
	virtual void PossiblyOverrideFrameTime(MRTFLOAT &fFrameTime);
	virtual void PostRender();

	virtual void SetResolution(int nWidth, int nHeight);

	virtual void MakeHTTPRequest(const char *szURL,HTTPCallback callback,void *pUserData,EHttpRequestType eRequestType = kGeneral);
	virtual void CancelHTTPRequestsOfType(EHttpRequestType eRequestType);

	virtual void Update();

private:

	void DoScreenCapGDI();
	void WriteBMP(const char *name, int W, int H, int Bpp, int *data);
	void FinalizeVideo();
	int m_nCurrentRecordingFrame;
	int m_nGdiScreenCapBufferSize;
	unsigned char *m_pGdiScreenCapBuffer;
	bool m_bRecording;

	DWORD CountSetBits(ULONG_PTR bitMask);
	int GetNumLogicalProcessors();

	HWND m_hWnd;
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	int m_nMouseX;
	int m_nMouseY;

	//multithreaded rendering support
	static void RenderThreadFunc(void *pParams);
	HandleList ThreadHandles;
	ThreadParametersList ThreadParametersList;
	int m_nNumProcessors;
	int m_nNumActiveProcessors;
	CCamera *m_pCurrentCamera;
	HandleList WakeUpEvents;
	HandleList BackToSleepEvents;
	SThreadSafeUINT m_nNextCellToRender;

	//critical section support
	CriticalSectionList m_CriticalSections;

	D3DPRESENT_PARAMETERS m_d3dpp;

	int *m_pWindowMarginX;
	int *m_pWindowMarginY;

	//http interface
	CLibCurlHttp m_LibCurlHttp;
};

#endif //PLATFORMDIRECTX_H