/*======================================================

ScreenGrid.h

The class is a screen space regular grid that holds ScreenCells, which are individual units of cacheable work for the raytracer.

======================================================*/

#ifndef SCREENGRID_H
#define SCREENGRID_H

#include <vector>

//forward declarations
class CScreenCell;
class CCamera;

//typedefs
typedef std::vector<CScreenCell *> ScreenCellList;

class CScreenGrid
{
public:
	CScreenGrid(CCamera *pParentCamera, int nScreenGridCellCount, int nScreenWidth, int nScreenHeight);
	~CScreenGrid();

	CScreenCell &GetCell(int nIndex) const {return *ScreenCells[nIndex];}

	void PostRender(unsigned char *pPixelBuffer,int nPixelBufferWidth);

	void SetResolution(int nWidth, int nHeight);

private:

	int m_nScreenGridCellCount; //per axis

	int m_nScreenWidth;
	int m_nScreenHeight;

	CCamera *m_pParentCamera;

	ScreenCellList ScreenCells;
};

#endif //SCREENGRID_H