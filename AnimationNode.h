#ifndef _ANIMATIONNODE_H_
#define _ANIMATIONNODE_H_
#include "Mesh.h"
#include <assimp/anim.h>

class AnimationNode {

public:
	string name;
	vector<glm::mat4> positionTransforms;
	vector<glm::mat4> orientationTransforms;
	vector<glm::mat4> scalingTransforms;
	vector<Mesh*> meshList;
	AnimationNode(aiNodeAnim* nodeAnim, vector<Mesh*> masterMeshList);
	~AnimationNode();
};
#endif
