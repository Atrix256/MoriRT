/*======================================================

Matrix.h

A matrix class (very minimal currently because it isn't needed for much)

======================================================*/

#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#include "Vec3.h"

class Matrix
{
public:
	Matrix()
	{
		Identity();
	}

	void Identity()
	{
		for(int nIndexY = 0; nIndexY < 3; ++nIndexY)
		{
			for(int nIndexX = 0; nIndexX < 3; ++nIndexX)
			{
				m_fValues[nIndexX][nIndexY] = (nIndexX == nIndexY) ? 1.0f : 0.0f;
			}
		}
	}

	//from http://www.fastgraph.com/makegames/3drotation/
	void RotationX(MRTFLOAT fAngleRadians)
	{
		Identity();
		
		m_fValues[1][1] =  cosf(fAngleRadians);
		m_fValues[2][1] = -sinf(fAngleRadians);
		m_fValues[1][2] =  sinf(fAngleRadians);
		m_fValues[2][2] =  cosf(fAngleRadians);
	}

	void RotationY(MRTFLOAT fAngleRadians)
	{
		Identity();
		
		m_fValues[0][0] =  cosf(fAngleRadians);
		m_fValues[2][0] =  sinf(fAngleRadians);
		m_fValues[0][2] = -sinf(fAngleRadians);
		m_fValues[2][2] =  cosf(fAngleRadians);
	}

	void RotationZ(MRTFLOAT fAngleRadians)
	{
		Identity();
		
		m_fValues[0][0] =  cosf(fAngleRadians);
		m_fValues[1][0] = -sinf(fAngleRadians);
		m_fValues[0][1] =  sinf(fAngleRadians);
		m_fValues[1][1] =  cosf(fAngleRadians);
	}

	Matrix operator * (const Matrix &otherMatrix)
	{
		Matrix mRet;

		for(int nIndexY = 0; nIndexY < 3; ++nIndexY)
		{
			for(int nIndexX = 0; nIndexX < 3; ++nIndexX)
			{
				mRet.m_fValues[nIndexX][nIndexY] = 0.0f;

				for(int nIndex = 0; nIndex < 3; ++nIndex)
				{
					mRet.m_fValues[nIndexX][nIndexY] += m_fValues[nIndex][nIndexY] * otherMatrix.m_fValues[nIndexX][nIndex];
				}
			}
		}

		return mRet;
	}

	Vec3 operator * (const Vec3 &vOtherVec)
	{
		Vec3 vRet;

		for(int nIndexY = 0; nIndexY < 3; ++nIndexY)
		{
			vRet[nIndexY] = 0.0f;

			for(int nIndex = 0; nIndex < 3; ++nIndex)
			{
				vRet[nIndexY] = m_fValues[nIndex][nIndexY] * vOtherVec[nIndex];
			}
		}

		return vRet;
	}

	MRTFLOAT m_fValues[3][3];
};

#endif //MATRIX_H