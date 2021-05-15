#pragma once

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "core.h"

class Cube;
class Player;
class Model;

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
	BoundingBox(glm::vec3 boxMin, glm::vec3 boxMax, Model* parentObj, bool client);
	BoundingBox(glm::vec3 boxMin, glm::vec3 boxMax, Cube* parentObj, bool client = true);
	BoundingBox(glm::vec3 boxMin, glm::vec3 boxMax, Player* parentObj, bool client = true);

	glm::vec3 getMin() { return min; }
	glm::vec3 getMax() { return max; }
	bool getActive() { return active; }

	Player* getParentPlayer() { return parentPlayer; }
	Cube* getParentCube() { return parentCube; }
	Model* getParentModel() { return parentModel; }

	void setMin(glm::vec3 p) { min = p; }
	void setMax(glm::vec3 p) { max = p; }
	void setActive(bool val) { active = val; }
	void setModel(glm::mat4 m) {
		
		//apply transformation to min and max
		glm::mat4 someTransformation = glm::inverse(model) * m;
		glm::vec4 minTemp(min.x, min.y, min.z, 1.0f);
		glm::vec4 maxTemp(max.x, max.y, max.z, 1.0f);
		minTemp = minTemp * someTransformation;
		maxTemp = maxTemp * someTransformation;
		min = glm::vec3(minTemp.x / minTemp.w, minTemp.y / minTemp.w, minTemp.z / minTemp.w);
		max = glm::vec3(maxTemp.x / maxTemp.w, maxTemp.y / maxTemp.w, maxTemp.z / maxTemp.w);
		
		//update model
		model = m;
	}
	bool checkCollision(BoundingBox* b);
	
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update(glm::vec3 _min, glm::vec3 _max);
	
private:
	
	//min and max points of the bounding box
	glm::vec3 min;
	glm::vec3 max;
	Cube* parentCube;
	Player* parentPlayer;
	Model* parentModel;
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

	bool isClient;
};

////////////////////////////////////////////////////////////////////////////////
#endif