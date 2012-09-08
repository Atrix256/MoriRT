/*======================================================

RTMath.cpp

General math functions

======================================================*/

#include "RTMath.h"
#include <math.h>
#include <float.h>

/*
MRTFLOAT RoundNumberUpToMultipleOf(MRTFLOAT fValue, MRTFLOAT fMultiple)
{
	return ceilf(fValue / fMultiple) * fMultiple;
}

MRTFLOAT RoundNumberDownToMultipleOf(MRTFLOAT fValue, MRTFLOAT fMultiple)
{
	return floorf(fValue / fMultiple) * fMultiple;
}

MRTFLOAT CalculateTimeToNextMultipleOf(MRTFLOAT fValue, MRTFLOAT fMultiple, MRTFLOAT fValueIncrement)
{
	MRTFLOAT fRet;

	if(fValueIncrement < 0)
	{
		fRet = (RoundNumberDownToMultipleOf(fValue, fMultiple) - fValue) / fValueIncrement;
	}
	else if(fValueIncrement > 0)
	{
		fRet = (RoundNumberUpToMultipleOf(fValue, fMultiple) - fValue) / fValueIncrement;
	}
	//else it's infinite, so just return a big number
	else
	{
		fRet = MRTFLOAT_MAX;
	}

	return fRet;
}
*/