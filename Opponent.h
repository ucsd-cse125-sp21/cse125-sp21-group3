#pragma once
#include "Model.h"
#include "core.h"
#include "BoundingBox.h"
#include <deque>

/*
 * Similar to the player class, but only doing most basic functions.
 */
class Opponent {
public:
	int id;
	Model* playerModel;
	Model* playerGunModel;
	glm::vec3 playerModelCenter;
	glm::vec3 playerGunModelCenter;
	BoundingBox* boundingBox;
	std::deque<Cube*> footprints;
	glm::vec3 lastFootPrintPos;
	glm::vec3 position;
	glm::vec3 direction;
	int moving;

	//constant opponent values
	float playerModelScale = 0.31f;
	float playerGunModelScale = 0.4f;
	float height = 4.0f;
	float width = 1.0f;

	//methods
	Opponent(int _id, glm::vec3 _position);
	~Opponent();
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update();
	void setPosition(glm::vec3 p) { position = p; }
	void setDirection(glm::vec3 d) { direction = d; }
	void setMoving(int m) { moving = m; }
};