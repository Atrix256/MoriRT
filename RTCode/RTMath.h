/*======================================================

RTMath.h

General math functions

======================================================*/

#include <math.h>
#include <float.h>
#include "Defines.h"

inline MRTFLOAT RoundNumberUpToMultipleOf(MRTFLOAT fValue, MRTFLOAT fMultiple)
{
	//there's gotta be a better way to do this!  Gotta think about it more at some point
	MRTFLOAT fNewValue = fMultiple != 0.0f ? ceilf(fValue / fMultiple) * fMultiple : fValue;
//	if(fNewValue == fValue)
//	{
//		fNewValue += fMultiple;
//	}

	return fNewValue;
}

inline MRTFLOAT RoundNumberDownToMultipleOf(MRTFLOAT fValue, MRTFLOAT fMultiple)
{
	//there's gotta be a better way to do this!  Gotta think about it more at some point
	MRTFLOAT fNewValue = fMultiple != 0.0f ? floorf(fValue / fMultiple) * fMultiple : fValue;
//	if(fNewValue == fValue)
//	{
//		fNewValue -= fMultiple;
//	}

	return fNewValue;
}

inline MRTFLOAT CalculateTimeToNextMultipleOf(MRTFLOAT fValue, MRTFLOAT fMultiple, MRTFLOAT fValueIncrement)
{
	//TODO: try fmodf or something instead?  check w/ sleepy
	MRTFLOAT fRet;

	if(fValueIncrement < 0.0f)
	{
		MRTFLOAT fNextValue = RoundNumberDownToMultipleOf(fValue, fMultiple);

		if(fValue == fNextValue)
		{
			fRet = -fMultiple / fValueIncrement;
		}
		else
		{
			fRet = (fNextValue - fValue) / fValueIncrement;
		}

		/*
		if(fRet < 0.0001f)
		{
			fRet += fMultiple / fValueIncrement;
		}*/
	}
	else if(fValueIncrement > 0.0f)
	{
		MRTFLOAT fNextValue = RoundNumberUpToMultipleOf(fValue, fMultiple);

		if(fValue == fNextValue)
		{
			fRet = fMultiple / fValueIncrement;
		}
		else
		{
			fRet = (fNextValue - fValue) / fValueIncrement;
		}

		/*
		if(fRet < 0.0001f)
		{
			fRet += fMultiple / fValueIncrement;
		}*/
	}
	//else it's infinite, so just return a big number
	else
	{
		fRet = MRTFLOAT_MAX;
	}

	return fRet;
}