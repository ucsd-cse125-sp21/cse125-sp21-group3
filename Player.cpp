#include "Player.h"
#include "glm/gtx/euler_angles.hpp"
#include <glm\gtx\string_cast.hpp>

/*
 * File Name: Player.cpp
 *
 * This file contains the Player implementation. This class will control player
 * movement as well as control player collisions and interactions.
 *
 * @author Lucas Hwang
 */

 ////////////////////////////////////////////////////////////////////////////////

/*
 * Default constructor for the camera.
 *
 * @return Player object
 * @author Lucas Hwang
 */
Player::Player(glm::vec3 _position) {
   
    position = _position;
    prevPosition = position;
    height = 2.0f;
    width = 1.0f;
    boundingBox = new BoundingBox(glm::vec3(position.x - width * 0.5f, position.y - height * 0.75f, position.z - width * 0.5f),
        glm::vec3(position.x + width * 0.5f, position.y + height * 0.25f, position.z + width * 0.5f));
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    speed = 0.05f;
}

/*
 * Computes physical forces acting on player.
 *
 * @author Lucas Hwang
 */
void Player::computeForces() {

    forceNet = glm::vec3(0.0f, 0.0f, 0.0f);
    applyForce(glm::vec3(0.0f, -9.8f * mass, 0.0f)); //applying gravity
}

/*
 * Moves time forward a tiny bit and updates player position and physical
 * properties
 *
 * @param deltaTime How much time has passed since last update
 * @author Lucas Hwang
 */
void Player::integrate(float deltaTime) {

    velocity = velocity + (forceNet / mass) * deltaTime;
    prevPosition = position;
    position = position + velocity * deltaTime;
}

/*
 * Handles collision detection and other physical restraints.
 *
 * @author Lucas Hwang
 */
void Player::applyConstraints(std::vector<BoundingBox*> boundingBoxList) {

    for (int i = 0; i < boundingBoxList.size(); i++) {
        BoundingBox* b = boundingBoxList.at(i);
        //check to make sure bounding box is active for object
        //and that we are not checking against our own bounding box
        if (b->getActive() && b != boundingBox) {

            if (boundingBox->checkCollision(b)) {

                BoundingBox* prevBoundingBox = new BoundingBox(
                    glm::vec3(prevPosition.x - width * 0.5f, prevPosition.y - height * 0.75f, prevPosition.z - width * 0.5f),
                    glm::vec3(prevPosition.x + width * 0.5f, prevPosition.y + height * 0.25f, prevPosition.z + width * 0.5f));
                handleCollision(prevBoundingBox, b);
                updateBoundingBox();
                //std::cout << "min point collision" << std::endl;
            }
        }
    }
}


/*
 * Called each frame to update player's position and other physical properties.
 *
 * @param deltaTime How much time has passed since last update
 * @author Lucas Hwang
 */
void Player::update(float deltaTime, std::vector<BoundingBox*> boundingBoxList) {
  
    //computeForces();
    //integrate(deltaTime);
    if (glm::length(velocity) > 0.0f) {

        position = position + velocity * deltaTime; 
        //update player bounding box
        updateBoundingBox();
        //std::cout << "player bounding box min: " << glm::to_string(boundingBox->getMin()) << std::endl;
        //std::cout << "player bounding box max: " << glm::to_string(boundingBox->getMax()) << std::endl;
    }

    if (boundingBox->getActive()) {
        applyConstraints(boundingBoxList);
    }

    prevPosition = position;
    //camera and player position should always be the same, at least for now
    playerCamera->setPosition(position);

    //update player camera
    playerCamera->Update();
    
}

/*
 * Moves the player in the direction inputted.
 * 
 * @param dir Integer representing the directional key pressed
 * @author Lucas Hwang
 */
void Player::moveDirection(int dir) {

    if (dir == forward) {
        glm::vec3 v = glm::normalize(playerCamera->getDirection()) * speed;
        v.y = 0.0f;
        velocity += v;
    }
    if (dir == backward) {
        glm::vec3 v = -glm::normalize(playerCamera->getDirection()) * speed;
        v.y = 0.0f;
        velocity += v;
    }
    if (dir == left) {
        glm::vec3 v = -glm::normalize(glm::cross(playerCamera->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
        v.y = 0.0f;
        velocity += v;
    }
    if (dir == right) {
        glm::vec3 v = glm::normalize(glm::cross(playerCamera->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
        v.y = 0.0f;
        velocity += v;
    }
}

/*
 * Updates the min and max points of the bounding box based on the player position.
 *
 * @author Lucas Hwang
 */
void Player::updateBoundingBox() {
    boundingBox->setMin(glm::vec3(position.x - width * 0.5f, position.y - height * 0.75f, position.z - width * 0.5f));
    boundingBox->setMax(glm::vec3(position.x + width * 0.5f, position.y + height * 0.25f, position.z + width * 0.5f));
}


/*
 * Handles the response after a collision has been detected for the player.
 *
 * @param prevBoundingBox The bounding box for the player's previous position
 * @param b The bounding box for the objec that the player is colliding with
 * @author Lucas Hwang
 */
void Player::handleCollision(BoundingBox* prevBoundingBox, BoundingBox* b) {

    glm::vec3 a_min = prevBoundingBox->getMin();
    glm::vec3 a_max = prevBoundingBox->getMax();
    glm::vec3 b_min = b->getMin();
    glm::vec3 b_max = b->getMax();

    //to keep track of how the intersections changed between this and the previous frame
    bool prevIntersectX = (a_min.x <= b_max.x && a_max.x >= b_min.x);
    bool prevIntersectY = (a_min.y <= b_max.y && a_max.y >= b_min.y);
    bool prevIntersectZ = (a_min.z <= b_max.z && a_max.z >= b_min.z);

    //if we were not intersecting in the x axis in the previous frame
    if (!prevIntersectX) {

        //reverts x position
        position.x = prevPosition.x;
        if (velocity.x != 0.0f) {
            velocity.x = 0.0f;
        }
    }

    //if we were not intersecting in the y axis in the previous frame
    if (!prevIntersectY) {

        //reverts z position
        position.y = prevPosition.y;
        if (velocity.y != 0.0f) {
            velocity.y = 0.0f;
        }
    }

    //if we were not intersecting in the z axis in the previous frame
    if (!prevIntersectZ) {

        //reverts z position
        position.z = prevPosition.z;
        if (velocity.z != 0.0f) {
            velocity.z = 0.0f;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////