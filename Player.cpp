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
Player::Player(glm::vec3 _position, Maze* mz) {
    
    position = _position;

    lastFootPrintPos = _position;

    prevPosition = position;
    height = 4.0f;
    width = 1.0f;
    //std::cout << "position.y: " << position.y << std::endl;
    //std::cout << "player min at start: " << (position.y - height * 0.75f) << std::endl;
    boundingBox = new BoundingBox(glm::vec3(position.x - width * 0.5f, position.y - height * 0.875f, position.z - width * 0.5f),
        glm::vec3(position.x + width * 0.5f, position.y + height * 0.125f, position.z + width * 0.5f), this);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    speed = 3.5f;
    playerWeapon = new Weapon();

    maxHealth = 100.0f;
    currentHealth = 100.0f;

    currentArmor = 0.0f;
    currentDamageBoost = 0.0f;

    currentAbility = none;

    maze = mz;
    
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75f, position.y - height * 0.82f, 2.25f));
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(playerModelScale));
    glm::mat4 playerModelRootTransform = translation  * rotation * scaling; 
    playerModel = new Model("Assets/character.gltf", playerModelRootTransform);
    walkingBackward = false;
    playerModel->playAnimation(playerModel->animationClipList.at(0), 0.00f, false); //puts the character in the default pose
}

void Player::createFootPrint(glm::vec3 footprintPos) {
    if (glm::distance(lastFootPrintPos, footprintPos) > 5.0f) {
        irrklang::vec3df position(footprintPos.x, footprintPos.y, footprintPos.z);
        irrklang::ISound* snd = soundEngine->play3D("footstep.mp3", position, false, true);
        Cube* footprint = new Cube(footprintPos - glm::vec3(1.0f, footprintPos.y, 0.5f), footprintPos - glm::vec3(0.0f, footprintPos.y - 0.01f, 0.0f));
        footprint->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
        if (this->footprints.size() > 10) {
            this->footprints.pop_front();
        }
        this->footprints.push_back(footprint);
        lastFootPrintPos = footprintPos;
        
        
        if (snd)
        {
            if (state == sprint) {
                snd->setMinDistance(10.0f); // a mid sound
                snd->setPlaybackSpeed(2.0f);
                snd->setVolume(1.0f);
            }
            else if (state == crouch) {
                snd->setMinDistance(2.0f);
                snd->setPlaybackSpeed(0.75f);
                snd->setVolume(0.25f);
            }
            else {
                snd->setMinDistance(5.0f);
                snd->setVolume(0.75f);
            }
            snd->setIsPaused(false); // unpause the sound
        }
    }
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
                    glm::vec3(prevPosition.x + width * 0.5f, prevPosition.y + height * 0.25f, prevPosition.z + width * 0.5f), this);
                handleCollision(prevBoundingBox, b);
                boundingBox->update(glm::vec3(position.x - width * 0.5f, position.y - height * 0.75f, position.z - width * 0.5f),
                    glm::vec3(position.x + width * 0.5f, position.y + height * 0.25f, position.z + width * 0.5f));
                delete prevBoundingBox;
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

    switch (state)
    {
        case crouch:
            if (position.y >= 2.0f)
            {
                glm::vec3 v = glm::vec3(0.0f, -1.0f, 0.0f) * speed / 2.0f;
                velocity += v;
            }
            velocity *= 0.5f;
        case sprint:
            velocity *= 1.40f;
            break;
        case dead:
            velocity *= 3;
            break;
        case still:
            velocity *= 0;
            break;
        default:
            if (position.y <= 3.5f)
            {
                glm::vec3 v = glm::vec3(0.0f, 1.0f, 0.0f) * speed / 2.0f;
                velocity += v;
                velocity *= 0.5f;
            }
            break;
    }
    if (glm::length(velocity) > 0.0f) {

       
        position = position + velocity * deltaTime;
        
        //update player bounding box
        boundingBox->update(glm::vec3(position.x - width * 0.5f, position.y - height * 0.75f, position.z - width * 0.5f),
            glm::vec3(position.x + width * 0.5f, position.y + height * 0.25f, position.z + width * 0.5f));
    }

    if (boundingBox->getActive()) {
        applyConstraints(maze -> getBoundingBox());
    }
    if (state != dead && state != still)
    {
        createFootPrint(position);
    }
    if (state != still)
    {
        //update player model position
        glm::vec3 diff = position - prevPosition;
        playerModel->rootModel[3][0] += diff.x;
        playerModel->rootModel[3][1] += diff.y;
        playerModel->rootModel[3][2] += diff.z;
    
        prevPosition = position;
        oldPitch = playerCamera -> getPitch();

       
        //camera and player position should always be the same, at least for now
        playerCamera->setPosition(position);
    }

    if (glm::length(velocity) > 0.0f && state != dead) {
        playerModel->playAnimation(playerModel->animationClipList.at(0), 0.3f, walkingBackward);
    }

    //update player camera
    playerCamera->Update();
   
    //update player model
    playerModel->update();
}

/*
 * Called each frame to draw the player's model.
 *
 * @param viewProjMtx The view projection matrix needed to render the cube to the window.
 * Check the rendering reference section of the project documentation for more details.
 * @param shader The shader used to render the cube
 * @author Lucas Hwang
 */
