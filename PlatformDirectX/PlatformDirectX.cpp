/*======================================================

PlatformDirectX.cpp

DirectX platform stuff

======================================================*/

#include "PlatformDirectX.h"
#include "..\RTCode\Camera.h"
#include "..\RTCode\RayTracer.h"
#include "..\RTCode\Texture.h"
#include <process.h>
#include "..\RTCode\CameraManager.h"

CPlatformDirectX::CPlatformDirectX()
:m_pCurrentCamera(0)
,m_nNextCellToRender(0)
,m_bRecording(false)
,m_hWnd(0)
,m_nGdiScreenCapBufferSize(0)
,m_pGdiScreenCapBuffer(0)
,m_pWindowMarginX(0)
,m_pWindowMarginY(0)
,m_nCurrentRecordingFrame(0)
,m_nMouseX(0)
,m_nMouseY(0)
{
	//get the number of logical processors on this system
	m_nNumProcessors = GetNumLogicalProcessors();
	m_nNumActiveProcessors = m_nNumProcessors;

	#if FORCE_SINGLE_THREADED
		m_nNumActiveProcessors = 1;
	#endif

	//create the thread parameters
	SThreadParameters ThreadParameters;
	ThreadParameters.m_nTotalThreads = m_nNumProcessors;
	ThreadParameters.m_pPlatform = this;
	for(int nIndex = 0; nIndex < m_nNumProcessors; ++nIndex)
	{
		ThreadParameters.m_hWakeUpEvent = CreateEvent(0,false,false,0);
		ThreadParameters.m_hBackToSleepEvent = CreateEvent(0,false,false,0);		
		ThreadParameters.m_nThreadIndex = nIndex;
		ThreadParametersList.push_back(ThreadParameters);

		WakeUpEvents.push_back(ThreadParameters.m_hWakeUpEvent);
		BackToSleepEvents.push_back(ThreadParameters.m_hBackToSleepEvent);
	}

	//start up a thread per processor
	for(int nIndex = 0; nIndex < m_nNumProcessors; ++nIndex)
	{
		DWORD nThreadID;
		ThreadHandles.push_back(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RenderThreadFunc,&ThreadParametersList[nIndex],0,&nThreadID));
	}
}

CPlatformDirectX::~CPlatformDirectX()
{
	//shut down all the threads and then clear the thread handles list
	for(HandleList::iterator it = ThreadHandles.begin(); it != ThreadHandles.end(); ++it)
	{
		TerminateThread(*it,0);
		CloseHandle(*it);
	}
	ThreadHandles.clear();

	//these guys are closed automatically per the documentation of CreateEvent
	BackToSleepEvents.clear();
	WakeUpEvents.clear();
}

void CPlatformDirectX::Init(LPDIRECT3DDEVICE9 pD3DDevice, HWND hWnd, D3DPRESENT_PARAMETERS &d3dpp,int *pWindowMarginX,int *pWindowMarginY)
{
	m_hWnd = hWnd;
	m_pD3DDevice = pD3DDevice;
	m_d3dpp = d3dpp;
	m_pWindowMarginX = pWindowMarginX;
	m_pWindowMarginY = pWindowMarginY;
}

void CPlatformDirectX::RenderThreadFunc(void *pParams)
{
	SThreadParameters *pThreadParameters = (SThreadParameters *)pParams;
	CRayTracer &RayTracer = CRayTracer::GetSingleton();
	CPlatformDirectX *pPlatform = pThreadParameters->m_pPlatform;

	while(true)
	{
		//wait to be woken up
		WaitForSingleObject(pThreadParameters->m_hWakeUpEvent, INFINITE);

		//render all the cells we should render
		int nRayBounceCount = RayTracer.GetRayBounceCount();

		unsigned int nNextCellToRender;
		while(pPlatform->m_nNextCellToRender.GetNextValueDownIfgt0(nNextCellToRender))
		{
			pThreadParameters->m_pPlatform->m_pCurrentCamera->RenderCell(nNextCellToRender,nRayBounceCount,CCamera::kRenderNormal);
		}

		//set our back to sleep event
		SetEvent(pThreadParameters->m_hBackToSleepEvent);
	}
}

