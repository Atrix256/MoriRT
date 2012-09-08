/*======================================================

CameraManager.h

Holds and manages cameras

======================================================*/

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <vector>
#include "Vec3.h"

//forward declarations
class CCamera;
class CScene;

//typedefs
typedef std::vector<CCamera *> CameraList;

class CCameraManager
{
public:
	CCameraManager();
	~CCameraManager();

	CCamera *AddCamera(Vec3 vCameraPos, Vec3 vCameraTarget, Vec3 vUp, int nRenderWidth, int nRenderHeight, MRTFLOAT fFOV, MRTFLOAT fScreenRectSize, int nScreenGridCellCount, Vec3 v3dOffset = Vec3(5.0f,0.0f,0.0f));
	void Update();

	void SetResolution(int nWidth, int nHeight);

private:
	CameraList m_CameraList;
};

extern CCameraManager CameraManager;

#endif //CAMERAMANAGER_H