/*======================================================

Camera.cpp

The camera object

======================================================*/

#include "Camera.h"
#include "Scene.h"
#include "Ray.h"
#include "PrimitiveBase.h"
#include "ScreenCell.h"
#include "RayTracer.h"
#include "Matrix.h"
#include "PlatformBase.h"

CCamera::CCamera(Vec3 vCameraPos, Vec3 vCameraTarget, Vec3 vUp, int nRenderWidth, int nRenderHeight, MRTFLOAT fFOV, MRTFLOAT fScreenRectSize, int nScreenGridCellCount, Vec3 v3dOffset, bool bChildCamera)
:m_pScene(0)
,m_vCameraPos(vCameraPos)
,m_vCameraTarget(vCameraTarget)
,m_vCameraUp(vUp)
,m_nRenderWidth(nRenderWidth)
,m_nRenderHeight(nRenderHeight)
,m_fFOV(fFOV * 3.14159f / 180.0f)
,m_fScreenRectWidth(fScreenRectSize)
,m_fScreenRectHeight(fScreenRectSize * (MRTFLOAT)nRenderHeight / (MRTFLOAT)nRenderWidth)
,m_nScreenGridCellCount(nScreenGridCellCount)
,m_bOrthographic(false)
,m_fOrthoWidth(0.0f)
,m_fOrthoHeight(0.0f)
,m_fOrthoOffsetX(0.0f)
,m_fOrthoOffsetY(0.0f)
,m_fOrthoNear(0.0f)
,m_ScreenGrid(this,nScreenGridCellCount, nRenderWidth, nRenderHeight)
,m_bDirty(true)
,m_nRayCount(0)
,m_pRays(0)
,m_bRenderCellBoundaries(false)
,m_bRedBlue3dMode(false)
,m_bIs3dChildCamera(bChildCamera)
,m_pOtherCameraFor3d(0)
{
	m_v3dOffset = v3dOffset;

	Vec3 vCameraFwd = vCameraTarget - vCameraPos;
	vCameraFwd.Normalize();

	Vec3 vCameraLeft = vCameraFwd.Cross(vUp);
	m_vCameraUp = vCameraLeft.Cross(vCameraFwd);

	m_fVerticalFOV = m_fFOV * ((MRTFLOAT)m_nRenderHeight / (MRTFLOAT)m_nRenderWidth);
}

void CCamera::LookAt(Vec3 vCameraPos, Vec3 vCameraTarget, Vec3 vUp)
{
	m_vCameraPos = vCameraPos;
	m_vCameraTarget = vCameraTarget;
	m_vCameraUp = vUp;

	Vec3 vCameraFwd = vCameraTarget - vCameraPos;
	vCameraFwd.Normalize();

	Vec3 vCameraLeft = vCameraFwd.Cross(vUp);
	m_vCameraUp = vCameraLeft.Cross(vCameraFwd);

	DirtyAllCachedData();

	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->DirtyAllCachedData();
}

CCamera::~CCamera()
{
	delete[] m_pRays;
	m_pRays = 0;

	if(m_pOtherCameraFor3d)
		delete m_pOtherCameraFor3d;
}

