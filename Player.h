#pragma once

#include "main.h"
#include "Camera.h"
#include "BoundingBox.h"
#include "Weapon.h"

/*
 * File Name: Player.h
 *
 * Contains the Player class. Should hold all the properties and
 * objects that a player would have.
 *
 * @author Lucas Hwang
 */

class Player {
public:
	Player(glm::vec3 _position);

	void applyForce(glm::vec3 f) { forceNet += f; }
	void computeForces();
	void integrate(float deltaTime);
	void applyConstraints(std::vector<BoundingBox*> boundingBoxList);
	void update(float deltaTime, std::vector<BoundingBox*> boundingBoxList);
	void draw(const glm::mat4& viewProjMtx, GLuint shader);

	// Access functions
	void setPosition(glm::vec3 p) { position = p; }
	void setVelocity(glm::vec3 v) { velocity = v; }
	void setForceNet(glm::vec3 f) { forceNet = f; }
	void setPlayerCamera(Camera* c) { playerCamera = c; }

	void shootWeapon(std::vector<BoundingBox*>);

	glm::vec3 getPosition() { return position; }
	glm::vec3 getVelocity() { return velocity; }
	glm::vec3 getForceNet() { return forceNet; }
	Camera* getPlayerCamera() { return playerCamera; }
	BoundingBox* getBoundingBox() { return boundingBox; }
	void createFootPrint(glm::vec3);
	void moveDirection(int dir);
	void updateBoundingBox();
	void handleCollision(BoundingBox* prevBoundingBox, BoundingBox* b);
	std::vector<Cube*> getFootprints() { return this->footprints; }
	enum movementDirection {
		forward,
		backward,
		left,
		right,
		up,
		down,
		crouch,
		stand,
		sprint
	};

private:
	
	BoundingBox* boundingBox; //used to check collisions
	float mass;
	float speed;
	float height;
	float width;
	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::vec3 velocity;
	glm::vec3 forceNet;
	Camera* playerCamera;
	Weapon* playerWeapon;
	std::vector<Cube*> footprints;
	movementDirection state;
	glm::vec3 lastFootPrintPos;
	float currentHealth;
	float maxHealth;
};

////////////////////////////////////////////////////////////////////////////////
