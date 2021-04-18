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

	bool checkCollision(BoundingBox* b);
	

private:
	
	//min and max points of the bounding box
	glm::vec3 min;
	glm::vec3 max;
	void* parentObj;
	//used to toggle collisions
	bool active;
	
	//used to resolve collisions
	float delta = 0.01f;
};

////////////////////////////////////////////////////////////////////////////////
