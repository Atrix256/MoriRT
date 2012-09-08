/*======================================================

ObjectPool.h

templated class that handles object pools

======================================================*/

#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

//#include <vector>
#include "list.h"

template <class T, int nPageSize>
class CObjectPool
{
public:
	typedef std::vector<T *> ObjectPointerList;
	//typedef RTDynamicList<T *,nPageSize> ObjectPointerList;

	CObjectPool()
	{
	}

	~CObjectPool()
	{
		/*
		std::vector<T *>::iterator itr;

		for(itr = m_Pages.begin();
			itr != m_Pages.end();
			++itr)
		{
			delete[] (*itr);
		}
		*/

		for(unsigned int nIndex = 0; nIndex < m_Pages.size(); ++nIndex)
		{
			delete[] m_Pages[nIndex];
		}

		m_Pages.clear();
		m_pFreeObjects.clear();
	}

	T *Allocate()
	{
		//if we don't have any free objects, allocate a new page
		if(m_pFreeObjects.size() == 0)
			AllocatePage();

		//return the last free object
		T *pNode = *m_pFreeObjects.rbegin();
		m_pFreeObjects.pop_back();
		return pNode;
	}

	void Free(T *pNode)
	{
		m_pFreeObjects->push_back(pNode);
	}

	void Reserve(unsigned int nNumber)
	{
		while(m_pFreeObjects.size() < nNumber)
			AllocatePage();
	}

private:

	void AllocatePage()
	{
		T *pNewObjects = new T[nPageSize];
		m_Pages.push_back(pNewObjects);
		for(int nIndex = 0; nIndex < nPageSize; ++nIndex)
		{
			m_pFreeObjects.push_back(&pNewObjects[nIndex]);
		}
	}

	ObjectPointerList m_Pages;
	ObjectPointerList m_pFreeObjects;
};

#endif //OBJECTPOOL_H