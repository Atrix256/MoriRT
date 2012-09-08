/*======================================================

TextureManager.cpp

Holds and manages textures

======================================================*/

#include "TextureManager.h"
#include "Texture.h"
#include "RayTracer.h"
#include "PlatformBase.h"
#include <string.h>

CTextureManager TextureManager;

CTextureManager::CTextureManager()
:m_bDeconstructed(false)
{
}

CTextureManager::~CTextureManager()
{
	//free the memory for each camera
	for(TextureList::iterator it = m_TextureList.begin(); it != m_TextureList.end(); ++it)
	{
		delete (*it);
	}

	//empty the camera list
	m_TextureList.clear();

	m_bDeconstructed = true;
}

CTexture *CTextureManager::FindTexture(const char *szFileName)
{
	for(TextureList::iterator it = m_TextureList.begin(); it != m_TextureList.end(); ++it)
	{
		if(!stricmp(szFileName,(*it)->GetFileName()))
		{
			return (*it);
		}
	}

	return 0;
}

CTexture *CTextureManager::LoadTexture(const char *szFileName)
{
	//if it's already loaded, just return that
	CTexture *pTexture = FindTexture(szFileName);
	if(pTexture)
	{
		pTexture->AddRef();
		return pTexture;
	}

	//else we need to create a new one
	pTexture = new CTexture(szFileName);

	//have our platform code load it
	CRayTracer::GetSingleton().GetPlatform().LoadTexture(pTexture);

	//add it to our list
	m_TextureList.push_back(pTexture);

	//return the texture we loaded
	return pTexture;
}

void CTextureManager::FreeTexture(CTexture *pTexture)
{
	if(!m_bDeconstructed)
	{
		const char *szFileName = pTexture->GetFileName();
		for(TextureList::iterator it = m_TextureList.begin(); it != m_TextureList.end(); ++it)
		{
			if(!stricmp(szFileName,(*it)->GetFileName()))
			{
				if((*it)->ReleaseRef() == 0)
				{
					delete (*it);
					m_TextureList.erase(it);
				}
				return;
			}
		}
	}
}