/*======================================================

List.h

Static and dynamic lists, so we can control allocations and such better than STL containers

======================================================*/

#ifndef LIST_H
#define LIST_H

#include <string.h>

//best used for simple types.  complex types that need to allocate memory and such in their equals operator etc probably are not handled well with this list
//you do get a destructor call when an item is removed though, but no construct / desconstruct called when objects move in the list or are moved around in memory due to resize

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Base List
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class RTBaseList
{
public:

	RTBaseList()
	:m_pDataItems(0)
	,m_nSize(0)
	,m_nCapacity(0)
	{
	}

	virtual ~RTBaseList()
	{
		clear();
	}

	int size() const {return m_nSize;}

	//adds a new item to the back
	void push_back(const T &newVal)
	{
		//make sure we have enough space for another value
		if(ensureRoomForMoreItems(1))
		{
			//put our new value at the end and increase our size
			m_pDataItems[m_nSize] = newVal;
			m_nSize++;
		}
	}

	//adds a new item in the specified place
	void insert(int nInsertIndex, const T &newVal, bool bInsertBefore=true)
	{
		//make sure we have enough space for another value
		if(ensureRoomForMoreItems(1))
		{
			if(bInsertBefore)
			{
				//shift everything right to make room for the new item
				memmove(&m_pDataItems[nInsertIndex + 1],&m_pDataItems[nInsertIndex],sizeof(T) * (m_nSize - nInsertIndex));

				//increase our list size
				m_nSize++;

				//put our new item into the list
				m_pDataItems[nInsertIndex] = newVal;
			}
			else
			{
				//put our new item into the list
				m_pDataItems[m_nSize] = newVal;

				//increase our list size
				m_nSize++;
			}
		}
	}

	T &operator [](int index)
	{
		//TODO: could put range checking here in debug if we wanted
		return m_pDataItems[index];
	}

	const T &operator [](int index) const
	{
		//TODO: could put range checking here in debug if we wanted
		return m_pDataItems[index];
	}

	//removes the last item from the list.  Does not return item (follows STD:VECTOR pattern)
	//also does not resize the array, it lets it stay at the high water mark to avoid memory churn
	void pop_back()
	{
		if(m_nSize > 0)
		{
			//explicit destructor call
			m_pDataItems[m_nSize-1].~T();
			m_nSize--;
		}
	}

	void clear()
	{
		//make sure destructors get called
		while(m_nSize > 0)
			pop_back();
	}

	T &last()
	{
		return m_pDataItems[m_nSize-1];
	}

	void erase(int nEraseIndex, bool bPreserveOrder = false)
	{
		//explicit destructor call
		m_pDataItems[nEraseIndex].~T();

		//if we care about order
		if(bPreserveOrder)
		{
			//shift everything left 1 and remove the last item in the list
			memmove(&m_pDataItems[nEraseIndex],&m_pDataItems[nEraseIndex + 1],  sizeof(T) * (m_nSize - nEraseIndex - 1));
			m_nSize--;
		}
		//if we dont care about order
		else
		{
			//set the index to the last item in the list and remove the last item in the list
			m_pDataItems[nEraseIndex] = last();
			m_nSize--;
		}
	}

	//inserts a new value into the list, making sure to put it into the correct place for sorting, making sure it isn't already in there
	//this works so long as the list is already sorted low to high and that type has these operators are defined for your type: < != =
	//not the fastest function in the world but hopefully not so bad.
	//a binary search could be used to find the insert location, and making room for the new item could be better.
	void insertUniqueSortedLowHigh(const T &newVal)
	{
		//find where this value belongs in the list
		int nInsertLocation = 0;
		while(nInsertLocation < m_nSize && m_pDataItems[nInsertLocation] < newVal)
			nInsertLocation++;

		//if we found a place for it
		if(nInsertLocation < m_nSize)
		{
			//if we don't already have this value there
			if(m_pDataItems[nInsertLocation] != newVal)
			{
				insert(nInsertLocation,newVal);
			}
		}
		//else we ran out of list looking for the place
		else
		{
			push_back(newVal);
		}
	}

	//this assumes the current list and the other list both only have uniques and that they are both sorted low to high
	//if those assumptions are true, this function makes a single list out of both lists that contains no duplicates
	//and is sorted low to high
	void combineUniqueSortedLists(const RTBaseList &otherList)
	{
		//TODO: this, more efficiently!  Something like the merge part of a merge sort, but making sure to cull duplicates
		for(int index = 0; index < otherList.size(); ++index)
			insertUniqueSortedLowHigh(otherList[index]);
	}

	void copy(const RTBaseList &otherList)
	{
		//make sure we are empty
		clear();

		//make sure we have enough room to hold the contents of the other list
		if(ensureRoomForMoreItems(otherList.size()))
		{
			//a memmove could be done here instead but doing it this way, if someone passes the wrong list type in,
			//it will throw a compile time error so ::shrug::  If this is a performance problem feel free to switch it
			//to the other way if you'd like.
			for(int index = 0;index<otherList.size();index++)
			{
				push_back(otherList[index]);
			}
		}
	}

protected:
	T *m_pDataItems;
	int m_nSize;
	int m_nCapacity;


	//private interface
	virtual bool ensureRoomForMoreItems(int nCount) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Dynamic List
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T, int nPageSize=10>
class RTDynamicList : public RTBaseList<T>
{
public:
	RTDynamicList()
	{
		this->m_pDataItems = 0;
		this->m_nCapacity = 0;
		this->m_nSize = 0;
	}

	virtual ~RTDynamicList()
	{
		delete[] this->m_pDataItems;
	}

protected:
	virtual bool ensureRoomForMoreItems(int nCount)
	{
		//if we don't have enough space
		if((this->m_nSize + nCount) > this->m_nCapacity)
		{
			//calculate how many new pages we need to add
			int nNumItemsToAdd = (this->m_nSize + nCount) - this->m_nCapacity;
			int nNumNewPages = nNumItemsToAdd / nPageSize;
			if(nNumItemsToAdd % nPageSize)
				nNumNewPages++;

			//update our capacity
			this->m_nCapacity += nNumNewPages * nPageSize;

			//reallocate our memory to the larger size
			//note: realloc helps prevent memory churn instead of an alloc and copy since it might be able to just get larger
			//realloc of course also copies all the old data to the new memory if new memory was allocated
			this->m_pDataItems = (T *)realloc(this->m_pDataItems,this->m_nCapacity * sizeof(T));
		}

		//always room for one more
		return true;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Fixed List
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T, int nCapacity>
class RTFixedList : public RTBaseList<T>
{
public:
	RTFixedList()
	{
		this->m_pDataItems = this->m_pFixedDataItems;
		this->m_nCapacity = nCapacity;
		this->m_nSize = 0;
	}

	virtual ~RTFixedList() {}

protected:

	virtual bool ensureRoomForMoreItems(int nCount)
	{
		AssertFatal((this->m_nSize + nCount) <= this->m_nCapacity, "RTFixedList ran out of space!");
		return (this->m_nSize + nCount) <= this->m_nCapacity;
	}

	T m_pFixedDataItems[nCapacity];
};


#endif //LIST_H