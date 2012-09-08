/*======================================================

ScreenGrid.cpp

The class is a screen space regular grid that holds ScreenCells, which are individual units of cacheable work for the raytracer.

======================================================*/

#include "ScreenGrid.h"
#include "ScreenCell.h"
#include "WorldCell.h"
#include "Camera.h"

CScreenGrid::CScreenGrid(CCamera *pParentCamera, int nScreenGridCellCount, int nScreenWidth, int nScreenHeight)
:m_nScreenGridCellCount(nScreenGridCellCount)
,m_pParentCamera(pParentCamera)
{
	SetResolution(nScreenWidth,nScreenHeight);
}

CScreenGrid::~CScreenGrid()
{
	//free the screen cells
	for(ScreenCellList::iterator it = ScreenCells.begin(); it != ScreenCells.end(); ++it)
		delete *it;

	ScreenCells.empty();
}

void CScreenGrid::PostRender(unsigned char *pPixelBuffer,int nPixelBufferWidth)
{
	bool bRenderCellBoundaries = m_pParentCamera->GetRenderCellBoundaries();

	for(ScreenCellList::iterator it = ScreenCells.begin(); it != ScreenCells.end(); ++it)
	{
		//if this cell was just cleaned this frame
		CScreenCell &ScreenCell = *(*it);
		if(ScreenCell.m_nFlags & CScreenCell::kFlags_JustCleanedGeometry)
		{
			//get the list of world cells that this screen cell has rays that visited
			WorldCellList &VisitedWorldCellList = ScreenCell.GetVisitedWorldCellList();

			//for each world cell, let them know that we visited
			for(WorldCellList::iterator it = VisitedWorldCellList.begin(); it != VisitedWorldCellList.end(); ++it)
			{
				(*it)->OnVisitedByScreenCell(&ScreenCell);
			}

			//mark this cell as being all clean now
			ScreenCell.m_nFlags = CScreenCell::kFlags_AllClean;

			//copy the screen cell to the screen
			ScreenCell.CopyContentsToBuffer(pPixelBuffer,nPixelBufferWidth);
		}
		else if(ScreenCell.m_nFlags & CScreenCell::kFlags_JustCleanedPixels)
		{
			//mark this cell as being all clean now
			ScreenCell.m_nFlags = CScreenCell::kFlags_AllClean;

			//copy the screen cell to the screen
			ScreenCell.CopyContentsToBuffer(pPixelBuffer,nPixelBufferWidth);
		}
		else if(bRenderCellBoundaries)
		{
			//copy the screen cell to the screen
			ScreenCell.CopyContentsToBuffer(pPixelBuffer,nPixelBufferWidth);
		}
	}
}

void CScreenGrid::SetResolution(int nWidth, int nHeight)
{
	//clear any existing screen cells
	while(!ScreenCells.empty())
	{
		delete *ScreenCells.rbegin();
		ScreenCells.pop_back();
	}

	m_nScreenWidth = nWidth;
	m_nScreenHeight = nHeight;

	int nStartX, nEndX;
	int nStartY, nEndY;
	int nScreenCellIndex = 0;

	for(int nIndexY = 0; nIndexY < m_nScreenGridCellCount; ++nIndexY)
	{
		nStartY = ((nIndexY * m_nScreenHeight) / m_nScreenGridCellCount);

		if(nIndexY == m_nScreenGridCellCount - 1)
		{
			nEndY = m_nScreenHeight;
		}
		else
		{
			nEndY = (((nIndexY + 1) * m_nScreenHeight) / m_nScreenGridCellCount);
		}

		for(int nIndexX = 0; nIndexX < m_nScreenGridCellCount; ++nIndexX)
		{
			nStartX = ((nIndexX * m_nScreenWidth) / m_nScreenGridCellCount);

			if(nStartX == m_nScreenGridCellCount - 1)
			{
				nEndX = m_nScreenWidth;
			}
			else
			{
				nEndX = (((nIndexX + 1) * m_nScreenWidth) / m_nScreenGridCellCount);
			}

			ScreenCells.push_back(new CScreenCell(m_pParentCamera,nScreenCellIndex, nStartX, nStartY, nEndX, nEndY));

			nScreenCellIndex++;
		}
	}
}