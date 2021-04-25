#ifndef _MATRIXKEYFRAME_H_
#define _MATRIXKEYFRAME_H_

#include "core.h"
#include <assimp/anim.h>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>

////////////////////////////////////////////////////////////////////////////////

class MatrixKeyframe
{

public:
	float time;
	glm::quat value;
	float tangentIn; //slope in
	float tangentOut; //slope out
	char ruleIn[256];
	char ruleOut[256];
	float A, B, C, D; //cubic coefficients for span before keyframe
	MatrixKeyframe(aiQuatKey m);
	~MatrixKeyframe();
};

////////////////////////////////////////////////////////////////////////////////

#endif
