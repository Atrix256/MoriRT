/*======================================================

SceneManager.h

Holds and manages scenes

======================================================*/

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>
#include "vec3.h"

//forward declarations
class CScene;

//typedefs
typedef std::vector<CScene *> SceneList;

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();

	CScene *AddScene(MRTFLOAT fGridSize, const Vec3 &vWorldMin, const Vec3 &vWorldMax);

private:
	SceneList m_SceneList;
};

extern CSceneManager SceneManager;

#endif //SCENEMANAGER_H