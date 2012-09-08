/*======================================================

Texture.cpp

Holds and manages texture data

======================================================*/

#include "Texture.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

CTexture::CTexture(const char *szFileName)
:m_szFileName(szFileName)
,m_nRefCount(1)
,m_nWidth(0)
,m_nHeight(0)
,m_pPixelData(0)
{
}

CTexture::~CTexture()
{
	delete[] m_pPixelData;
}

void CTexture::SetPixelData(int nWidth, int nHeight, unsigned int *pPixelData, int nRowPitch)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_pPixelData = new unsigned int[m_nWidth * m_nHeight];

	unsigned char *pSrc = (unsigned char *)pPixelData;
	unsigned char *pDest = (unsigned char *)m_pPixelData;
	for(int nIndex = 0; nIndex < nHeight; ++nIndex)
	{
		memcpy(pDest,pSrc,m_nWidth * sizeof(unsigned int));

		pSrc += nRowPitch;
		pDest += m_nWidth * sizeof(unsigned int);
	}
}

Vec3 CTexture::GetTexelBilinear(MRTFLOAT a_U, MRTFLOAT a_V,MRTFLOAT *pAlphaValue /*= 0*/)
{
	if(a_U < 0.0f)
	{
		a_U = fmodf(a_U,1.0f) + 1.0f;
	}

	if(a_V < 0.0f)
	{
		a_V = fmodf(a_V,1.0f) + 1.0f;
	}

	// fetch a bilinearly filtered texel
	MRTFLOAT fu = (a_U) * (float)m_nWidth;
	MRTFLOAT fv = (a_V) * (float)m_nHeight;
	int u1 = ((int)fu) % m_nWidth;
	int v1 = ((int)fv) % m_nHeight;
	int u2 = (u1 + 1) % m_nWidth;
	int v2 = (v1 + 1) % m_nHeight;
	// calculate fractional parts of u and v


	MRTFLOAT fracu = fu - floorf( fu );
	MRTFLOAT fracv = fv - floorf( fv );
	// calculate weight factors
	MRTFLOAT w1 = (1.0f - fracu) * (1.0f - fracv);
	MRTFLOAT w2 = fracu * (1.0f - fracv);
	MRTFLOAT w3 = (1.0f - fracu) * fracv;
	MRTFLOAT w4 = fracu *  fracv;
	// fetch four texels

	Vec3 c1;
	Vec3 c2;
	Vec3 c3;
	Vec3 c4;
	c1.SetColorFromBGRA(m_pPixelData[u1 + v1 * m_nWidth]);
	c2.SetColorFromBGRA(m_pPixelData[u2 + v1 * m_nWidth]);
	c3.SetColorFromBGRA(m_pPixelData[u1 + v2 * m_nWidth]);
	c4.SetColorFromBGRA(m_pPixelData[u2 + v2 * m_nWidth]);

	//if the caller wants the alpha too
	if(pAlphaValue)
	{
		MRTFLOAT a1 = ((MRTFLOAT)(float)((m_pPixelData[u1 + v1 * m_nWidth] & 0xFF000000) >> 24)) / 255.0f;
		MRTFLOAT a2 = ((MRTFLOAT)(float)((m_pPixelData[u2 + v1 * m_nWidth] & 0xFF000000) >> 24)) / 255.0f;
		MRTFLOAT a3 = ((MRTFLOAT)(float)((m_pPixelData[u1 + v2 * m_nWidth] & 0xFF000000) >> 24)) / 255.0f;
		MRTFLOAT a4 = ((MRTFLOAT)(float)((m_pPixelData[u2 + v2 * m_nWidth] & 0xFF000000) >> 24)) / 255.0f;

		*pAlphaValue = a1 * w1 + a2 * w2 + a3 * w3 + a4 * w4;
	}

	// scale and sum the four colors
	return c1 * w1 + c2 * w2 + c3 * w3 + c4 * w4;
}

Vec3 CTexture::GetTexel(MRTFLOAT a_U, MRTFLOAT a_V,MRTFLOAT *pAlphaValue /*= 0*/)
{
	if(a_U < 0.0f)
	{
		a_U = fmodf(a_U,1.0f) + 1.0f;
	}

	if(a_V < 0.0f)
	{
		a_V = fmodf(a_V,1.0f) + 1.0f;
	}

	// fetch a bilinearly filtered texel
	MRTFLOAT fu = (a_U) * (float)m_nWidth;
	MRTFLOAT fv = (a_V) * (float)m_nHeight;
	int u1 = ((int)fu) % m_nWidth;
	int v1 = ((int)fv) % m_nHeight;

	unsigned int nColor = m_pPixelData[u1 + v1 * m_nWidth];

	Vec3 c1;
	c1.SetColorFromBGRA(nColor);

	//if the caller wants the alpha too
	if(pAlphaValue)
	{
		*pAlphaValue = ((MRTFLOAT)(float)((nColor & 0xFF000000) >> 24)) / 255.0f;
	}

	return c1;
}