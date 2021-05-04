#ifndef _NODE_H_
#define _NODE_H_
#include "Mesh.h"
#include "VectorKeyframe.h"
#include "MatrixKeyframe.h"
#include <assimp/anim.h>
#include "Bone.h"

class Node {

public:

	string name;
	glm::mat4 transform;
	vector<Mesh*> meshList;
	Node(string _name);
	~Node();
	vector<Node*> children;
};
#endif
