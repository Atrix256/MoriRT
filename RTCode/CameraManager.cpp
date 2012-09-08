/*======================================================

CameraManager.h

Holds and manages cameras

======================================================*/

#include "CameraManager.h"
#include "Camera.h"

CCameraManager CameraManager;

CCameraManager::CCameraManager()
{
}

CCameraManager::~CCameraManager()
{
	//free the memory for each camera
	for(CameraList::iterator it = m_CameraList.begin(); it != m_CameraList.end(); ++it)
	{
		delete (*it);
	}

	//empty the camera list
	m_CameraList.clear();
}

CCamera *CCameraManager::AddCamera(Vec3 vCameraPos, Vec3 vCameraTarget, Vec3 vUp, int nRenderWidth, int nRenderHeight, MRTFLOAT fFOV, MRTFLOAT fScreenRectSize, int nScreenGridCellCount, Vec3 v3dOffset /*= Vec3(2.0f,0.0f,0.0f)*/)
{
	CCamera *pNewCamera = new CCamera(vCameraPos, vCameraTarget, vUp, nRenderWidth, nRenderHeight, fFOV, fScreenRectSize, nScreenGridCellCount, v3dOffset, false);

	m_CameraList.push_back(pNewCamera);

	return pNewCamera;
}

void CCameraManager::Update()
{
	//give each camera an update
	for(CameraList::iterator it = m_CameraList.begin(); it != m_CameraList.end(); ++it)
	{
		(*it)->Update();
	}
}

void CCameraManager::SetResolution(int nWidth, int nHeight)
{
	//tell each camera about the resolution change
	for(CameraList::iterator it = m_CameraList.begin(); it != m_CameraList.end(); ++it)
	{
		(*it)->SetResolution(nWidth,nHeight);
	}
}