#pragma once

#include "core.h"

/*
 * File Name: BoundingBox.h
 *
 * Contains the BoundingBox class. BoundingBoxes are axis-aligned
 * boxes around objects which are used to check collisions.
 *
 * @author Lucas Hwang
 */

class BoundingBox {
public:
	
	BoundingBox(glm::vec3 boxMin, glm::vec3 boxMax, void* parentObj);

	glm::vec3 getMin() { return min; }
	glm::vec3 getMax() { return max; }
	bool getActive() { return active; }

	void setMin(glm::vec3 p) { min = p; }
	void setMax(glm::vec3 p) { max = p; }
	void setActive(bool val) { active = val; }
	void setModel(glm::mat4 m) { model = m; }
	bool checkCollision(BoundingBox* b);
	
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update(glm::vec3 _min, glm::vec3 _max);
	
private:
	
	//min and max points of the bounding box
	glm::vec3 min;
	glm::vec3 max;
	void* parentObj;
	//used to toggle collisions
	bool active;
	
	//used to resolve collisions
	float delta = 0.01f;

	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 model;
	glm::vec3 color;

	// Cube Information
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
};

////////////////////////////////////////////////////////////////////////////////
