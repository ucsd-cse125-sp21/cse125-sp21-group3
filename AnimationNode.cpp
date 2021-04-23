#include "AnimationNode.h"
#include "AssimpToGlmHelper.h"
#include <glm/gtx/string_cast.hpp>

AnimationNode::AnimationNode(aiNodeAnim* aiNodeAnim, vector<Mesh*> masterMeshList) {

	//cout << "num position keys: " << aiNodeAnim->mNumPositionKeys << endl;
	//cout << "num rotation keys: " << aiNodeAnim->mNumRotationKeys << endl;
	//cout << "num scaling keys: " << aiNodeAnim->mNumScalingKeys << endl;
	//assert(true);
	assert(aiNodeAnim->mNumPositionKeys == aiNodeAnim->mNumRotationKeys && 
		aiNodeAnim->mNumRotationKeys == aiNodeAnim->mNumScalingKeys);
	
	if (aiNodeAnim->mNumPositionKeys <= 0) {
		cout << "must have greater than 0 position keys for animation" << endl;
		return;
	}

	aiVectorKey prevPositionKey = aiNodeAnim->mPositionKeys[0];
	aiQuatKey prevOrientationKey = aiNodeAnim->mRotationKeys[0];
	aiVectorKey prevScalingKey = aiNodeAnim->mScalingKeys[0];

	glm::mat4 prevPosition = glm::translate(glm::mat4(1.0f),
		AssimpToGlmHelper::convertAiVec3ToGlmVec3(prevPositionKey.mValue));
	glm::mat4 prevOrientation = AssimpToGlmHelper::convertAiMat3ToGlmMat3(prevOrientationKey.mValue.GetMatrix());
	glm::mat4 prevScaling = glm::scale(glm::mat4(1.0f),
		AssimpToGlmHelper::convertAiVec3ToGlmVec3(prevScalingKey.mValue));

	for (int i = 1; i < aiNodeAnim->mNumPositionKeys; i++) {

		aiVectorKey currPositionKey = aiNodeAnim->mPositionKeys[i];
		aiQuatKey currOrientationKey = aiNodeAnim->mRotationKeys[i]; 
		aiVectorKey currScalingKey = aiNodeAnim->mScalingKeys[i];
		
		glm::mat4 currPosition = glm::translate(glm::mat4(1.0f), 
			AssimpToGlmHelper::convertAiVec3ToGlmVec3(currPositionKey.mValue));
		glm::mat4 currOrientation = AssimpToGlmHelper::convertAiMat3ToGlmMat3(currOrientationKey.mValue.GetMatrix());
		glm::mat4 currScaling = glm::scale(glm::mat4(1.0f),
			AssimpToGlmHelper::convertAiVec3ToGlmVec3(currScalingKey.mValue));
		
		glm::mat4 positionDiff = glm::inverse(prevPosition) * currPosition;
		glm::mat4 orientationDiff = glm::inverse(prevOrientation) * currOrientation;
		glm::mat4 scalingDiff = glm::inverse(prevScaling) * currScaling;

		positionTransforms.push_back(positionDiff);
		orientationTransforms.push_back(orientationDiff);
		scalingTransforms.push_back(scalingDiff);
		
		prevPosition = currPosition;
		prevOrientation = currOrientation;
		prevScaling = currScaling;
	}

	string aiNodeAnimName = aiNodeAnim->mNodeName.C_Str();
	for (int i = 0; i < masterMeshList.size(); i++) {
		string meshName = masterMeshList.at(i)->name;
		if (meshName.find("-") != string::npos) {
			//cout << "substr" << endl;
			meshName = meshName.substr(0, meshName.find("-"));
		}
		//cout << "meshName: " << meshName << endl;
		//cout << "aiNodeAnimName: " << aiNodeAnimName << endl;
		if (meshName.compare(aiNodeAnimName) == 0) {
			meshList.push_back(masterMeshList.at(i));
		}
	}
}