void CCamera::Update()
{
	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->Update();

	//if we aren't dirty, nothing to do
	if(!m_bDirty)
	{
		return;
	}

	//else, we are going to update it so mark it as clean
	m_bDirty = false;

	//calculate our camera's forward and left vectors
	Vec3 vFwd = m_vCameraTarget - m_vCameraPos;
	vFwd.Normalize();
	Vec3 vLeft = vFwd.Cross(m_vCameraUp);

	//make space for how many rays we are going to have.  width * height * 6 MRTFLOATs
	//6 MRTFLOATs per ray, 3 for the ray origin, 3 for the ray direction
	if(m_nRayCount < m_nRenderWidth * m_nRenderHeight)
	{
		delete[] m_pRays;
		m_pRays = new Ray[m_nRenderWidth * m_nRenderHeight];
		m_nRayCount = m_nRenderWidth * m_nRenderHeight;
	}

	MRTFLOAT fPercentY = -0.5f;
	MRTFLOAT fDeltaX = 1.0f / (MRTFLOAT)m_nRenderWidth;
	MRTFLOAT fDeltaY = 1.0f / (MRTFLOAT)m_nRenderHeight;
	Ray *pCurrentRay = m_pRays;


	Vec3 vUpperLeft;
	Vec3 vUpperRight;
	Vec3 vLowerLeft;

	{
		Matrix mRotationX;
		Matrix mRotationY;
		Matrix mRotation;
		mRotationX.RotationY(-m_fFOV/2.0f);
		mRotationY.RotationX(-m_fVerticalFOV/2.0f);
		mRotation = mRotationX * mRotationY;

		vUpperLeft = vFwd;
		vUpperLeft = mRotation * vUpperLeft;
		vUpperLeft.Normalize();
	}

	{
		Matrix mRotationX;
		Matrix mRotationY;
		Matrix mRotation;
		mRotationX.RotationY(m_fFOV/2.0f);
		mRotationY.RotationX(-m_fVerticalFOV/2.0f);
		mRotation = mRotationX * mRotationY;

		vUpperRight = vFwd;
		vUpperRight = mRotation * vUpperRight;
		vUpperRight.Normalize();
	}

	{
		Matrix mRotationX;
		Matrix mRotationY;
		Matrix mRotation;
		mRotationX.RotationY(-m_fFOV/2.0f);
		mRotationY.RotationX(m_fVerticalFOV/2.0f);
		mRotation = mRotationX * mRotationY;

		vLowerLeft = vFwd;
		vLowerLeft = mRotation * vLowerLeft;
		vLowerLeft.Normalize();
	}

	Vec3 vRight = vUpperRight - vUpperLeft;
	Vec3 vDown  = vLowerLeft - vUpperLeft;

	MRTFLOAT fFOVCorrectionX = tanf(m_fFOV/2.0f) * 2.0f;
	MRTFLOAT fFOVCorrectionY = tanf(m_fVerticalFOV/2.0f) * 2.0f;

	//loop through all the rows
	for(int nIndexY = 0; nIndexY < m_nRenderHeight; ++nIndexY)
	{
		MRTFLOAT fPercentX = -0.5f;
		Vec3 vVerticalComponent = m_vCameraUp * m_fScreenRectHeight * fPercentY;

		//loop through all the columns
		for(int nIndexX = 0; nIndexX < m_nRenderWidth; nIndexX++)
		{
			//special calculations for orthographic mode
			if(m_bOrthographic)
			{
				//store the ray information
				*pCurrentRay = Ray(Vec3(fPercentX * m_fOrthoWidth + m_fOrthoOffsetX, -1.0f * fPercentY * m_fOrthoHeight + m_fOrthoOffsetY,m_fOrthoNear),Vec3(0,0,1.0f));
			}
			//perspective mode
			else
			{
				Vec3 vDir = vFwd - (vLeft * fPercentX * fFOVCorrectionX) - (m_vCameraUp * fPercentY * fFOVCorrectionY);
				vDir.Normalize();
				*pCurrentRay = Ray(m_vCameraPos,vDir);
			}

			//move to the next ray destination percent on the x axis
			fPercentX += fDeltaX;

			//move to the next ray
			pCurrentRay ++;
		}

		//move to the next ray destination percent on the y axis
		fPercentY += fDeltaY;
	}
}

Vec3 CCamera::GetColorForRay(bool bDebugThisRay, CScreenCell &ScreenCell, unsigned int nRayCacheRayIndex, unsigned int nCameraRayIndex, int nMaxRayBounce)
{
	#if PIXELDEBUG == false
		bDebugThisRay = false;
	#endif
	
	//if no scene, return black
	if(!m_pScene)
	{
		return Vec3(0,0,0);
	}

	/*
	//convert the ray direction vector to an RGB color
	Ray CameraRay = m_pRays[nCameraRayIndex];
	Vec3 vColor(CameraRay.m_vDir.m_fX,CameraRay.m_vDir.m_fY,CameraRay.m_vDir.m_fZ);
	vColor = vColor + Vec3(1.0f,1.0f,1.0f);
	vColor = vColor / 2.0f;

	//vColor.m_fX = 0.0f;
	//vColor.m_fY = 0.0f;
	//vColor.m_fZ = 0.0f;
	return vColor;
	*/

	//get the color for this ray from the scene, adding in the ambient light of the scene
	MRTFLOAT fTimeTilFirstHit; //just a dummy parameter needed, but we could make a depth buffer at this point if we wanted to by storing the value in a buffer (or put it in the alpha channel or something)
	return m_pScene->GetColorForRay(bDebugThisRay,ScreenCell,nRayCacheRayIndex,m_pRays[nCameraRayIndex],1.0f,nMaxRayBounce,fTimeTilFirstHit);
}

