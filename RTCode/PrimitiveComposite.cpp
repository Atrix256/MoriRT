/*======================================================

PrimitiveComposite.cpp

A primitive which is defined by 2 child primitives and an operation of how to combine them.

A child primitive can also be another composite primitive

======================================================*/

#include "PrimitiveComposite.h"
#include "AABB.h"

bool CPrimitiveComposite::InternalRayIntersects(const Ray &ray, SCollisionInfo &CollisionInfo, MRTFLOAT fMaxDistance)
{
	//get the list of collisions this ray has with the left child
	CompositeRayCollisionList LeftRayCollisionList;
	m_pLeftChild->GetRayCollisionList(ray,LeftRayCollisionList);

	//if we have no collisions with the left shape, we can bail out under certain operations knowing that we missed the shape
	if(LeftRayCollisionList.empty())
	{
		if(m_eOperation == kAND ||
		   m_eOperation == kSubtract)
		{
			return false;
		}
	}

	//get the list of collisions this ray has with the right child
	CompositeRayCollisionList RightRayCollisionList;
	m_pRightChild->GetRayCollisionList(ray,RightRayCollisionList);

	//if we have no collisions with the right shape, we can bail out under certain operations knowing that we missed the shape
	if(LeftRayCollisionList.empty())
	{
		if(m_eOperation == kAND)
		{
			return false;
		}
	}

	//calculate our starting states by looking at our first collision to know whether we are inside or not
	bool bRayInsideLeft = false;
	if(!LeftRayCollisionList.empty())
	{
		bRayInsideLeft = (*LeftRayCollisionList.rbegin()).m_CollisionInfo.m_bFromInside;
	}

	bool bRayInsideRight = false;
	if(!RightRayCollisionList.empty())
	{
		bRayInsideRight = (*RightRayCollisionList.rbegin()).m_CollisionInfo.m_bFromInside;
	}

	//loop through our data
	while(!LeftRayCollisionList.empty() || !RightRayCollisionList.empty())
	{
		bool bLeftIsWinner = false;

		//get our left collision item
		SCompositeRayCollisionItem *pLeft = 0;
		if(!LeftRayCollisionList.empty())
		{
			pLeft = &(*LeftRayCollisionList.rbegin());
		}
		//if we don't have it, we know the right side is the winner
		else
		{
			bLeftIsWinner = false;
		}

		//get our right collision item
		SCompositeRayCollisionItem *pRight = 0;
		if(!RightRayCollisionList.empty())
		{
			pRight = &(*RightRayCollisionList.rbegin());
		}
		//if we don't have it, we know the left side is the winner
		else
		{
			bLeftIsWinner = true;
		}

		//if we have both a left and a right, we need to see who is closer in time to determine the winner
		//else we already determined the winner
		if(pLeft && pRight)
		{
			bLeftIsWinner = (pLeft->m_CollisionInfo.m_fCollisionTime < pRight->m_CollisionInfo.m_fCollisionTime);
		}

		//calculate whether we were in the shape previously or not
		bool bWasInShape;
		switch(m_eOperation)
		{
			case kAND:
			{
				bWasInShape = bRayInsideLeft && bRayInsideRight;
				break;
			}
			case kXOR:
			{
				bWasInShape = bRayInsideLeft ^ bRayInsideRight;
				break;
			}
			case kOR:
			//case kAdd:
			{
				bWasInShape = bRayInsideLeft || bRayInsideRight;
				break;
			}
			case kSubtract:
			{
				if(bRayInsideLeft)
				{
					if(bRayInsideRight)
					{
						bWasInShape = false;
					}
					else
					{
						bWasInShape = true;
					}
				}
				else
				{
					bWasInShape = false;
				}
				break;
			}
		}

		//update our per shape bools as necesary
		SCompositeRayCollisionItem *pWinner = 0;
		if(bLeftIsWinner)
		{
			pWinner = pLeft;
			bRayInsideLeft = !pWinner->m_CollisionInfo.m_bFromInside;
		}
		else
		{
			pWinner = pRight;
			bRayInsideRight = !pWinner->m_CollisionInfo.m_bFromInside;
		}

		//calculate whether we are in the shape now or not
		bool bIsInShape;
		switch(m_eOperation)
		{
			case kAND:
			{
				bIsInShape = bRayInsideLeft && bRayInsideRight;
				break;
			}
			case kXOR:
			{
				bIsInShape = bRayInsideLeft ^ bRayInsideRight;
				break;
			}
			case kOR:
			//case kAdd:
			{
				bIsInShape = bRayInsideLeft || bRayInsideRight;
				break;
			}
			case kSubtract:
			{
				if(bRayInsideLeft)
				{
					if(bRayInsideRight)
					{
						bIsInShape = false;
					}
					else
					{
						bIsInShape = true;
					}
				}
				else
				{
					bIsInShape = false;
				}
				break;
			}
		}

		//if we hit a real boundary
		if(bIsInShape != bWasInShape)
		{
			//if we are past time zero, we are done
			if(pWinner->m_CollisionInfo.m_fCollisionTime > 0.0f)
			{
				//enforce max distance
				if(fMaxDistance < 0.0f || pWinner->m_CollisionInfo.m_fCollisionTime <= fMaxDistance)
				{
					//set our initial collision info
					CollisionInfo = pWinner->m_CollisionInfo;

					//flip the normal and inside flag if we need to
					if(bWasInShape != CollisionInfo.m_bFromInside)
					{
						CollisionInfo.m_bFromInside = !CollisionInfo.m_bFromInside;
						CollisionInfo.m_vSurfaceNormal *= -1.0f;
					}

					return true;
				}

				//if we got here, the max distance was exceeded, bail out as no collision
				return false;
			}
		}

		//remove the winner from the list
		if(bLeftIsWinner)
		{
			LeftRayCollisionList.pop_back();
		}
		else
		{
			RightRayCollisionList.pop_back();
		}
	}

	//if we got here, we didn't hit a boundary of the shape
	return false;
}

AABB CPrimitiveComposite::GetAABB() const
{
	AABB aRet;
	aRet = m_pLeftChild->GetAABB();
	aRet.AddAABB(m_pRightChild->GetAABB());
	return aRet;
}