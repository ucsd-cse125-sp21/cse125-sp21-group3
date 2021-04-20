#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
using namespace std;

void printAiMat3(aiMatrix3x3 m) {

	cout << m.a1 << " " << m.a2 << " " << m.a3 << " " << "\n" <<
		m.b1 << " " << m.b2 << " " << m.b3 << " " << "\n" <<
		m.c1 << " " << m.c2 << " " << m.c3 << " " << "\n" << endl;
}

void printAiMat4(aiMatrix4x4 m) {

	cout << m.a1 << " " << m.a2 << " " << m.a3 << " " << m.a4 << "\n" <<
		m.b1 << " " << m.b2 << " " << m.b3 << " " << m.b4 << "\n" <<
		m.c1 << " " << m.c2 << " " << m.c3 << " " << m.c4 << "\n" <<
		m.d1 << " " << m.d2 << " " << m.d3 << " " << m.d4 << endl;
}

glm::mat3 convertAiMat3ToGlmMat3(aiMatrix3x3 m) {

	return glm::mat3({
		{m.a1, m.b1, m.c1},
		{m.a2, m.b2, m.c2},
		{m.a3, m.b3, m.c3}
		});
}

glm::mat4 convertAiMat4ToGlmMat4(aiMatrix4x4 m) {

	return glm::mat4({
		{m.a1, m.b1, m.c1, m.d1},
		{m.a2, m.b2, m.c2, m.d2},
		{m.a3, m.b3, m.c3, m.d3},
		{m.a4, m.b4, m.c4, m.d4}
		});
}

