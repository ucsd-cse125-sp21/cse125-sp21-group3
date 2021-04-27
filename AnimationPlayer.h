#ifndef _ANIMATIONPLAYER_H_
#define _ANIMATIONPLAYER_H_

#include "AnimationClip.h"

////////////////////////////////////////////////////////////////////////////////

class AnimationPlayer
{

public:

	
	std::vector<AnimationClip*> animationClipList;
	void play(AnimationClip* animationClip, float time, glm::mat4 & rootWorld);
	AnimationPlayer();
	~AnimationPlayer();
};

////////////////////////////////////////////////////////////////////////////////

#endif
