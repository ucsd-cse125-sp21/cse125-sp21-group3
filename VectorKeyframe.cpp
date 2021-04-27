#include "VectorKeyframe.h"
#include "AssimpToGlmHelper.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


VectorKeyframe::VectorKeyframe(aiVectorKey v) {

	value = AssimpToGlmHelper::convertAiVec3ToGlmVec3(v.mValue);
	time = v.mTime;
	A = 0;
	B = 0;
	C = 0;
	D = 0;
}

VectorKeyframe::~VectorKeyframe() {

}