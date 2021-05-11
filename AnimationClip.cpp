#include "AnimationClip.h"
#include <iostream>
#include <string>
#include "AssimpToGlmHelper.h"
#include <glm/gtx/string_cast.hpp>


AnimationClip::AnimationClip(map<string, AnimationNode*> _animationNodeMap, vector<Mesh*> _masterMeshList) {
	animationNodeMap = _animationNodeMap;
	masterMeshList = _masterMeshList;

	//initialize bonePositions and boneNormals
	for (int i = 0; i < masterMeshList.size(); i++) {
		vector<glm::vec3> numPositions(masterMeshList.at(i)->positions.size());
		bonePositions.push_back(numPositions);
		boneNormals.push_back(numPositions);
	}

	assert(bonePositions.size() == masterMeshList.size());
	prevTime = 0;
	map<string, AnimationNode*>::iterator it = animationNodeMap.begin();
	int numKeys = it->second->positionKeys.size();
	if (numKeys > 0) {
		duration = it->second->positionKeys[numKeys - 1]->time;
	}
	else {
		duration = 0;
	}
}

AnimationClip::~AnimationClip() {

}


int findKeyframe(float time, AnimationNode* animationNode) {
	for (int j = 0; j < animationNode->positionKeys.size(); j++) {
		if (time <= animationNode->positionKeys[j]->time) {
			return j;
		}
	}

	return animationNode->positionKeys.size() - 1;
}

void AnimationClip::calculateInterpolatedMatrices(float time, AnimationNode* animationNode,
	glm::mat4 & positionTransform, glm::mat4 & rotationTransform, glm::mat4 & scalingTransform) {

	//cout << "calculate interpolated matrices time: " << time << endl;
	int keyframe = findKeyframe(time, animationNode);
	if (keyframe == 0 || keyframe == animationNode->positionKeys.size() - 1) {
		positionTransform = glm::translate(positionTransform, animationNode->positionKeys[keyframe]->value);
		rotationTransform = glm::toMat4(animationNode->rotationKeys[keyframe]->value);
		scalingTransform = glm::scale(scalingTransform, animationNode->scalingKeys[keyframe]->value);
	}
	else {
		//cout << "animationNode: " << animationNode->name << endl;
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
			glm::vec3 prevPosition = animationNode->positionKeys[keyframe - 1]->value;
			position = prevPosition + animationNode->positionKeys[keyframe]->tangentIn * (ratio);

		}
		if (animationNode->scalingKeys[keyframe]->tangentIn == glm::vec3(0.0f, 0.0f, 0.0f)) {
			scaling = animationNode->scalingKeys[keyframe]->value;
		}
		else {
			glm::vec3 prevScale = animationNode->scalingKeys[keyframe - 1]->value;
			scaling = prevScale + animationNode->scalingKeys[keyframe]->tangentIn * (ratio);
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

	
}

void AnimationClip::calculateBoneTransforms(float time, Node* node, glm::mat4 parentTransform) {

	glm::mat4 nodeTransform;
	if (animationNodeMap.find(node->name) != animationNodeMap.end()) {
		
		AnimationNode* animationNode = animationNodeMap.find(node->name)->second;
		glm::mat4 positionTransform(1.0f);
		glm::mat4 rotationTransform(1.0f);
		glm::mat4 scalingTransform(1.0f);
		calculateInterpolatedMatrices(time, animationNode, positionTransform, rotationTransform, scalingTransform);
		nodeTransform = positionTransform * rotationTransform * scalingTransform;
	}
	else {
		nodeTransform = node->transform; 
	}

	glm::mat4 globalTransform = parentTransform * nodeTransform;
	if (boneTransformMap.find(node->name) == boneTransformMap.end()) {
		boneTransformMap.insert(pair<string, glm::mat4>(node->name, globalTransform));
	}
	else {
		boneTransformMap.find(node->name)->second = globalTransform;
	}

	for (int i = 0; i < node->meshList.size(); i++) {
		node->meshList.at(i)->model = globalTransform;
	}

	for (int i = 0; i < node->children.size(); i++) {
		Node* child = node->children.at(i);
		calculateBoneTransforms(time, child, globalTransform);
	}
}

void AnimationClip::applyBoneTransforms() {

	for (int i = 0; i < masterMeshList.size(); i++) {
		Mesh* mesh = masterMeshList.at(i);
		vector<glm::vec3> bonePositions(mesh->positions.size());
		vector<glm::vec3> boneNormals(mesh->positions.size());
		for (int j = 0; j < mesh->bones.size(); j++) {
			Bone* bone = mesh->bones.at(j);
			if (boneTransformMap.find(bone->name) != boneTransformMap.end()) {
				glm::mat4 nodeModel = boneTransformMap.find(bone->name)->second;
				for (int k = 0; k < bone->vertexIds.size(); k++) {
					
					int vertexId = bone->vertexIds.at(k);
					glm::vec4 v(mesh->initialPositions.at(vertexId).x, mesh->initialPositions.at(vertexId).y, mesh->initialPositions.at(vertexId).z, 1.0f);
					glm::vec4 v_prime = nodeModel * bone->offsetMatrix * v;
					bonePositions.at(vertexId) += glm::vec3(v_prime.x, v_prime.y, v_prime.z) * bone->weights.at(k);
					glm::vec4 n(mesh->initialNormals.at(vertexId).x, mesh->initialNormals.at(vertexId).y, mesh->initialNormals.at(vertexId).z, 0.0f);
					glm::vec4 n_prime = nodeModel * bone->offsetMatrix * n;
					boneNormals.at(vertexId) += glm::vec3(n_prime.x, n_prime.y, n_prime.z) * bone->weights.at(k);
					
				}
			}
			else {
				cout << "could not find node: " << bone->name << " in boneTransformMap" << endl;
			}
		}

		if (mesh->bones.size() > 0) {
			mesh->positions = bonePositions;
			mesh->normals = boneNormals;
			//cout << "final value for vertex 828: " << glm::to_string(mesh->positions.at(828)) << endl;
			glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_positions);
			glBufferSubData(GL_ARRAY_BUFFER, 0.0f, sizeof(glm::vec3) * mesh->positions.size(), mesh->positions.data());
			glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normals);
			glBufferSubData(GL_ARRAY_BUFFER, 0.0f, sizeof(glm::vec3) * mesh->normals.size(), mesh->normals.data());
		}
	}
}

void AnimationClip::selectKeyframe(int keyframe, glm::mat4 rootWorld) {

	map<string, AnimationNode*>::iterator i = animationNodeMap.begin();
	for (i = animationNodeMap.begin(); i != animationNodeMap.end(); i++) {
		AnimationNode* animationNode = i->second;
		for (int j = 0; j < animationNode->meshList.size(); j++) {
			Mesh* mesh = animationNode->meshList.at(j);
			glm::mat4 positionTransform = glm::translate(glm::mat4(1.0f), animationNode->positionKeys[keyframe]->value);
			glm::mat4 rotationTransform = glm::toMat4(animationNode->rotationKeys[keyframe]->value);
			glm::mat4 scalingTransform = glm::scale(glm::mat4(1.0f), animationNode->scalingKeys[keyframe]->value);
			
			mesh->model = rootWorld * positionTransform * rotationTransform * scalingTransform;
		}
	}
}