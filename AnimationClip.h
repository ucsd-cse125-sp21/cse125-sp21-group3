#ifndef _ANIMATIONCLIP_H_
#define _ANIMATIONCLIP_H_

#include "AnimationNode.h"
#include <assimp/anim.h>

////////////////////////////////////////////////////////////////////////////////

class AnimationClip
{

public:
	
	float prevTime;
	float duration;
	string name;
	vector<AnimationNode*> animNodeList;
	void evaluate(float time, glm::mat4 rootWorld);
	void selectKeyframe(int keyframe, glm::mat4 rootWorld);
	bool load(char* filename);
	//void calculateTransforms(AnimationNode* animationNode, int keyframe);

	~AnimationClip();
	AnimationClip(vector<AnimationNode*> _animNodeList);
};

////////////////////////////////////////////////////////////////////////////////

#endif
