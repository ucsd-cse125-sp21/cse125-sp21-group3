#ifndef _ANIMATIONCLIP_H_
#define _ANIMATIONCLIP_H_

#include "AnimationNode.h"
#include <assimp/anim.h>

////////////////////////////////////////////////////////////////////////////////

class AnimationClip
{

public:
	
	float rangeStart;
	float rangeEnd;
	float prevTime;
	string name;
	vector<AnimationNode*> animNodeList;
	int index;
	void evaluate(int modelIndex, glm::mat4 & rootWorld);
	void update();
	bool load(char* filename);

	~AnimationClip();
	AnimationClip(vector<AnimationNode*> _animNodeList);
};

////////////////////////////////////////////////////////////////////////////////

#endif
