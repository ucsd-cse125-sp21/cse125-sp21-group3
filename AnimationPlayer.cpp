#include "AnimationPlayer.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

AnimationPlayer::AnimationPlayer() {

}

void AnimationPlayer::play(AnimationClip* animationClip, float speed, glm::mat4 & rootWorld) {

	float time = animationClip->prevTime + speed;
	if (time > animationClip->duration) {
		time = 0.0f;
	}
	animationClip->evaluate(time, rootWorld);
	animationClip->prevTime = time;
}
