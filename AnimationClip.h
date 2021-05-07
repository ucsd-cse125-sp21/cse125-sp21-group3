#ifndef _ANIMATIONCLIP_H_
#define _ANIMATIONCLIP_H_

#include "AnimationNode.h"
#include <assimp/anim.h>
#include "Node.h"
#include "Mesh.h"
#include <map>

using namespace std;
////////////////////////////////////////////////////////////////////////////////

class AnimationClip
{

public:
	
	float prevTime;
	float duration;
	string name;
	//vector<AnimationNode*> animNodeList;
	map<string, AnimationNode*> animationNodeMap;
	vector<Mesh*> masterMeshList;
	vector<vector<glm::vec3>> bonePositions; //holds list of positions for each mesh based on bones
	vector<vector<glm::vec3>> boneNormals; //holds list of normals for each mesh based on bones

	map<string, glm::mat4> boneTransformMap;
	void calculateBoneTransforms(float time, Node* node, glm::mat4 rootTransform);
	void applyBoneTransforms();
	void calculateInterpolatedMatrices(float time, AnimationNode* animationNode,
		glm::mat4& positionTransform, glm::mat4& rotationTransform, glm::mat4& scalingTransform);

	void selectKeyframe(int keyframe, glm::mat4 rootWorld);

	~AnimationClip();
	AnimationClip(map<string, AnimationNode*> _animationNodeMap, vector<Mesh*> _masterMeshList);
};

////////////////////////////////////////////////////////////////////////////////

#endif