void CPlatformDirectX::Render(CCamera *pCamera, int nScreenWidth, int nScreenHeight, int nMaxRayBounce)
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

	//set the events to wake up our threads
	int nIndex = 0;
	for(HandleList::iterator it = WakeUpEvents.begin(); it != WakeUpEvents.end(); ++it)
	{
		//only wake up as many threads as we should
		if(nIndex < m_nNumActiveProcessors)
			SetEvent(*it);
		++nIndex;
	}

	//wait for the threads to go back to sleep
	WaitForMultipleObjects(m_nNumActiveProcessors,&(*(BackToSleepEvents.begin())),true,INFINITE);
}

void CPlatformDirectX::FreeVolatileResources()
{
}

void CPlatformDirectX::InitVolatileResources()
{
}

void CPlatformDirectX::LogText(const char *szText)
{
	OutputDebugStringA(szText);
}

int CPlatformDirectX::CriticalSectionAdd()
{
	CRITICAL_SECTION cs;
	InitializeCriticalSection(&cs);
	m_CriticalSections.push_back(cs);
	return m_CriticalSections.size() - 1;
}

void CPlatformDirectX::CriticalSectionEnter(int nCriticalSectionIndex)
{
	EnterCriticalSection(&m_CriticalSections[nCriticalSectionIndex]);
}

void CPlatformDirectX::CriticalSectionLeave(int nCriticalSectionIndex)
{
	LeaveCriticalSection(&m_CriticalSections[nCriticalSectionIndex]);
}

typedef BOOL (WINAPI *LPFN_GLPI)(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, 
    PDWORD);

DWORD CPlatformDirectX::CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
    DWORD i;
    
    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}

int CPlatformDirectX::GetNumLogicalProcessors()
{
    LPFN_GLPI glpi;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;
    PCACHE_DESCRIPTOR Cache;

    glpi = (LPFN_GLPI) GetProcAddress(
                            GetModuleHandle(TEXT("kernel32")),
                            "GetLogicalProcessorInformation");
    if (NULL == glpi) 
    {
        return 1;
    }

    while (!done)
    {
        DWORD rc = glpi(buffer, &returnLength);

        if (FALSE == rc) 
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if (buffer) 
                    free(buffer);

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
                        returnLength);

                if (NULL == buffer) 
                {
                    return 1;
                }
            } 
            else 
            {
                return 1;
            }
        } 
        else
        {
            done = TRUE;
        }
    }

    ptr = buffer;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) 
    {
        switch (ptr->Relationship) 
        {
        case RelationNumaNode:
            // Non-NUMA systems report a single record of this type.
            numaNodeCount++;
            break;

        case RelationProcessorCore:
            processorCoreCount++;

            // A hyperthreaded core supplies more than one logical processor.
            logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
            break;

        case RelationCache:
            // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
            Cache = &ptr->Cache;
            if (Cache->Level == 1)
            {
                processorL1CacheCount++;
            }
            else if (Cache->Level == 2)
            {
                processorL2CacheCount++;
            }
            else if (Cache->Level == 3)
            {
                processorL3CacheCount++;
            }
            break;

        case RelationProcessorPackage:
            // Logical processors share a physical package.
            processorPackageCount++;
            break;

        default:
            //_tprintf(TEXT("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
            break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

	/*
    _tprintf(TEXT("\nGetLogicalProcessorInformation results:\n"));
    _tprintf(TEXT("Number of NUMA nodes: %d\n"), 
             numaNodeCount);
    _tprintf(TEXT("Number of physical processor packages: %d\n"), 
             processorPackageCount);
    _tprintf(TEXT("Number of processor cores: %d\n"), 
             processorCoreCount);
    _tprintf(TEXT("Number of logical processors: %d\n"), 
             logicalProcessorCount);
    _tprintf(TEXT("Number of processor L1/L2/L3 caches: %d/%d/%d\n"), 
             processorL1CacheCount,
             processorL2CacheCount,
             processorL3CacheCount);*/
    
    free(buffer);

    return logicalProcessorCount;
}

void CPlatformDirectX::ExitApplication()
{
	PostQuitMessage(0);
}
/*
void CPlatformDirectX::LoadTexture(CTexture *pTexture)
{
	bool bSuccess = false;
	LPDIRECT3DTEXTURE9 pDXTexture=NULL;
	HRESULT hr = D3DXCreateTextureFromFileA(m_pD3DDevice,pTexture->GetFileName(),&pDXTexture);

	if(SUCCEEDED(hr))
	{
		D3DSURFACE_DESC Desc;
		hr = pDXTexture->GetLevelDesc(0,&Desc);
		if(SUCCEEDED(hr))
		{
			D3DLOCKED_RECT LockedRect;
			hr = pDXTexture->LockRect(0,&LockedRect,NULL,D3DLOCK_READONLY);
			if(SUCCEEDED(hr))
			{
				pTexture->SetPixelData(Desc.Width,Desc.Height,(unsigned int*)LockedRect.pBits,LockedRect.Pitch);
				pDXTexture->UnlockRect(0);
				bSuccess = true;
			}
		}
		pDXTexture->Release();
	}

	//set our debug texture if we didnt succeeded
	if(!bSuccess)
		SetDebugTexture(pTexture);
}
*/

void CPlatformDirectX::StartRecordingVideo()
{
	if(m_bRecording)
		return;

	m_bRecording = true;

	//make sure our frames folder exists
	CreateDirectory(L"Videos",NULL);
	CreateDirectory(L"Videos/Frames",NULL);

	//delete all .bmp's
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(L"Videos/Frames/*.bmp", &ffd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			std::wstring filename = L"Videos/Frames/";
			filename += ffd.cFileName;
			DeleteFile(filename.c_str());
		}
		while (FindNextFile(hFind, &ffd) != 0);
	}

	m_nCurrentRecordingFrame = 0;
}

