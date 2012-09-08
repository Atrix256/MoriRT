/*======================================================

PlatformBase.cpp

This is the platform specific functionality class.  Derive your own class from this
and pass it to RayTracer::Initto implement functionality for your platform

======================================================*/

#include "PlatformBase.h"
#include "Camera.h"
#include "Texture.h"
#include "../PackedTextures.h"
#include "CommonUI/UIStack.h"

CPlatformBase::CPlatformBase()
:m_bLeftMouseDown(false)
{
}

void CPlatformBase::Render(CCamera *pCamera, int nScreenWidth, int nScreenHeight, int nMaxRayBounce)
{
	int nNumRenderCells = pCamera->GetNumRenderCells();

	for(int nIndex = 0; nIndex < nNumRenderCells; ++nIndex)
	{
		pCamera->RenderCell(nIndex, nMaxRayBounce,CCamera::kRenderNormal);
	}
}

void CPlatformBase::SetDebugTexture(CTexture *pTexture)
{
	unsigned int nA = 0xFFFF00FF;
	unsigned int nB = 0xFF00FF00;

	unsigned int pPixelData[] =
	{
		nA, nA, nA, nA, nA, nA, nA, nA,
		nA, nA, nA, nB, nB, nA, nA, nA,
		nA, nA, nB, nB, nB, nB, nA, nA,
		nA, nB, nA, nB, nB, nA, nB, nA,
		nA, nA, nA, nB, nB, nA, nA, nA,
		nA, nA, nA, nA, nA, nA, nA, nA,
		nA, nA, nA, nA, nA, nA, nA, nA,
		nA, nA, nA, nA, nA, nA, nA, nA,
	};

	pTexture->SetPixelData(8,8,pPixelData,8*sizeof(unsigned int));
}

void CPlatformBase::LoadTexture(CTexture *pTexture)
{
	unsigned char *pPixels;
	int nWidth;
	int nHeight;
	if(GetPackedTexture(pTexture->GetFileName(),pPixels,nWidth,nHeight))
	{
		pTexture->SetPixelData(nWidth,nHeight,(unsigned int *)pPixels,nWidth * sizeof(unsigned int));
	}
	else
	{
		SetDebugTexture(pTexture);
	}
}

/*
void CPlatformBase::MakeHTTPRequest(const char *szURL,HTTPCallback callback,void *pUserData,EHttpRequestType eRequestType)
{
	if(callback)
	{
		callback(pUserData,kHttpRequestCommsError,NULL);
	}
}*/