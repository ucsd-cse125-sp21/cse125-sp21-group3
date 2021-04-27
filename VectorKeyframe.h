#ifndef _VECTORKEYFRAME_H_
#define _VECTORKEYFRAME_H_

#include "core.h"
#include <assimp/anim.h>

////////////////////////////////////////////////////////////////////////////////

class VectorKeyframe
{

public:
	float time;
	glm::vec3 value;
	glm::vec3 tangentIn; //slope in
	glm::vec3 tangentOut; //slope out
	char ruleIn[256];
	char ruleOut[256];
	float A, B, C, D; //cubic coefficients for span before keyframe
	VectorKeyframe(aiVectorKey v);
	~VectorKeyframe();
};

////////////////////////////////////////////////////////////////////////////////

#endif