void CPlatformDirectX::StopRecordingVideo()
{
	if(!m_bRecording)
		return;

	m_bRecording = false;

	//create the video
	FinalizeVideo();
}

void CPlatformDirectX::PossiblyOverrideFrameTime(MRTFLOAT &fFrameTime)
{
	if(m_bRecording)
	{
		fFrameTime = 1.0f / 30.0f;
	}
}

void CPlatformDirectX::PostRender()
{
	if(m_bRecording)
	{
		DoScreenCapGDI();
		m_nCurrentRecordingFrame++;
	}
}

void CPlatformDirectX::DoScreenCapGDI()
{
	HWND hDesktopWnd = m_hWnd;
	RECT windowRect;
	GetClientRect( hDesktopWnd, &windowRect );
	int nScreenWidth = windowRect.right - windowRect.left;
	int nScreenHeight = windowRect.bottom - windowRect.top;

	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	HBITMAP hCaptureBitmap =CreateCompatibleBitmap(hDesktopDC, 
		nScreenWidth, nScreenHeight);
	SelectObject(hCaptureDC,hCaptureBitmap); 
	BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight,
		hDesktopDC,0,0,SRCCOPY|CAPTUREBLT); 

	char szFileName[256];
	sprintf(szFileName,"Videos/Frames/VideoFrame%i.bmp",m_nCurrentRecordingFrame);

	BITMAP bitmap;
	GetObject(hCaptureBitmap, sizeof(BITMAP), &bitmap);

	int size = bitmap.bmWidth*bitmap.bmHeight*bitmap.bmBitsPixel/8;

	if(m_nGdiScreenCapBufferSize < size)
	{
		delete[] m_pGdiScreenCapBuffer;
		m_nGdiScreenCapBufferSize = size;
		m_pGdiScreenCapBuffer = new BYTE [m_nGdiScreenCapBufferSize];
	}

	::GetBitmapBits((HBITMAP)hCaptureBitmap,size,m_pGdiScreenCapBuffer);
	WriteBMP(szFileName,bitmap.bmWidth,bitmap.bmHeight,bitmap.bmBitsPixel,(int*)m_pGdiScreenCapBuffer);

	ReleaseDC(hDesktopWnd,hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
}

