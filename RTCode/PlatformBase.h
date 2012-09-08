/*======================================================

PlatformBase.h

This is the platform specific functionality class.  Derive your own class from this
and pass it to RayTracer::Initto implement functionality for your platform

======================================================*/

#ifndef PLATFORMBASE_H
#define PLATFORMBASE_H

#include "Vec3.h"

//forward declarations
class CCamera;
class CTexture;
class CUIStack;

//typedefs
enum ERequestFinishCode
{
	kHttpRequestOK,
	kHttpRequestCommsError,
	kHttpRequestServerError,
	kHttpRequestUnknownResponse,
	kHttpRequestCancel
};
typedef void(*HTTPCallback)(void *pUserData, ERequestFinishCode eFinishCode, const char *szResponse);

class CPlatformBase
{
public:
	CPlatformBase();
	virtual ~CPlatformBase() {}

	virtual void GetMousePosition(int &nMouseX, int &nMouseY) {}
	virtual bool GetLeftMouseDown() {return m_bLeftMouseDown;}
	virtual void SetLeftMouseDown(bool bDown) {m_bLeftMouseDown = bDown;}

	virtual void LogText(const char *szText) {}

	virtual void Render(CCamera *pCamera, int nScreenWidth, int nScreenHeight, int nMaxRayBounce);

	virtual int CriticalSectionAdd() {return -1;}
	virtual void CriticalSectionEnter(int nCriticalSectionIndex) {}
	virtual void CriticalSectionLeave(int nCriticalSectionIndex) {}

	virtual void ExitApplication() {}

	virtual int GetNumberRenderThreads() {return 1;}
	virtual int GetNumberActiveRenderThreads() {return GetNumberRenderThreads();}
	virtual void SetNumberActiveRenderThreads(int nNum) {}

	virtual void SetDebugTexture(CTexture *pTexture);

	virtual void StartRecordingVideo() {}
	virtual void StopRecordingVideo() {}
	virtual bool IsRecordingVideo() {return false;}
	virtual void PossiblyOverrideFrameTime(MRTFLOAT &fFrameTime) {}
	virtual void PostRender() {}

	virtual void SetResolution(int nWidth, int nHeight) {}

	void LoadTexture(CTexture *pTexture);

	virtual void Update() {}

	enum EHttpRequestType
	{
		kGeneral,
		kLogin,
		kCreateAccount,
		kCreateMatch,
		kMatchPing,
		kAll
	};

	virtual void MakeHTTPRequest(const char *szURL,HTTPCallback callback,void *pUserData,EHttpRequestType eRequestType = kGeneral) = 0;
	virtual void CancelHTTPRequestsOfType(EHttpRequestType eRequestType) = 0;

protected:
	bool m_bLeftMouseDown;
};

#endif //PLATFORMBASE_H