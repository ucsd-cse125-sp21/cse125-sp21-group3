#ifndef _ANIMATIONCLIP_H_
#define _ANIMATIONCLIP_H_

#include "Mesh.h"
#include <assimp/anim.h>

////////////////////////////////////////////////////////////////////////////////

class AnimationClip
{

public:
	
	float rangeStart;
	float rangeEnd;
	float prevTime;
	vector<aiNodeAnim*> animNodeList;
	vector<Mesh*> meshList;
	void evaluate(float time, glm::mat4 & rootWorld);
	bool load(char* filename);

	~AnimationClip();
	AnimationClip(vector<aiNodeAnim*> _animNodeList, vector<Mesh*> _meshList);
};

////////////////////////////////////////////////////////////////////////////////

#endif
