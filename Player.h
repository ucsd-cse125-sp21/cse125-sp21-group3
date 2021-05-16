#pragma once


#ifndef PLAYER_H
#define PLAYER_H

#include "main.h"
#include "core.h"
#include "Camera.h"
#include "BoundingBox.h"
#include "Weapon.h"
#include <deque>
#include "Maze.h"
#include "Model.h"
#include <irrKlang.h>

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

/*
 * File Name: Player.h
 *
 * Contains the Player class. Should hold all the properties and
 * objects that a player would have.
 *
 * @author Lucas Hwang
 */

class Maze;
class Camera;
class Weapon;
class Game;

class Player {
public:
	Player(glm::vec3 _position, Maze* mz, bool isClient=true);

	void applyForce(glm::vec3 f) { forceNet += f; }
	void computeForces();
	void integrate(float deltaTime);
	void applyConstraints(std::vector<BoundingBox*> boundingBoxList);
	void update(float deltaTime, std::vector<BoundingBox*> boundingBoxList, Game* game);
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	
	// Access functions
	void setPosition(glm::vec3 p) { position = p; }
	void setVelocity(glm::vec3 v) { velocity = v; }
	void setForceNet(glm::vec3 f) { forceNet = f; }
	void setPlayerCamera(Camera* c) { playerCamera = c; }
	void setSoundEngine(irrklang::ISoundEngine* i) { soundEngine = i;  }


	BoundingBox* shootWeapon(std::vector<BoundingBox*>);

	glm::vec3 getPosition() { return position; }
	glm::vec3 getVelocity() { return velocity; }
	glm::vec3 getForceNet() { return forceNet; }
	Camera* getPlayerCamera() { return playerCamera; }
	BoundingBox* getBoundingBox() { return boundingBox; }
	Model* getPlayerModel() { return playerModel; }
	Model* getPlayerGunModel() { return playerGunModel; }
	glm::vec3 getPlayerModelCenter() { return playerModelCenter; }
	glm::vec3 getPlayerGunModelCenter() { return playerGunModelCenter; }
	void createFootPrint(glm::vec3);
	void moveDirection(int dir);
	void updateBoundingBox();
	void handleCollision(BoundingBox* prevBoundingBox, BoundingBox* b);
	std::deque<Cube*> getFootprints() { return this->footprints; }

	float getHealth() { return currentHealth; }
	float getMaxHealth() { return maxHealth; }
	float getArmor() { return currentArmor; }
	float getDamageBoost() { return currentDamageBoost; }
	float getState() { return state; }


	void setHealth(float health);
	void setArmor(float armor);
	void setDamageBoost(float damageBoost);
	void setMaxHealth(float max);

	void setState(int st);

	void useAbility();

	void pickUpAbility();

	bool removeWallAbility();
	bool seeMapAbility();


	bool walkingBackward;
	enum movementDirection {
		forward,
		backward,
		left,
		right,
		up,
		down
	};

	enum playerPositions {
		crouch,
		stand,
		sprint,
		still,
		dead
	};

	enum abilityTypes {
		none,
		removeWall,
		trackPlayer,
		seeMap,
		healPlayer,
		increasePlayerHealth,
		armorPlayer,
		damageBoost,
		opened //to represent opened chests, not actual ability
	};

	//networking stuff
	void setId(int i) { id = i; }
	int getId() { return id; }
	string getPlayerInputString();
	string getPlayerInfoString();
	void setMoving(int m) { moving = m; }
	void setLookingDirection(glm::vec3 d) { lookingDirection = d; }
	void setHasFired(bool val) { hasFired = val; }
	void setIsFiring(bool val) { isFiring = val; }
	bool getHasFired() { return hasFired; }
	glm::vec3 getLookingDirection() { return lookingDirection; }

private:

	Model* playerModel;
	Model* playerGunModel;
	glm::vec3 playerModelCenter;
	glm::vec3 playerGunModelCenter;
	float playerModelScale = 0.31f;
	float playerGunModelScale = 0.4f;
	float playerWalkingSpeed = 0.3f;
	glm::vec3 playerToModelDiff;
	BoundingBox* boundingBox; // used to check collisions
	
	float mass;
	float speed;
	float height;
	float width;
	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::vec3 velocity;
	glm::vec3 forceNet;
	Camera* playerCamera;
	irrklang::ISoundEngine* soundEngine;
	Weapon* playerWeapon;

	//float oldPitch;

	int state;

	std::deque<Cube*> footprints;
	glm::vec3 lastFootPrintPos;
	float currentHealth;
	float maxHealth;

	float currentArmor;

	float currentDamageBoost;

	int currentAbility;

	Maze* maze;
	
	//networking stuff
	int id;
	bool isClient;
	int moving;
	bool hasFired;
	bool isFiring;
	string playerInputString;
	string playerInfoString;
	glm::vec3 lookingDirection;
};

////////////////////////////////////////////////////////////////////////////////
#endif