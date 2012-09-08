/*======================================================

TextureManager.h

Holds and manages textures

======================================================*/

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <vector>

//forward declarations
class CTexture;

//typedefs
typedef std::vector<CTexture *> TextureList;

class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	CTexture *FindTexture(const char *szFileName);
	CTexture *LoadTexture(const char *szFileName);
	void FreeTexture(CTexture *pTexture);

private:
	TextureList m_TextureList;
	bool m_bDeconstructed;
};

extern CTextureManager TextureManager;

#endif //TEXTUREMANAGER_H