/*======================================================

SceneManager.cpp

Holds and manages scenes

======================================================*/

#include "SceneManager.h"
#include "Scene.h"

CSceneManager SceneManager;

CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
	//free the memory for each scene
	for(SceneList::iterator it = m_SceneList.begin(); it != m_SceneList.end(); ++it)
	{
		delete (*it);
	}

	//empty the scene list
	m_SceneList.clear();
}

CScene *CSceneManager::AddScene(MRTFLOAT fGridSize, const Vec3 &vWorldMin, const Vec3 &vWorldMax)
{
	CScene *pNewScene = new CScene(fGridSize, vWorldMin, vWorldMax);

	m_SceneList.push_back(pNewScene);

	return pNewScene;
}