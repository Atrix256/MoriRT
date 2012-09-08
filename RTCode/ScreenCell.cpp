/*======================================================

ScreenCell.cpp

A cell in the screen grid.  This is a single unit of cacheable work for the raytracer.

======================================================*/

#include "ScreenCell.h"
#include <algorithm>

CScreenCell::CScreenCell(CCamera *pParentCamera, int nScreenCellIndex, int nStartPixelX, int nStartPixelY, int nEndPixelX, int nEndPixelY)
:m_RayCache(nEndPixelX - nStartPixelX, nEndPixelY - nStartPixelY)
,m_nFlags(kFlags_AllDirty)
,m_nScreenCellIndex(nScreenCellIndex)
,m_nStartPixelX(nStartPixelX)
,m_nStartPixelY(nStartPixelY)
,m_nEndPixelX(nEndPixelX)
,m_nEndPixelY(nEndPixelY)
,m_pParentCamera(pParentCamera)
,m_pScreenGridPixels(NULL)
{
	int nWidth = (nEndPixelX - nStartPixelX);
	int nHeight = (nEndPixelY - nStartPixelY);
	m_pScreenGridPixels = new unsigned char[nWidth * nHeight *4];
}

CScreenCell::~CScreenCell()
{
	delete[] m_pScreenGridPixels;
}

void CScreenCell::RenderCellBoundaries()
{
	//if the cell is completely dirty
	if(m_nFlags == CScreenCell::kFlags_AllDirty)
		return;

	unsigned char nColor[4];
	if(m_nFlags & CScreenCell::kFlags_PixelsClean)
	{
		nColor[0] = 0;
		nColor[1] = 165;
		nColor[2] = 255;
		nColor[3] = 255;
	}
	else
	{
		nColor[0] = 165;
		nColor[1] = 0;
		nColor[2] = 165;
		nColor[3] = 255;
	}

	int nMargin = 1;
	int nStartX = nMargin;
	int nStartY = nMargin;
	int nEndX = (GetEndPixelX() - GetStartPixelX()) - nMargin;
	int nEndY = (GetEndPixelY() - GetStartPixelY()) - nMargin;

	int nWidth = nEndX - nStartX;
	int nHeight = nEndY - nStartY;

	int nRenderWidth = (GetEndPixelX() - GetStartPixelX());

	unsigned char *pUpperLeft = &m_pScreenGridPixels[(nStartY * nRenderWidth + nStartX) * 4];
	unsigned char *pLowerLeft = &m_pScreenGridPixels[((nEndY - 1) * nRenderWidth + nStartX) * 4];

	for(int nIndex = 0; nIndex < nWidth; ++nIndex)
	{
		pUpperLeft[0] = nColor[0];
		pUpperLeft[1] = nColor[1];
		pUpperLeft[2] = nColor[2];
		pUpperLeft[3] = nColor[3];

		pUpperLeft += 4;

		pLowerLeft[0] = nColor[0];
		pLowerLeft[1] = nColor[1];
		pLowerLeft[2] = nColor[2];
		pLowerLeft[3] = nColor[3];

		pLowerLeft += 4;
	}

	pUpperLeft = &m_pScreenGridPixels[(nStartY * nRenderWidth + nStartX) * 4];
	unsigned char *pUpperRight = &m_pScreenGridPixels[(nStartY * nRenderWidth + (nEndX - 1)) * 4];

	for(int nIndex = 0; nIndex < nHeight; ++nIndex)
	{
		pUpperLeft[0] = nColor[0];
		pUpperLeft[1] = nColor[1];
		pUpperLeft[2] = nColor[2];
		pUpperLeft[3] = nColor[3];

		pUpperLeft += nRenderWidth * 4;

		pUpperRight[0] = nColor[0];
		pUpperRight[1] = nColor[1];
		pUpperRight[2] = nColor[2];
		pUpperRight[3] = nColor[3];

		pUpperRight += nRenderWidth * 4;
	}
}

void CScreenCell::OnVisitWorldCell(CWorldCell *pWorldCell)
{
	//add this if it's not already in the list
	if(std::find(m_VisitedWorldCellList.begin(),m_VisitedWorldCellList.end(),pWorldCell) == m_VisitedWorldCellList.end())
	{
		m_VisitedWorldCellList.push_back(pWorldCell);
	}
}

unsigned char *CScreenCell::GetPixelBuffer(int &nWidth, int &nHeight)
{
	nWidth = (m_nEndPixelX - m_nStartPixelX);
	nHeight = (m_nEndPixelY - m_nStartPixelY);
	return m_pScreenGridPixels;
}

void CScreenCell::CopyContentsToBuffer(unsigned char *pPixelBuffer,int nPixelBufferWidth)
{
	if(!pPixelBuffer)
		return;

	unsigned char *pCurrentDestRow = &pPixelBuffer[ (m_nStartPixelY * nPixelBufferWidth + m_nStartPixelX) * 4 ];
	unsigned char *pCurrentSrcRow = m_pScreenGridPixels;

	int nSrcWidth = (m_nEndPixelX - m_nStartPixelX);
	int nSrcHeight = (m_nEndPixelY - m_nStartPixelY);

	int nDestPitch = nPixelBufferWidth * 4;
	int nSrcPitch = nSrcWidth * 4;

	for(int nIndexY = 0; nIndexY < nSrcHeight; ++nIndexY)
	{
		memcpy(pCurrentDestRow,pCurrentSrcRow,nSrcPitch);

		pCurrentDestRow += nDestPitch;
		pCurrentSrcRow += nSrcPitch;
	}

}