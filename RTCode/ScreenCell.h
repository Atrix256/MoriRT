/*======================================================

ScreenCell.h

A cell in the screen grid.  This is a single unit of cacheable work for the raytracer.

======================================================*/

#ifndef SCREENCELL_H
#define SCREENCELL_H

#include <vector>
#include "RayCache.h"

//forward declarations
class CWorldCell;
class CCamera;

//typedefs
typedef std::vector<CWorldCell *> WorldCellList;

class CScreenCell
{
public:
	CScreenCell(CCamera *pParentCamera, int nScreenCellIndex, int nStartPixelX, int nStartPixelY, int nEndPixelX, int nEndPixelY);
	~CScreenCell();

	void RenderCellBoundaries();

	int GetStartPixelX() const {return m_nStartPixelX;}
	int GetStartPixelY() const {return m_nStartPixelY;}
	int GetEndPixelX() const {return m_nEndPixelX;}
	int GetEndPixelY() const {return m_nEndPixelY;}

	CRayCache m_RayCache;
	unsigned char m_nFlags;

	enum EFlags
	{
		kFlags_PixelsClean = 1,
		kFlags_GeometryClean = 2,
		kFlags_JustCleanedGeometry = 4,
		kFlags_JustCleanedPixels = 8,

		kFlags_AllClean = kFlags_PixelsClean | kFlags_GeometryClean,
		kFlags_AllDirty = 0
	};

	void OnVisitWorldCell(CWorldCell *pWorldCell);

	WorldCellList &GetVisitedWorldCellList() {return m_VisitedWorldCellList;}

	int GetScreenCellIndex() const {return m_nScreenCellIndex;}

	CCamera *GetParentCamera() {return m_pParentCamera;}

	unsigned char *GetPixelBuffer(int &nWidth, int &nHeight);

	void CopyContentsToBuffer(unsigned char *pPixelBuffer,int nPixelBufferWidth);

private:

	int m_nScreenCellIndex;
	int m_nStartPixelX;
	int m_nStartPixelY;
	int m_nEndPixelX;
	int m_nEndPixelY;

	CCamera *m_pParentCamera;

	WorldCellList m_VisitedWorldCellList;

	unsigned char *m_pScreenGridPixels;
};

#endif //SCREENCELL_H