void CCamera::DirtyAllCachedData(void)
{
	//remember that the camera is dirty
	m_bDirty = true;

	//clear all cached data
	for(int nIndex = 0; nIndex < m_nScreenGridCellCount * m_nScreenGridCellCount; ++nIndex)
	{
		MarkScreenCellAsDirty(nIndex);
	}

	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->DirtyAllCachedData();
}

void CCamera::SetScene(CScene *pScene)
{
	if(m_pScene != pScene)
	{
		DirtyAllCachedData();

		m_pScene = pScene;

		DirtyAllCachedData();
	}

	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->SetScene(pScene);
}

void CCamera::RenderCell(int nIndex, int nMaxRayBounce, ERenderMode eRenderMode, CScreenCell *pOverRideCellPixels /*= NULL*/)
{
	CScreenCell &ScreenCell = m_ScreenGrid.GetCell(nIndex);

	//if we are in 3d mode and this is our main render call
	if(m_bRedBlue3dMode && eRenderMode == kRenderNormal)
	{
		//we are rendering the red channel
		eRenderMode = kRenderRed;

		//have the other camera render the blue channel
		m_pOtherCameraFor3d->RenderCell(nIndex, nMaxRayBounce, kRenderBlue, &ScreenCell);
	}

	//get our pixels from this cell, or if we were told to get our pixels from a different cell, get them from that
	//this comes in handy when there are 2 cameras in the case of red/blue 3d but they want to render to the same pixels
	int nScreenCellPixelWidth, nScreenCellPixelHeight;
	unsigned char *pScreenCellPixels;
	if(pOverRideCellPixels)
		pScreenCellPixels = pOverRideCellPixels->GetPixelBuffer(nScreenCellPixelWidth,nScreenCellPixelHeight);
	else
		pScreenCellPixels = ScreenCell.GetPixelBuffer(nScreenCellPixelWidth,nScreenCellPixelHeight);

	//if our pixels haven't changed since last time, nothing to do here
	if(ScreenCell.m_nFlags & CScreenCell::kFlags_PixelsClean)
	{
		//render our screen cell boundaries if we should
		if(m_bRenderCellBoundaries)
			ScreenCell.RenderCellBoundaries();
		return;
	}

	//update metrics
	if(ScreenCell.m_nFlags & CScreenCell::kFlags_GeometryClean)
		CRayTracer::GetSingleton().IncrementCellsUpdated();
	else
		CRayTracer::GetSingleton().IncrementCellsRendered();

	int nStartX = ScreenCell.GetStartPixelX();
	int nStartY = ScreenCell.GetStartPixelY();
	int nEndX = ScreenCell.GetEndPixelX();
	int nEndY = ScreenCell.GetEndPixelY();
	int nWidth = nEndX - nStartX;
	int nHeight = nEndY - nStartY;

	int nScreenWidth = CRayTracer::GetSingleton().GetScreenWidth();

	unsigned char *pCurrentRow = pScreenCellPixels;
	unsigned int nCurrentRowCameraRayIndex = (nStartY * nScreenWidth + nStartX);
	int nRayCacheRayIndex = 0;

	#if PIXELDEBUG == true
		int nMouseX, nMouseY;
		if(eRenderMode == kRenderNormal)
		{
			CRayTracer::GetSingleton().GetPlatform().GetMousePosition(nMouseX,nMouseY);
			nMouseX -= nStartX;
			nMouseY -= nStartY;
		}
	#endif

	for(int nIndexY = 0; nIndexY < nHeight; ++nIndexY)
	{
		unsigned char *pCurrentPixel = pCurrentRow;
		unsigned int nCurrentCameraRayIndex = nCurrentRowCameraRayIndex;

		for(int nIndexX = 0; nIndexX < nWidth; ++nIndexX)
		{
			bool bDebugThisPixel = false;

			#if PIXELDEBUG == true
				if(eRenderMode == kRenderNormal)
				{
					if(nIndexX == nMouseX && nIndexY == nMouseY)
					{
						bDebugThisPixel = true;
					}
				}
			#endif

			//ask our camera for the color of this ray
			Vec3 vRayColor = GetColorForRay(bDebugThisPixel, ScreenCell, nRayCacheRayIndex, nCurrentCameraRayIndex, nMaxRayBounce);

			//clamp colors
			if(vRayColor.m_fX > 1.0f)
				vRayColor.m_fX = 1.0f;

			if(vRayColor.m_fY > 1.0f)
				vRayColor.m_fY = 1.0f;

			if(vRayColor.m_fZ > 1.0f)
				vRayColor.m_fZ = 1.0f;

			switch(eRenderMode)
			{
				case kRenderNormal:
				{
					//set the pixel color
					pCurrentPixel[0] = (unsigned char)(vRayColor.m_fZ * 255.0f); // the blue channel
					pCurrentPixel[1] = (unsigned char)(vRayColor.m_fY * 255.0f); // the green channel
					pCurrentPixel[2] = (unsigned char)(vRayColor.m_fX * 255.0f); // the red channel
					pCurrentPixel[3] = 255;									     // the alpha channel
					break;
				}
				case kRenderRed:
				{
					MRTFLOAT fMonoChrome = vRayColor.GetMonochromeFromRGB();

					//set the pixel color
					//pCurrentPixel[0] = 0; // the blue channel
					pCurrentPixel[1] = 0; // the green channel
					pCurrentPixel[2] = (unsigned char)(fMonoChrome * 255.0f); // the red channel
					pCurrentPixel[3] = 255;									     // the alpha channel
					break;
				}
				case kRenderBlue:
				{
					MRTFLOAT fMonoChrome = vRayColor.GetMonochromeFromRGB();

					//set the pixel color
					pCurrentPixel[0] = (unsigned char)(fMonoChrome * 255.0f); // the blue channel
					break;
				}
			}

			//move to the next ray / pixel
			pCurrentPixel += 4;
			nRayCacheRayIndex++;
			nCurrentCameraRayIndex++;
		}

		pCurrentRow += nWidth * 4;
		nCurrentRowCameraRayIndex += nScreenWidth;
	}

	//render our screen cell boundaries if we should
	if(m_bRenderCellBoundaries)
		ScreenCell.RenderCellBoundaries();

	//mark this cell as having just been
	if((ScreenCell.m_nFlags & CScreenCell::kFlags_GeometryClean) == 0)
	{
		ScreenCell.m_nFlags = CScreenCell::kFlags_JustCleanedGeometry;
	}
	else
	{
		ScreenCell.m_nFlags = CScreenCell::kFlags_JustCleanedPixels;
	}
}

