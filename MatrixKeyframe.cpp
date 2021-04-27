#include "MatrixKeyframe.h"
#include <iostream>
#include "AssimpToGlmHelper.h"
#include <glm/gtx/string_cast.hpp>


MatrixKeyframe::MatrixKeyframe(aiQuatKey m) {

	value = AssimpToGlmHelper::convertAiQuatToGlmQuat(m.mValue);
	time = m.mTime;
	A = 0;
	B = 0;
	C = 0;
	D = 0;
}

MatrixKeyframe::~MatrixKeyframe() {

}