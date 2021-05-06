#include "Bone.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "AssimpToGlmHelper.h"

Bone::Bone(aiBone* aiBone, vector<glm::vec3> meshPositions, vector<glm::vec3> meshNormals) {

	name = aiBone->mName.C_Str();
	offsetMatrix = AssimpToGlmHelper::convertAiMat4ToGlmMat4(aiBone->mOffsetMatrix);

	for (int i = 0; i < aiBone->mNumWeights; i++) {
		aiVertexWeight vw = aiBone->mWeights[i];
		defaultPositions.push_back(meshPositions.at(vw.mVertexId));
		defaultNormals.push_back(meshNormals.at(vw.mVertexId));
		vertexIds.push_back(vw.mVertexId);
		weights.push_back(vw.mWeight);
	}

	currentPositions = defaultPositions;
	currentNormals = defaultNormals;
	
	assert(currentPositions.size() == currentNormals.size() && currentNormals.size() == weights.size());
}

Bone::~Bone() {

}

void Bone::applyTransform(glm::mat4 positionTransform, glm::mat4 rotationTransform, glm::mat4 scalingTransform) {

	/*
	for (int i = 0; i < defaultPositions.size(); i++) {

		glm::vec3 positionA = defaultPositions.at(i);
		glm::vec4 tempPosition(defaultPositions.at(i).x, defaultPositions.at(i).y, defaultPositions.at(i).z, 1.0f);
		tempPosition = tempPosition * positionTransform;
		glm::vec3 positionB(tempPosition.x / tempPosition.w, tempPosition.y / tempPosition.w, tempPosition.z / tempPosition.w);
		glm::vec3 positionAtoB = positionB - positionA;
		glm::vec3 interpolatedPosition = (positionA + weights.at(i) * positionAtoB);
		glm::mat4 currentPositionTransform = glm::translate(glm::mat4(1.0f), interpolatedPosition - positionA);

		glm::quat rotationA(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 tempRotation(defaultPositions.at(i).x, defaultPositions.at(i).y, defaultPositions.at(i).z, 1.0f);
		tempRotation = tempRotation * rotationTransform;
		glm::vec3 rotationB(tempRotation.x / tempRotation.w, tempRotation.y / tempRotation.w, tempRotation.z / tempRotation.w);
		glm::vec3 rotationAtoB = positionB - positionA;
		glm::vec3 interpolatedRotation = (rotationA + weights.at(i) * rotationAtoB);
		glm::mat4 currentRotationTransform = glm::translate(glm::mat4(1.0f), interpolatedRotation - rotationA);

		glm::vec3 scalingA = defaultPositions.at(i);
		glm::vec4 tempScaling(defaultPositions.at(i).x, defaultPositions.at(i).y, defaultPositions.at(i).z, 1.0f);
		tempScaling = tempScaling * scalingTransform;
		glm::vec3 scalingB(tempScaling.x / tempScaling.w, tempScaling.y / tempScaling.w, tempScaling.z / tempScaling.w);
		glm::vec3 scalingAtoB = positionB - positionA;
		glm::vec3 interpolatedScaling = (scalingA + weights.at(i) * scalingAtoB);
		glm::mat4 currentPositionTransform = glm::scale(glm::mat4(1.0f), interpolatedScaling - scalingA);
	}
	*/
}