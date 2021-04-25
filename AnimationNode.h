#ifndef _ANIMATIONNODE_H_
#define _ANIMATIONNODE_H_
#include "Mesh.h"
#include "VectorKeyframe.h"
#include "MatrixKeyframe.h"
#include <assimp/anim.h>

class AnimationNode {

public:
	string name;
	vector<VectorKeyframe*> positionKeys;
	vector<MatrixKeyframe*> rotationKeys;
	vector<VectorKeyframe*> scalingKeys;
	vector<Mesh*> meshList;
	AnimationNode(aiNodeAnim* nodeAnim, vector<Mesh*> masterMeshList, float ticksPerSecond);
	~AnimationNode();
};
#endif
