#include "AnimationClip.h"
#include <iostream>
#include <string>
#include "AssimpToGlmHelper.h"
#include <glm/gtx/string_cast.hpp>


AnimationClip::AnimationClip(vector<aiNodeAnim*> _animNodeList, vector<Mesh*> _meshList) {
	animNodeList = _animNodeList;
	meshList = _meshList;
	prevTime = 0;
}

AnimationClip::~AnimationClip() {

}

bool AnimationClip::load(char* filename) {
	return true;
}

void AnimationClip::evaluate(float time, glm::mat4 & rootWorld) {
	
	for (int i = 0; i < animNodeList.size(); i++) {
		aiNodeAnim* animNode = animNodeList.at(i);
		glm::mat4 translation(1.0f);
		glm::mat4 rotation(1.0f);
		glm::mat4 scaling(1.0f);
		for (int j = animNode->mNumPositionKeys - 1; j >= 0; j--) {
			aiVectorKey key = animNode->mPositionKeys[j];
			if (time > key.mTime) {
				translation = glm::translate(translation, AssimpToGlmHelper::convertAiVec3ToGlmVec3(key.mValue));
				break;
			}
		}
		for (int j = animNode->mNumRotationKeys - 1; j >= 0; j--) {
			aiQuatKey key = animNode->mRotationKeys[j];
			if (key.mTime > time) {
				rotation = AssimpToGlmHelper::convertAiMat3ToGlmMat3(key.mValue.GetMatrix());
				cout << "rotation: " << endl;
				cout << glm::to_string(rotation) << endl;
				break;
			}
		}
		for (int j = animNode->mNumScalingKeys - 1; j >= 0; j--) {
			aiVectorKey key = animNode->mScalingKeys[j];
			if (key.mTime > time) {
				scaling = glm::scale(scaling, AssimpToGlmHelper::convertAiVec3ToGlmVec3(key.mValue));
				break;
			}
		}
	
		glm::mat4 transform = transform * translation * rotation * scaling;
		string nodeName = animNode->mNodeName.C_Str();
		for (int j = 0; j < meshList.size(); j++) {
			if (nodeName.compare(meshList.at(j)->name) == 0) {
				meshList.at(j)->model = transform * rootWorld;
			}
		}
	}
}