void CPlatformDirectX::WriteBMP(const char *name, int W, int H, int Bpp, int *data)
{
	BITMAPINFO Bmi={0};
	Bmi.bmiHeader.biSize      = sizeof(BITMAPINFOHEADER);
	Bmi.bmiHeader.biWidth      =  W;
	Bmi.bmiHeader.biHeight      = H;
	Bmi.bmiHeader.biPlanes      = 1;
	Bmi.bmiHeader.biBitCount    = Bpp; 
	Bmi.bmiHeader.biCompression  = BI_RGB;
	Bmi.bmiHeader.biSizeImage    = W*H*Bpp/8; 

	FILE* image = fopen (name,"wb");
	if(image==0)
		return;
	int h = abs(Bmi.bmiHeader.biHeight);
	int w = abs(Bmi.bmiHeader.biWidth);
	Bmi.bmiHeader.biHeight=-h;
	Bmi.bmiHeader.biWidth=w;
	int sz = Bmi.bmiHeader.biSizeImage;

	BITMAPFILEHEADER bfh;
	bfh.bfType=('M'<<8)+'B'; 
	bfh.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); 
	bfh.bfSize=sz+bfh.bfOffBits; 
	bfh.bfReserved1=0; 
	bfh.bfReserved2=0; 

	fwrite(&bfh,sizeof(bfh),1,image);
	fwrite(&Bmi.bmiHeader,sizeof(BITMAPINFOHEADER),1,image);
	fwrite(data,sz,1,image);
	fclose(image);
}

void CPlatformDirectX::FinalizeVideo(void)
{
	//find what the next output filename we can use is
	FILE *File = NULL;
	bool done = false;
	char szVideoFilename[256];
	int nextVideoIndex = 1;
	do 
	{
		sprintf(szVideoFilename,"Videos/Video%i.mp4",nextVideoIndex);
		File = fopen(szVideoFilename,"rb");

		if(File)
		{
			fclose(File);
			nextVideoIndex++;
		}
		else
		{
			done = true;
		}
	} 
	while(!done);

	//encode our video!
	char szBuffer[256];
	sprintf(szBuffer,"Videos\\ffmpeg -r %i -b:v 1800 -i Videos/Frames/VideoFrame%%d.bmp %s",30,szVideoFilename);
	system(szBuffer);
}

void CPlatformDirectX::SetResolution(int nWidth, int nHeight)
{
	//resize the window
	DWORD nWindowFlags = WS_CAPTION|WS_SYSMENU;
	RECT ScreenRect;
	ScreenRect.left = 0;
	ScreenRect.top = 0;
	ScreenRect.right = nWidth;
	ScreenRect.bottom = nHeight;

	AdjustWindowRect(&ScreenRect,nWindowFlags,false);

	*m_pWindowMarginX = ScreenRect.left;
	*m_pWindowMarginY = ScreenRect.top;

	//center the window
    RECT rc ;
	GetWindowRect ( m_hWnd, &rc ) ;   

    SetWindowPos( m_hWnd, 0, 
        (GetSystemMetrics(SM_CXSCREEN) - (ScreenRect.right - ScreenRect.left))/2,
        (GetSystemMetrics(SM_CYSCREEN) - (ScreenRect.bottom - ScreenRect.top))/2,
         ScreenRect.right - ScreenRect.left, ScreenRect.bottom - ScreenRect.top,
		 SWP_NOZORDER );


	//set the new resolution
	m_d3dpp.BackBufferWidth = nWidth;
	m_d3dpp.BackBufferHeight = nHeight;
	m_pD3DDevice->Reset(&m_d3dpp);

	//tell the raytracer about the resolution switch
	CRayTracer::GetSingleton().SetResolution(nWidth,nHeight);
}

void CPlatformDirectX::MakeHTTPRequest(const char *szURL,HTTPCallback callback,void *pUserData,EHttpRequestType eRequestType /*= kGeneral*/)
{
	m_LibCurlHttp.Request(szURL,callback,pUserData,(int)eRequestType);
}

void CPlatformDirectX::CancelHTTPRequestsOfType(EHttpRequestType eRequestType)
{
	m_LibCurlHttp.CancelRequestsOfType(eRequestType);
}

void CPlatformDirectX::Update()
{
	m_LibCurlHttp.Update();
}