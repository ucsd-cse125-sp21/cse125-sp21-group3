#include "AnimationPlayer.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

AnimationPlayer::AnimationPlayer() {

}

void AnimationPlayer::play(AnimationClip* animationClip, int modelIndex, glm::mat4 & rootWorld) {

	animationClip->evaluate(modelIndex, rootWorld);
}
