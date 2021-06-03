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
	Player(glm::vec3 _position, Game* gm, bool isClient=true);

	void applyForce(glm::vec3 f) { forceNet += f; }
	void computeForces();
	void integrate(float deltaTime);
	void applyConstraints(std::vector<BoundingBox*> boundingBoxList);
	void update(float deltaTime, Game* game);
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	
	// Access functions
	void setPosition(glm::vec3 p);
	void setVelocity(glm::vec3 v) { velocity.x = v.x; velocity.y = v.y; velocity.z = v.z; }
	void setForceNet(glm::vec3 f) { forceNet = f; }
	void setPlayerCamera(Camera* c) { playerCamera = c; }
	void setSoundEngine(irrklang::ISoundEngine* i) { soundEngine = i;  }
	irrklang::ISoundEngine* getSoundEngine() { return this->soundEngine; }

	BoundingBox* shootWeapon(std::vector<BoundingBox*>, bool playerShot);

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
	void setInput(int dir, bool on);
	void moveDirection(int dir);
	void updateBoundingBox();
	void handleCollision(BoundingBox* prevBoundingBox, BoundingBox* b);
	std::deque<Cube*> getFootprints() { return this->footprints; }

	float getHealth() { return currentHealth; }
	float getMaxHealth() { return maxHealth; }
	float getArmor() { return currentArmor; }
	float getDamageBoost() { return currentDamageBoost; }
	int getState() { return state; }


	void setHealth(float health);
	void setArmor(float armor);
	void setDamageBoost(float damageBoost);
	void setMaxHealth(float max);

	void setState(int st);

	void useAbility();

	void pickUpAbility();

	bool removeWallAbility();
	bool seeMapAbility();
	bool endMapAbility();


	bool walkingBackward;
	enum movementDirection {
		forward,
		backward,
		left,
		right,
		up,
		down, 
		crouchKey,
		sprintKey
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
	void setId(int i) { 
		id = i; 
		if (id == 0) {
			for (Mesh* m : playerModel->meshes) {
				m->baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		if (id == 1) {
			for (Mesh* m : playerModel->meshes) {
				m->baseColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			}
		}
		if (id == 2) {
			for (Mesh* m : playerModel->meshes) {
				m->baseColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			}
		}
		if (id == 3) {
			for (Mesh* m : playerModel->meshes) {
				m->baseColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			}
		}
	}
	int getId() { return id; }
	string getPlayerInputString();
	string getPlayerInfoString();
	void setMoving(int m) { moving = m; }
	int getMoving() { return moving; }
	//void setLookingDirection(glm::vec3 d) { lookingDirection = d; }
	void setHasFired(bool val);
	void setIsFiring(bool val) { isFiring = val; }
	bool getHasFired() { return hasFired; }
	void setIsHurt(bool val) { isHurt = val; }
	bool getIsHurt() { return isHurt; }

	void setAbility(int ab) { currentAbility = ab; }
	int getAbility() { return currentAbility; }

	//glm::vec3 getLookingDirection() { return lookingDirection; }

	bool* getInputDirections() { return inputDirections; }

	void resetInputDirections();

	void setPickUpAbilityKey(bool on) { pickUpAbilityKey = on; }
	void setUseAbilityKey(bool on) { useAbilityKey = on; }

	static string getAbilityName(int ability);

	void openChest();

	void setOldPitch(float p) { oldPitch = p; }
	void setOldYaw(float y) { oldYaw = y; }




private:
	float cameraOldYaw;
	glm::vec3 cameraOffset;
	Model* playerModel;
	Model* playerGunModel;
	glm::vec3 playerModelCenter;
	glm::vec3 playerGunModelCenter;
	float playerModelScale = 0.31f;
	float playerGunModelScale = 0.4f;
	float playerWalkingSpeed = 0.3f;
	glm::vec3 playerToModelDiff;
	bool isHurt = false;
	glm::vec3 playerColor;
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

	float oldPitch;
	float oldYaw;

	int state;

	std::deque<Cube*> footprints;
	glm::vec3 lastFootPrintPos;
	float currentHealth;
	float maxHealth;

	float currentArmor;

	float currentDamageBoost;

	int currentAbility;

	Maze* maze;
	Game* game;
	
	//networking stuff
	int id;
	bool isClient;
	int moving;
	bool hasFired;
	bool isFiring;
	string playerInputString;
	string playerInfoString;
	//glm::vec3 lookingDirection;

	bool* inputDirections;
	bool pickUpAbilityKey;
	bool useAbilityKey;

	bool usingMapAbility;

	float lastAbilityUseTime;
	float lastReloadTime;
	float lastFireTime;
};

////////////////////////////////////////////////////////////////////////////////
#endif