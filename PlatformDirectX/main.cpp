// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include "../RTCode/RayTracer.h"
#include "PlatformDirectX.h"
#include <stdio.h>

enum EResolution
{
	k50x50 = 0,
	k100x100,
	k200x200,
	k300x300,
	k320x240,
	k400x400,
	k500x500,
	k600x600,
	k640x480,
	k800x600,
	k800x800,
	k1000x1000,
	k1024x768,
	k1280x720,

	kCount
};

EResolution g_currentResolution = k500x500;

CPlatformDirectX g_DirectXPlatform;

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d; // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev; // the pointer to the device class
bool DeviceLost = false;
D3DPRESENT_PARAMETERS d3dpp;

// function prototypes
void initD3D(HWND hWnd); // sets up and initializes Direct3D
void render_frame(void); // renders a single frame
void cleanD3D(void); // closes Direct3D and releases memory

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void GetResolution(EResolution eResolution, int &width, int &height);

int g_nWindowMarginX;
int g_nWindowMarginY;

void FreeVolatileResources()
{
	g_DirectXPlatform.FreeVolatileResources();
}

void InitVolatileResources()
{
	g_DirectXPlatform.InitVolatileResources();
}

void CheckForLostDevice(void)
{
	HRESULT hr = d3ddev->TestCooperativeLevel();

	switch(hr)
	{
		case D3DERR_DEVICELOST:
		{
			FreeVolatileResources();
			break;
		}
		case D3DERR_DEVICENOTRESET:
		{
			d3ddev->Reset(&d3dpp);
			InitVolatileResources();
			break;
		}
	}
}

void GetResolution(EResolution eResolution, int &width, int &height)
{
	switch(eResolution)
	{
		case k50x50:width = 50; height = 50;return;
		case k100x100:width = 100; height = 100;return;
		case k200x200:width = 200; height = 200;return;
		case k300x300:width = 300; height = 300;return;
		case k320x240:width = 320; height = 240;return;
		case k400x400:width = 400; height = 400;return;
		case k500x500:width = 500; height = 500;return;
		case k640x480:width = 640; height = 480;return;
		case k600x600:width = 600; height = 600;return;
		case k800x600:width = 800; height = 600;return;
		case k800x800:width = 800; height = 800;return;
		case k1000x1000:width = 1000; height = 1000;return;
		case k1024x768:width = 1024; height = 768;return;
		case k1280x720:width = 1280; height = 720;return;
	}
}

void AdjustScreenSize(int delta)
{
	/*
	g_currentResolution = (EResolution)((int)g_currentResolution + delta);

	if(g_currentResolution < 0)
		g_currentResolution = (EResolution)0;
	else if(g_currentResolution >= kCount)
		g_currentResolution = (EResolution)((int)kCount - 1);

	int nWidth;
	int nHeight;
	GetResolution(g_currentResolution,nWidth,nHeight);

	d3dpp.BackBufferWidth = nWidth;
	d3dpp.BackBufferHeight = nHeight;
	d3ddev->Reset(&d3dpp);
	*/
}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	int nWidth,nHeight;
	GetResolution(g_currentResolution,nWidth,nHeight);

    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // wc.hbrBackground = (HBRUSH)COLOR_WINDOW;    // not needed any more
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

	DWORD nWindowFlags = WS_CAPTION|WS_SYSMENU;
	RECT ScreenRect;
	ScreenRect.left = 0;
	ScreenRect.top = 0;
	ScreenRect.right = nWidth;
	ScreenRect.bottom = nHeight;

	AdjustWindowRect(&ScreenRect,nWindowFlags,false);

	g_nWindowMarginX = ScreenRect.left;
	g_nWindowMarginY = ScreenRect.top;

    hWnd = CreateWindowEx(NULL,
                          L"WindowClass",
                          L"Ray Tracer",
						  nWindowFlags,
						  0, 0,    // the starting x and y positions should be 0
						  ScreenRect.right - ScreenRect.left, ScreenRect.bottom - ScreenRect.top,    // set the window to 640 x 480
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

	//center the window
    RECT rc ;
    GetWindowRect ( hWnd, &rc ) ;    
    SetWindowPos( hWnd, 0, 
        (GetSystemMetrics(SM_CXSCREEN) - rc.right)/2,
        (GetSystemMetrics(SM_CYSCREEN) - rc.bottom)/2,
         ScreenRect.right - ScreenRect.left, ScreenRect.bottom - ScreenRect.top,
		 SWP_NOZORDER|SWP_NOSIZE );

    // set up and initialize Direct3D
    initD3D(hWnd);

	g_DirectXPlatform.Init(d3ddev,hWnd,d3dpp,&g_nWindowMarginX,&g_nWindowMarginY);
	CRayTracer::GetSingleton().Init(&g_DirectXPlatform,nWidth, nHeight);

    // enter the main loop:

    MSG msg;

    while(TRUE)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT)
            break;

		CheckForLostDevice();

		if(!DeviceLost)
			render_frame();
    }

    // clean up DirectX and COM
    cleanD3D();

    return msg.wParam;
}

