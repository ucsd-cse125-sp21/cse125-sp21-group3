#ifndef _TEXTURECUBE_H_
#define _TEXTURECUBE_H_

#include "core.h"

////////////////////////////////////////////////////////////////////////////////
/*
 * File Name: TextureCube.h
 *
 * Used for rendering textures
 *
 * @author Part of 169 starter code
 */
using namespace std;
class TextureCube
{
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_textureCoords, EBO;
	GLuint texture;

	glm::mat4 model;
	glm::vec3 color;

	// Cube Information
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> textureCoords;
	std::vector<unsigned int> indices;


public:
	TextureCube();
	~TextureCube();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
};

////////////////////////////////////////////////////////////////////////////////

#endif