void CCamera::MarkScreenCellAsPixelDirty(int nScreenCellIndex)
{
	//get the screen cell
	CScreenCell &ScreenCell = m_ScreenGrid.GetCell(nScreenCellIndex);

	//make sure the pixel clean flag is off
	ScreenCell.m_nFlags = ScreenCell.m_nFlags & (~CScreenCell::kFlags_PixelsClean);

	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->MarkScreenCellAsPixelDirty(nScreenCellIndex);
}

void CCamera::MarkScreenCellAsDirty(int nScreenCellIndex)
{
	//get the screen cell
	CScreenCell &ScreenCell = m_ScreenGrid.GetCell(nScreenCellIndex);

	//tell all the world cells that this screen cell has rays that visited that this screen cell no longer has rays visiting them.
	//when this screen cell is re-rendered, it will tell all the new cells that it visits that it visited them
	WorldCellList &VisitedWorldCells = ScreenCell.GetVisitedWorldCellList();
	for(WorldCellList::iterator it = VisitedWorldCells.begin(); it != VisitedWorldCells.end(); ++it)
	{
		(*it)->OnUnvisitedByScreenCell(&ScreenCell);
	}

	//clear out the visited world cells
	VisitedWorldCells.clear();

	//mark that this cell is completely dirty
	ScreenCell.m_nFlags = CScreenCell::kFlags_AllDirty;

	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->MarkScreenCellAsDirty(nScreenCellIndex);
}

