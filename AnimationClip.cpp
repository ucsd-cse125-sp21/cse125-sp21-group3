#include "AnimationClip.h"
#include <iostream>
#include <string>
#include "AssimpToGlmHelper.h"
#include <glm/gtx/string_cast.hpp>


AnimationClip::AnimationClip(vector<AnimationNode*> _animNodeList) {
	animNodeList = _animNodeList;
	prevTime = 0;
	index = 0;
}

AnimationClip::~AnimationClip() {

}

bool AnimationClip::load(char* filename) {
	return true;
}

void AnimationClip::evaluate(int modelIndex, glm::mat4 & rootWorld) {
	
	
	for (int i = 0; i < animNodeList.size(); i++) {
		AnimationNode* animationNode = animNodeList.at(i);
		//cout << "mesh list size: " << animationNode->meshList.size() << endl;
		for (int j = 0; j < animationNode->meshList.size(); j++) {
			Mesh* mesh = animationNode->meshList.at(j);
			//if (animationNode->modelList.size() > 0) {
			//	
			//	mesh->model = animationNode->modelList.at(modelIndex);
			//	//cout << "mesh model: " << endl;
			//	//cout << glm::to_string(mesh->model) << endl;
			//	//mesh->model = glm::rotate(mesh->model, 1.57f, glm::vec3(1.0f, 0.0f, 0.0f));
			//	//std::cout << "mesh position: " << mesh->model[3][0] << " " << mesh->model[3][1] << " " << mesh->model[3][2] << endl;
			//}
			//else {
			//	cout << "model list empty" << endl;
			//}
		}
	}
}

void AnimationClip::update() {

	for (int i = 0; i < animNodeList.size(); i++) {
		AnimationNode* animationNode = animNodeList.at(i);
		//cout << "mesh list size: " << animationNode->meshList.size() << endl;
		for (int j = 0; j < animationNode->meshList.size(); j++) {
			Mesh* mesh = animationNode->meshList.at(j);
			//aiQuaternion quaternion(0.0f, 0.0f, 1.57f);
			//glm::mat4 rotation = AssimpToGlmHelper::convertAiMat3ToGlmMat3(quaternion.GetMatrix());
			mesh->model = mesh->model * animationNode->positionTransforms.at(index) *
				animationNode->orientationTransforms.at(index) * animationNode->scalingTransforms.at(index);
			//mesh->model = glm::rotate(mesh->model, 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
		}
	}

	index++;
}