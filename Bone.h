#ifndef _BONE_H_
#define _BONE_H_

#include "core.h"
#include <assimp/mesh.h>
using namespace std;
////////////////////////////////////////////////////////////////////////////////

class Bone
{

public:
	string name;

	glm::mat4 offsetMatrix;

	//subset of mesh positions and normals in their rest state
	vector<glm::vec3> defaultPositions;
	vector<glm::vec3> defaultNormals;

	//current positions and normals of bone vertices
	vector<glm::vec3> currentPositions;
	vector<glm::vec3> currentNormals;

	vector<int> vertexIds;

	//weights which influence vertices, all weights for a vertice should add up to 1
	vector<float> weights;

	Bone(aiBone* aiBone, vector<glm::vec3> meshPositions, vector<glm::vec3> meshNormals);
	~Bone();

	//applies a weighted transformation to all vertices
	void applyTransform(glm::mat4 positionTransform, glm::mat4 rotationTransform, glm::mat4 scalingTransform);
};

////////////////////////////////////////////////////////////////////////////////

#endif
