/*======================================================

Texture.h

Holds and manages texture data

======================================================*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "Vec3.h"

class CTexture
{
public:
	CTexture(const char *szFileName);
	~CTexture();

	void AddRef() {m_nRefCount++;}
	int ReleaseRef() {m_nRefCount--;  return m_nRefCount;}

	const char *GetFileName() const {return m_szFileName.c_str();}

	void SetPixelData(int nWidth, int nHeight, unsigned int *pPixelData, int nRowPitch);

	Vec3 GetTexelBilinear(MRTFLOAT fU, MRTFLOAT fV,MRTFLOAT *pAlphaValue = 0);
	Vec3 GetTexel(MRTFLOAT fU, MRTFLOAT fV,MRTFLOAT *pAlphaValue = 0);

	int GetWidth() const {return m_nWidth;}
	int GetHeight() const {return m_nHeight;}

private:
	std::string m_szFileName;
	int m_nRefCount;

	int m_nWidth;
	int m_nHeight;
	unsigned int *m_pPixelData;
};

#endif //TEXTURE_H