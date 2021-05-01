#include "AnimationNode.h"
#include "AssimpToGlmHelper.h"
#include <glm/gtx/string_cast.hpp>

AnimationNode::AnimationNode(aiNodeAnim* aiNodeAnim, vector<Mesh*> masterMeshList, float ticksPerSecond) {
	
	assert(aiNodeAnim->mNumPositionKeys == aiNodeAnim->mNumRotationKeys && 
		aiNodeAnim->mNumRotationKeys == aiNodeAnim->mNumScalingKeys);

	name = aiNodeAnim->mNodeName.C_Str();

	for (int i = 0; i < aiNodeAnim->mNumPositionKeys; i++) {
		
		VectorKeyframe* positionKey = new VectorKeyframe(aiNodeAnim->mPositionKeys[i]);
		VectorKeyframe* scalingKey = new VectorKeyframe(aiNodeAnim->mScalingKeys[i]);
		MatrixKeyframe* rotationKey = new MatrixKeyframe(aiNodeAnim->mRotationKeys[i]);
		positionKey->time /= ticksPerSecond;
		scalingKey->time /= ticksPerSecond;
		rotationKey->time /= ticksPerSecond;
		
		positionKeys.push_back(positionKey);
		scalingKeys.push_back(scalingKey);
		rotationKeys.push_back(rotationKey);		
	}

	//setting tangents for the position and scaling keys
	for (int i = 0; i < positionKeys.size(); i++) {
		
		if (i == 0) {
			//setting key to be constant before animation starts
			positionKeys.at(i)->tangentIn = glm::vec3(0.0f, 0.0f, 0.0f);
			scalingKeys.at(i)->tangentIn = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		else {
			//setting key tangents to use linear interpolation
			glm::vec3 positionSlope = positionKeys.at(i)->value - positionKeys.at(i - 1)->value;
			positionKeys.at(i - 1)->tangentOut = positionSlope;
			positionKeys.at(i)->tangentIn = positionKeys.at(i - 1)->tangentOut;

			glm::vec3 scalingSlope =scalingKeys.at(i)->value - scalingKeys.at(i - 1)->value;
			scalingKeys.at(i - 1)->tangentOut = scalingSlope;
			scalingKeys.at(i)->tangentIn = scalingKeys.at(i - 1)->tangentOut;
		}
	}

	//setting key values to be constant after animation ends
	positionKeys.at(positionKeys.size() - 1)->tangentOut = glm::vec3(0.0f, 0.0f, 0.0f);
	scalingKeys.at(scalingKeys.size() - 1)->tangentOut = glm::vec3(0.0f, 0.0f, 0.0f);
	
	//finding subset of meshes from master mesh list that the animation applies to
	string aiNodeAnimName = aiNodeAnim->mNodeName.C_Str();
	for (int i = 0; i < masterMeshList.size(); i++) {
		string meshName = masterMeshList.at(i)->name;
		if (meshName.find("-") != string::npos) {
			meshName = meshName.substr(0, meshName.find("-"));
		}
		if (meshName.compare(aiNodeAnimName) == 0) {
			meshList.push_back(masterMeshList.at(i));
		}
	}
	
}