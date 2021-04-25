#include "AnimationClip.h"
#include <iostream>
#include <string>
#include "AssimpToGlmHelper.h"
#include <glm/gtx/string_cast.hpp>


AnimationClip::AnimationClip(vector<AnimationNode*> _animNodeList) {
	animNodeList = _animNodeList;
	prevTime = 0;
	int numKeys = animNodeList.at(0)->positionKeys.size();
	if (numKeys > 0) {
		duration = animNodeList.at(0)->positionKeys[numKeys - 1]->time;
	}
	else {
		duration = 0;
	}
}

AnimationClip::~AnimationClip() {

}

bool AnimationClip::load(char* filename) {
	return true;
}


int findKeyframe(float time, AnimationNode* animationNode) {
	for (int j = 0; j < animationNode->positionKeys.size(); j++) {
		if (time <= animationNode->positionKeys[j]->time) {
			return j;
		}
	}

	return animationNode->positionKeys.size() - 1;
}
void AnimationClip::evaluate(float time, glm::mat4 rootWorld) {
	
	for (int i = 0; i < animNodeList.size(); i++) {
		AnimationNode* animationNode = animNodeList.at(i);
		glm::mat4 positionTransform(1.0f);
		glm::mat4 rotationTransform(1.0f);
		glm::mat4 scalingTransform(1.0f);
		int keyframe = findKeyframe(time, animationNode);
		if (keyframe == 0 || keyframe == animationNode->positionKeys.size() - 1) {
			positionTransform = glm::translate(positionTransform, animationNode->positionKeys[keyframe]->value);
			rotationTransform = glm::toMat4(animationNode->rotationKeys[keyframe]->value);
			scalingTransform = glm::scale(scalingTransform, animationNode->scalingKeys[keyframe]->value);
			
		}
		else {
			float timeA = animationNode->positionKeys[keyframe - 1]->time;
			float timeB = animationNode->positionKeys[keyframe]->time;
			float ratio = (time - timeA) / (timeB - timeA); 
			
			glm::vec3 position;
			glm::vec3 scaling;
			glm::quat rotation;
			if (animationNode->positionKeys[keyframe]->tangentIn == glm::vec3(0.0f, 0.0f, 0.0f)) {
				position = animationNode->positionKeys[keyframe]->value;
			}
			else {
				position = animationNode->positionKeys[keyframe]->tangentIn * (ratio);
				
			}
			if (animationNode->scalingKeys[keyframe]->tangentIn == glm::vec3(0.0f, 0.0f, 0.0f)) {
				scaling = animationNode->scalingKeys[keyframe]->value;
			}
			else {
				scaling = animationNode->scalingKeys[keyframe]->tangentIn * (ratio);
			}
			if (animationNode->rotationKeys[keyframe - 1]->value == animationNode->rotationKeys[keyframe]->value) {
				rotation = animationNode->rotationKeys[keyframe]->value;
			}
			else {
				rotation = glm::slerp(animationNode->rotationKeys[keyframe - 1]->value, animationNode->rotationKeys[keyframe]->value, ratio);
			}
			
			positionTransform = glm::translate(positionTransform, position);
			scalingTransform = glm::scale(scalingTransform, scaling);
			rotationTransform = glm::toMat4(rotation);
		}

		for (int j = 0; j < animationNode->meshList.size(); j++) {
			Mesh* mesh = animationNode->meshList.at(j);
			mesh->model = rootWorld * positionTransform * rotationTransform * scalingTransform;
		}
	}
}

void AnimationClip::selectKeyframe(int keyframe, glm::mat4 rootWorld) {

	for (int i = 0; i < animNodeList.size(); i++) {
		AnimationNode* animationNode = animNodeList.at(i);
		for (int j = 0; j < animationNode->meshList.size(); j++) {
			Mesh* mesh = animationNode->meshList.at(j);
			glm::mat4 positionTransform = glm::translate(glm::mat4(1.0f), animationNode->positionKeys[keyframe]->value);
			glm::mat4 rotationTransform = glm::toMat4(animationNode->rotationKeys[keyframe]->value);
			glm::mat4 scalingTransform = glm::scale(glm::mat4(1.0f), animationNode->scalingKeys[keyframe]->value);
			mesh->model = rootWorld * positionTransform * rotationTransform * scalingTransform;
		}
	}
}