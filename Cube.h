#ifndef _CUBE_H_
#define _CUBE_H_

#include "core.h"
#include "BoundingBox.h"

////////////////////////////////////////////////////////////////////////////////
/*
 * File Name: Cube.h
 *
 * Contains Cube class and defines all the properties of a cube and methods that
 * can be performed on a cube.
 *
 * @author Part of 169 starter code
 */
class Cube
{
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 model;
	glm::vec3 color;
	float health = 100.0f;
	// Cube Information
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;


	glm::vec3 mazePosition;
	BoundingBox* boundingBox;

	bool direction; // 1 if bottom, 0 if right
	int cubeType;
	bool canDelete;
	bool isClient;


public:
	Cube(glm::vec3 cubeMin = glm::vec3(-1, -1, -1), glm::vec3 cubeMax = glm::vec3(1, 1, 1), int type=border, bool client=true);
	~Cube();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update();
	glm::mat4 getModel();
	void setModel(glm::mat4 m);
	glm::vec3 getColor();
	void setColor(glm::vec3 c);
	BoundingBox* getBoundingBox() { return boundingBox; }

	glm::vec3 getMazePosition() { return mazePosition; }

	bool getDirection() { return direction; }
	int getType() { return cubeType; }

	bool isDeletable() { return canDelete; }



	enum cubeType
	{
		border,
		wall,
		abilityChest
	};
};

////////////////////////////////////////////////////////////////////////////////

#endif