unsigned char TranslateChar(unsigned char nChar)
{
	switch((unsigned char)nChar)
	{
		case VK_OEM_3:nChar='~';break;
	}

	return nChar;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
		case WM_MOUSEMOVE:
		{
			RECT WindowRect;
			GetWindowRect(hWnd,&WindowRect);
			
			POINT CursorPoint;
			GetCursorPos(&CursorPoint);

			CursorPoint.x -= WindowRect.left - g_nWindowMarginX;
			CursorPoint.y -= WindowRect.top - g_nWindowMarginY;

			g_DirectXPlatform.SetMousePosition(CursorPoint.x,CursorPoint.y);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			CRayTracer::GetSingleton().GetPlatform().SetLeftMouseDown(true);
			break;
		}
		case WM_LBUTTONUP:
		{
			CRayTracer::GetSingleton().OnLeftClick();
			CRayTracer::GetSingleton().GetPlatform().SetLeftMouseDown(false);
			break;
		}
		case WM_KEYUP:
		{
			CRayTracer::GetSingleton().OnKeyUp(TranslateChar((unsigned char)wParam));

			/*
			switch(wParam)
			{
				case VK_RETURN:
				{
					d3dpp.Windowed = !d3dpp.Windowed;
					d3ddev->Reset(&d3dpp);

					if(d3dpp.Windowed)
					{
						RECT rc ;
						GetWindowRect ( hWnd, &rc ) ;    
						SetWindowPos( hWnd, 0, 
						(GetSystemMetrics(SM_CXSCREEN) - rc.right)/2,
						(GetSystemMetrics(SM_CYSCREEN) - rc.bottom)/2,
						0, 0, SWP_NOZORDER|SWP_NOSIZE );
					}
					break;
				}
			}
			*/
			break;
		}
		case WM_KEYDOWN:
		{
			CRayTracer::GetSingleton().OnKeyDown(TranslateChar((unsigned char)wParam));
			break;
		}
		case WM_CHAR:
		{
			CRayTracer::GetSingleton().OnKeyTyped((unsigned char)wParam);
			break;
		}
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION); // create the Direct3D interface

    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program fullscreen, not windowed
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit

	int nWidth,nHeight;
	GetResolution(g_currentResolution,nWidth,nHeight);
	d3dpp.BackBufferWidth = nWidth;
	d3dpp.BackBufferHeight = nHeight;

	d3dpp.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;


    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);

	
	InitVolatileResources();
}

// this is the function used to render a single frame
void render_frame(void)
{
	static DWORD nLastTickCount = GetTickCount();
	static bool bFirstFrame = true;
	MRTFLOAT fFrameTime = 0;
	if(bFirstFrame)
	{
		bFirstFrame = false;
	}
	else
	{
		DWORD nTickCount = GetTickCount();
		fFrameTime = ((MRTFLOAT)((long)(nTickCount - nLastTickCount))) / 1000.0f;
		nLastTickCount = nTickCount;
	}

	//render our frame (software rendered)
	CRayTracer::GetSingleton().RenderFrame(fFrameTime);

	d3ddev->BeginScene();    // begins the 3D scene

	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	if(!FAILED(d3ddev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer)))
	{
		D3DLOCKED_RECT LockedRect;
		if(!FAILED(pBackBuffer->LockRect(&LockedRect,NULL,D3DLOCK_DISCARD)))
		{
			//copy the frame to the screen
			CRayTracer::GetSingleton().CopyFrameToPixels((unsigned char *)LockedRect.pBits,LockedRect.Pitch);

			pBackBuffer->UnlockRect();
		}

		pBackBuffer->Release();
	}


    d3ddev->EndScene();    // ends the 3D scene

    d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen

	//tell the ray tracer we are done rendering
	CRayTracer::GetSingleton().PostRender();
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
    d3ddev->Release(); // close and release the 3D device
    d3d->Release(); // close and release Direct3D
}