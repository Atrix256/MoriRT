/*======================================================

WorldCell.h

Each cell contains a list of the primitives inside of it as well
as a list of screen grid cells that have rays that go through the world cell.

======================================================*/

#ifndef WORLDCELL_H
#define WORLDCELL_H

#include <algorithm>
#include <vector>

//forward declarations
class CPrimitiveBase;
class CScreenCell;

//typedef
typedef std::vector<CPrimitiveBase *> PrimitiveList;
typedef std::vector<CScreenCell *> ScreenCellVisitorList;

class CWorldCell
{
public:
	CWorldCell()
	:m_nCellIndex(-1)
	{
	}

	PrimitiveList *GetPrimitiveList() {return &m_Primitives;}

	void OnVisitedByScreenCell(CScreenCell *pScreenCell)
	{
		//it's assumed that this function is only sent unique values (ie this list will be unique because it's only told about uniques)
		m_ScreenCellVisitors.push_back(pScreenCell);
	}

	void OnUnvisitedByScreenCell(CScreenCell *pScreenCell)
	{
		ScreenCellVisitorList::iterator it = std::find(m_ScreenCellVisitors.begin(), m_ScreenCellVisitors.end(), pScreenCell);

		if(it != m_ScreenCellVisitors.end())
		{
			m_ScreenCellVisitors.erase(it);
		}
	}

	ScreenCellVisitorList &GetScreenCellVisitors()
	{
		return m_ScreenCellVisitors;
	}

	void SetCellIndex(int nIndex)
	{
		m_nCellIndex = nIndex;
	}

private:
	PrimitiveList m_Primitives;
	ScreenCellVisitorList m_ScreenCellVisitors;
	int m_nCellIndex;
};

#endif //WORLDCELL_H