void Player::draw(const glm::mat4& viewProjMtx, GLuint shader) {

    if (Window::debugMode) {
        boundingBox->draw(viewProjMtx, shader);
    }

    playerModel->draw(viewProjMtx, shader);
}

/*
 * Moves the player in the direction inputted.
 *
 * @param dir Integer representing the directional key pressed
 * @author Lucas Hwang
 */
void Player::moveDirection(int dir) {
    glm::vec3 currentDirection = playerCamera->getDirection();
    currentDirection.y = 0.0f;

    if (dir == forward) {
        walkingBackward = false;
        glm::vec3 v = glm::normalize(currentDirection) * speed;
        velocity += v;
    }
    if (dir == backward) {
        walkingBackward = true;
        glm::vec3 v = -glm::normalize(currentDirection) * speed;
        velocity += v;
    }
    if (dir == left) {
        walkingBackward = false;
        glm::vec3 v = -glm::normalize(glm::cross(currentDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
        velocity += v;
    }
    if (dir == right) {
        walkingBackward = false;
        glm::vec3 v = glm::normalize(glm::cross(currentDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
        velocity += v;
    }
    if (state == dead || true)
    {
        if (dir == up) {
            glm::vec3 v = glm::vec3(0.0f, 1.0f, 0.0f) * speed;
            velocity += v;
        }
        if (dir == down) {
            glm::vec3 v = glm::vec3(0.0f, -1.0f, 0.0f) * speed;
            velocity += v;
        }
    }
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



void Player::setHealth(float health)
{
    if (maxHealth <= health)
    {
        health = maxHealth;
    }
    currentHealth = health;
    if (currentHealth < 0.0f && state != dead)
    {
        state = dead;
    }
}

void Player::setArmor(float armor)
{
    currentArmor = armor;
}

void Player::setDamageBoost(float damageBoost)
{
    currentDamageBoost = damageBoost;
}

void Player::setMaxHealth(float max)
{
    maxHealth = max;
}

void Player::setState(int st)
{
    state = st;
}

void Player::pickUpAbility()
{
    BoundingBox* shotObject = shootWeapon(maze -> getChestBoundingBox());
    if (shotObject == NULL)
    {
        return;
    }
    Cube* parentCube = shotObject -> getParentCube();
    std::cout << "LMAO" << parentCube->getType() << std::endl;
    if (parentCube->getType() == Cube::abilityChest || true)
    {
        int* playerPos = maze -> getCoordinates(getPosition());
        int* chestPos = maze -> getCoordinates(parentCube->getMazePosition());
        if ((playerPos[0] == chestPos[0] && playerPos[1] == chestPos[1]) || 
            (playerPos[0] == (chestPos[0] - 1) && playerPos[1] == chestPos[1]) || 
            (playerPos[0] == chestPos[0] && (playerPos[1] == chestPos[1] - 1)))
        {
            currentAbility = maze->getAbility(chestPos);
            maze->removeAbility(chestPos);

            std::cout << "Picked up ability:" << currentAbility << std::endl;

            delete parentCube;
            delete playerPos;
            delete chestPos;
        }
    }
}

void Player::useAbility()
{
    bool used = false;
    switch (currentAbility)
    {
    case none:
        break;
    case removeWall:
        used = removeWallAbility();
        break;
    case trackPlayer:
        break;
    case seeMap:
    {
        used = seeMapAbility();
        break;
    }
    case healPlayer:
        setHealth(getHealth() + 100.0f);
        used = true;
        break;
    case increasePlayerHealth:
        setMaxHealth(getMaxHealth() + 50.0f);
        used = true;
        break;
    case armorPlayer:
        setArmor(getArmor() + 1.0f);
        used = true;
        break;
    case damageBoost:
        setDamageBoost(getDamageBoost() + 50.0f);
        used = true;
        break;
    default:
        break;
    }
    if (used)
    {
        currentAbility = none;
    }
}



BoundingBox* Player::shootWeapon(std::vector<BoundingBox *> objects) {
    if (state != sprint && state != dead)
    {
        BoundingBox* shotObject = playerWeapon->Shoot(objects, playerCamera->getPosition(), playerCamera->getDirection());
        return shotObject;
    }
    return 0;
}

bool Player::removeWallAbility()
{
    BoundingBox* shotObject = shootWeapon(maze->getBoundingBox());
    if (shotObject == NULL)
    {
        return false;
    }
    Cube* parentCube = shotObject->getParentCube();
    if (parentCube != NULL && parentCube->isDeletable())
    {
        delete parentCube;
        return true;
    }
    return false;
}

bool Player::seeMapAbility()
{
    float mazeSize = maze->getMazeSize();
    float mapScale = maze->getMapScale();
    float middleMap = (mazeSize - 1) * mapScale / 2.0f;
    playerCamera->Reset();
    playerCamera->setPitch(-90.0f);
    playerCamera->setPosition(glm::vec3(middleMap, 200.0f, middleMap));
    setState(still);
    playerCamera->setFarClip(350.0f);
    return true;
}

////////////////////////////////////////////////////////////////////////////////