void CCamera::PostRender(unsigned char *pPixelBuffer,int nPixelBufferWidth)
{
	m_ScreenGrid.PostRender(pPixelBuffer, nPixelBufferWidth);

	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->PostRender(NULL, 0);
}

void CCamera::SetModeOrthographic(MRTFLOAT fWidth /*= 1.0f*/, MRTFLOAT fHeight /*= 1.0f*/, MRTFLOAT fOffsetX /*= 0.0f*/, MRTFLOAT fOffsetY /*= 0.0f*/, MRTFLOAT fNear /*= 0.0f*/)
{
	m_bOrthographic = true;

	DirtyAllCachedData();

	m_fOrthoWidth = fWidth;
	m_fOrthoHeight = fHeight;
	m_fOrthoOffsetX = fOffsetX;
	m_fOrthoOffsetY = fOffsetY;
	m_fOrthoNear = fNear;

	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->SetModeOrthographic(fWidth, fHeight, fOffsetX, fOffsetY, fNear);
}

void CCamera::SetModeProjection()
{
	m_bOrthographic = false;

	DirtyAllCachedData();

	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->SetModeProjection();
}

bool CCamera::GetRay(int nPixelX, int nPixelY, Ray &CameraRay)
{
	//if the camera is dirty or the pixel is out of range, we can't get the ray so bail out!
	if(m_bDirty ||
	   nPixelX < 0 || nPixelX >= m_nRenderWidth ||
	   nPixelY < 0 || nPixelY >= m_nRenderHeight)
	{
		return false;
	}

	//set our camera ray
	CameraRay = m_pRays[nPixelY * m_nRenderWidth + nPixelX];

	//return success
	return true;
}

void CCamera::SetResolution(int nWidth, int nHeight)
{
	if(m_pOtherCameraFor3d)
		m_pOtherCameraFor3d->SetResolution(nWidth,nHeight);

	//throw out all cached data
	DirtyAllCachedData();

	//set up our new values
	m_nRenderWidth = nWidth;
	m_nRenderHeight = nHeight;
	m_fVerticalFOV = m_fFOV * ((MRTFLOAT)m_nRenderHeight / (MRTFLOAT)m_nRenderWidth);
	m_fScreenRectHeight = (m_fScreenRectWidth * (MRTFLOAT)m_nRenderHeight / (MRTFLOAT)m_nRenderWidth);

	//tell our screen grid
	m_ScreenGrid.SetResolution(nWidth,nHeight);
}

void CCamera::SetRedBlue3dMode(bool bRedBlue3dMode)
{
	if(m_bRedBlue3dMode == bRedBlue3dMode)
		return;

	m_bRedBlue3dMode = bRedBlue3dMode;

	if(m_bRedBlue3dMode)
	{
		//good for the 3d scenes
		//Vec3 vOffset(2.0f,0.0f,0.0f);

		//good for the snake game
		//Vec3 vOffset(0.075f,0.0f,0.0f);

		m_pOtherCameraFor3d = new CCamera(m_vCameraPos + m_v3dOffset,m_vCameraTarget,m_vCameraUp,m_nRenderWidth,m_nRenderHeight,m_fFOV * 180.0f / 3.14159f,m_fScreenRectWidth,m_nScreenGridCellCount,m_v3dOffset,true);
		m_pOtherCameraFor3d->SetScene(m_pScene);

		//make sure the camera gets an update
		m_pOtherCameraFor3d->Update();
	}
	else
	{
		delete m_pOtherCameraFor3d;
		m_pOtherCameraFor3d = NULL;
	}
}