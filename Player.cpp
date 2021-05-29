#include "Player.h"
#include "glm/gtx/euler_angles.hpp"
#include <glm\gtx\string_cast.hpp>
#include "parsing/clientParse.h"


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
Player::Player(glm::vec3 _position, Game* gm, bool client) {
    
    id = -1;
    position = _position;

    lastFootPrintPos = _position;

    isClient = client;
    
    prevPosition = position;
    height = 4.0f;
    width = 1.0f;
    //std::cout << "position.y: " << position.y << std::endl;
    //std::cout << "player min at start: " << (position.y - height * 0.75f) << std::endl;
 
    boundingBox = new BoundingBox(glm::vec3(position.x - width * 0.5f, position.y - height * 0.875f, position.z - width * 0.5f),
        glm::vec3(position.x + width * 0.5f, position.y + 0.25f, position.z + width * 0.5f), this, isClient);
    
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    speed = 10.0f;
    playerWeapon = new Weapon();
  
    maxHealth = 100.0f;
    currentHealth = 100.0f;

    currentArmor = 0.0f;
    currentDamageBoost = 0.0f;

    currentAbility = none;

    state = stand;

    maze = gm -> maze;
    game = gm;
    

    playerCamera = new Camera(glm::vec3(position.x + 0.5f, position.y, position.z + 0.5f));
    
    //if (client) {
        //creating and initializing playerModel
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f, position.y - height * 0.82f, 2.5f));
        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(playerModelScale));
        glm::mat4 playerModelRootTransform = translation * rotation * scaling;
        playerModel = new Model("Assets/character.gltf", playerModelRootTransform, isClient);
        playerModelCenter = glm::vec3(playerModel->rootModel[3][0], playerModel->rootModel[3][1], playerModel->rootModel[3][2]);
        walkingBackward = false;
        if (isClient) {
            playerModel->playAnimation(playerModel->animationClipList.at(0), 0.00f, walkingBackward); //puts the character in the default pose
        }
        //creating and initializing playerGunModel
        rotation = glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
        translation = glm::translate(glm::mat4(1.0f), glm::vec3(3.5f, 3.0f, 1.8f));
        scaling = glm::scale(glm::mat4(1.0f), glm::vec3(playerGunModelScale));
        glm::mat4 playerGunModelRootTransform = translation * rotation * scaling;
        playerGunModel = new Model("Assets/shotgunFire.gltf", playerGunModelRootTransform, isClient);
        playerGunModelCenter = glm::vec3(playerGunModel->rootModel[3][0] - 0.45f, playerGunModel->rootModel[3][1], playerGunModel->rootModel[3][2] + 1.0f);
        
        /*playerModel->rootModel = glm::mat4(1.0f);
        playerModelCenter = glm::vec3(playerModel->rootModel[3][0], playerModel->rootModel[3][1], playerModel->rootModel[3][2]);
        playerModel->animationRootModel = glm::mat4(1.0f);
        playerModel->playAnimation(playerModel->animationClipList.at(0), playerWalkingSpeed, true);*/
        //cout << "before play animation in constructor" << endl;
    //}

    //networking stuff
    moving = 0;
    hasFired = false;
    isFiring = false;
    inputDirections = new bool[6];
    pickUpAbilityKey = false;
    useAbilityKey = false;
    usingMapAbility = false;
    oldPitch = 0.0f;
    oldYaw = 0.0f;
    for (int i = forward; i <= down; i++)
    {
        inputDirections[i] = false;
    }
    cout << "Player created" << endl;
    lastFireTime = -1.0f;
    
}

void Player::createFootPrint(glm::vec3 footprintPos) {
    if (glm::distance(lastFootPrintPos, footprintPos) > 5.0f) {
        irrklang::vec3df position(footprintPos.x, footprintPos.y, footprintPos.z);
        irrklang::ISound* snd = soundEngine->play3D("footstep.mp3", position, false, true);
        Cube* footprint = new Cube(footprintPos - glm::vec3(1.0f, footprintPos.y, 0.5f), footprintPos - glm::vec3(0.0f, footprintPos.y - 0.01f, 0.0f), Cube::border, isClient);
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
                    glm::vec3(prevPosition.x + width * 0.5f, prevPosition.y + 0.25f, prevPosition.z + width * 0.5f), this, isClient);
                handleCollision(prevBoundingBox, b);
                boundingBox->update(glm::vec3(position.x - width * 0.5f, position.y - height * 0.75f, position.z - width * 0.5f),
                    glm::vec3(position.x + width * 0.5f, position.y + 0.25f, position.z + width * 0.5f));
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
void Player::update(float deltaTime, Game* game)
{
    if (!game->gameBegun)
    {
        return;
    }
    if (!isClient) { //update stuff for not client side player instances
        if (currentHealth <= 0.0f)
        {
            state = dead;
        }

        switch (state)
        {
        case crouch:
            if (position.y >= 1.5f)
            {
                glm::vec3 v = glm::vec3(0.0f, -1.0f, 0.0f) * speed / 2.0f;
                velocity += v;
            }
            else
            {
                //velocity.y = 0.0f;
            }
            velocity *= 0.5f;
            break;
        case sprint:
            velocity *= 1.40f;
            //velocity.y = 0.0f;
            break;
        case dead:
            velocity *= 3;
            break;
        case still:
            velocity *= 0.0f;
            break;
        default:
            if (position.y <= 4.0f)
            {
                glm::vec3 v = glm::vec3(0.0f, 1.0f, 0.0f) * speed / 2.0f;
                velocity += v;
                velocity *= 0.5f;
            }
            else
            {
                //velocity.y = 0;
            }
            break;
        }


        if (glm::length(velocity) > 0.0f) {
            prevPosition = position;
            position = position + velocity * deltaTime;

            //update player bounding box
            boundingBox->update(glm::vec3(position.x - width * 0.5f, position.y - height * 0.75f, position.z - width * 0.5f),
                glm::vec3(position.x + width * 0.5f, position.y + 0.25f, position.z + width * 0.5f));
        }

        if (boundingBox->getActive()) {
            applyConstraints(game -> allBoundingBoxes);
        }
        //if (state != dead && state != still)
        //{
        //    createFootPrint(position);
        //}
    }
    else
    {
        if (moving == 1) {
            playerModel->playAnimation(playerModel->animationClipList.at(0), playerWalkingSpeed, false);
        }
        if (moving == -1) {
            playerModel->playAnimation(playerModel->animationClipList.at(0), playerWalkingSpeed, true);
        }

        if (id != game->myPlayerId) {
            playerModel->playAnimation(playerModel->animationClipList.at(0), 0.0f, false);
        }

        if (usingMapAbility)
        {
            if (game->gameTime >= lastAbilityUseTime + 5.0f)
            {
                endMapAbility();
                string message = "endSeeMap," + to_string(id) + ",";
                game->addServerInputMessage(message);
            }
        }

        playerModel->update();
        playerGunModel->update();

        if (isFiring) {
            if (playerGunModel->animationClipList.at(0)->prevTime + 0.2f > playerGunModel->animationClipList.at(0)->duration) {
                isFiring = false;
            }
            playerGunModel->playAnimation(playerGunModel->animationClipList.at(0), 0.2f, false);
        }
    }

    // Both client and server should update
    if (!usingMapAbility)
    {
        playerCamera->setPosition(glm::vec3(position.x + 0.5f, position.y, position.z + 0.5f));
    }


    //update player camera
    playerCamera->Update();
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
        boundingBox->update(glm::vec3(position.x - width * 0.5f, position.y - height * 0.75f, position.z - width * 0.5f),
            glm::vec3(position.x + width * 0.5f, position.y + 0.25f, position.z + width * 0.5f));
        boundingBox->draw(viewProjMtx, shader);
    }

    playerModel->draw(viewProjMtx, shader);
    playerGunModel->draw(viewProjMtx, shader);
}

void Player::setInput(int dir, bool on)
{
    inputDirections[dir] = on;
}

void Player::resetInputDirections()
{
    for (int i = forward; i <= down; i++)
    {
        inputDirections[i] = false;
    }
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
    // DEBUG: take out true if not testing
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
void Player::setHasFired(bool val) 
{
    if (val)
    {
        cout << "firing shot:" << game->gameTime << "|" << (lastFireTime + playerWeapon->getDelayTime()) << endl;
        if (game -> gameTime >= lastFireTime + playerWeapon->getDelayTime())
        {
            cout << "setting true" << endl;
            hasFired = val;
            lastFireTime = game->gameTime;
            isFiring = true;
        }
        else
        {
            isFiring = false;
            hasFired = false;
        }
    }
    else
    {
        hasFired = val;
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
    BoundingBox* shotObject = shootWeapon(maze -> getChestBoundingBox(), false);
    if (shotObject == NULL)
    {
        cout << "could not find chest" << endl;
        return;
    }
    Model* chest = shotObject -> getParentModel();
    //std::cout << "LMAO" << parentCube->getType() << std::endl;
    //if (parentCube->getType() == Cube::abilityChest || true)
    //{
    int* playerPos = maze -> getCoordinates(getPosition());
    glm::vec3 chestLocation(chest->rootModel[3][0], chest->rootModel[3][1], chest->rootModel[3][2]);
    int* chestPos = maze -> getCoordinates(chestLocation);
    if ((playerPos[0] == chestPos[0] && playerPos[1] == chestPos[1]) || 
        (playerPos[0] == (chestPos[0] - 1) && playerPos[1] == chestPos[1]) || 
        (playerPos[0] == chestPos[0] && (playerPos[1] == chestPos[1] - 1)))
    {
        currentAbility = maze->getAbility(chestPos[0], chestPos[1]);
        maze->removeAbility(chestPos[0], chestPos[1]);
        chest->opening = true;
        string chestOpenMessage = "chestOpen," + to_string(chestPos[0]) + "," + to_string(chestPos[1]) + "\r\n";
        Window::messagesToServer.push_back(chestOpenMessage);
        std::cout << "Picked up ability: " << currentAbility << "|" << Player::getAbilityName(currentAbility) << std::endl;

        //delete parentCube;
        delete playerPos;
        delete chestPos;
    }
    //}
}

/*
 * Plays chest animation for client if they have opened a chest.
 */
void Player::openChest() {
    cout << "enter open chest" << endl;
    BoundingBox* shotObject = shootWeapon(maze->getChestBoundingBox(), false);
    if (shotObject == NULL)
    {
        cout << "chest found" << endl;
        return;
    }
    Model* chest = shotObject->getParentModel();
    //std::cout << "LMAO" << parentCube->getType() << std::endl;
    //if (parentCube->getType() == Cube::abilityChest || true)
    //{
    int* playerPos = maze->getCoordinates(getPosition());
    glm::vec3 chestLocation(chest->rootModel[3][0], chest->rootModel[3][1], chest->rootModel[3][2]);
    int* chestPos = maze->getCoordinates(chestLocation);
    if ((playerPos[0] == chestPos[0] && playerPos[1] == chestPos[1]) ||
        (playerPos[0] == (chestPos[0] - 1) && playerPos[1] == chestPos[1]) ||
        (playerPos[0] == chestPos[0] && (playerPos[1] == chestPos[1] - 1)))
    {
        chest->opening = true;
        delete playerPos;
        delete chestPos;
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
        setHealth(getHealth() + 50.0f);
        used = true;
        break;
    case armorPlayer:
        setArmor(getArmor() + 50.0f);
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


string Player::getAbilityName(int ability)
{
    string name = "";
    switch (ability)
    {
    case none:
        name = "None";
        break;
    case removeWall:
        name = "Remove Wall";
        break;
    case trackPlayer:
        name = "Track Player";
        break;
    case seeMap:
        name = "See Map";
        break;
    case healPlayer:
        name = "Heal Player";
        break;
    case increasePlayerHealth:
        name = "Increase Max Health";
        break;
    case armorPlayer:
        name = "Add Armor";
        break;
    case damageBoost:
        name = "Damage Boost";
        break;
    default:
        name = "None";
        break;
    }
    return name;
}


BoundingBox* Player::shootWeapon(std::vector<BoundingBox *> objects, bool playerShot) {
   
    if (state != sprint && state != dead)
    {
        BoundingBox* shotObject = playerWeapon->Shoot(objects, playerCamera->getPosition(), playerCamera->getDirection());
        if (shotObject && playerShot)
        {
            cout << "shotObject" << endl;
            if (shotObject->getParentPlayer())
            {
                cout << "shot Player" << endl;
                Player* shotPlayer = shotObject->getParentPlayer();
                float damage = playerWeapon -> getDamage();
                damage *= (1.0f + currentDamageBoost/100.0f);
                float armor = shotPlayer->getArmor();
                damage *= (10.0f - armor) / 10.f;
                shotPlayer->setHealth(shotPlayer->getHealth() - damage);
            }
        }
        return shotObject;
    }
    return 0;
}

bool Player::removeWallAbility()
{
    BoundingBox* shotObject = shootWeapon(maze->getBoundingBox(), false);
    if (shotObject == NULL)
    {
        return false;
    }
    Cube* parentCube = shotObject->getParentCube();
    if (parentCube != NULL && parentCube->isDeletable())
    {
        cout << "deleting wall" << endl;
        int * cubePos = maze -> getCoordinates(parentCube->getMazePosition());
        maze->setWall(cubePos[0], cubePos[1], parentCube->getDirection(), 0);
        return true;
    }
    return false;
}

bool Player::seeMapAbility()
{
    usingMapAbility = true;

    if (isClient)
    {
        float mazeSize = maze->getMazeSize();
        float mapScale = maze->getMapScale();
        float middleMap = (mazeSize - 1) * mapScale / 2.0f;
        playerCamera->Reset();
        playerCamera->setPitch(-90.0f);
        playerCamera->setPosition(glm::vec3(middleMap, 200.0f, middleMap));
        setState(still);
        playerCamera->setFarClip(350.0f);
    }
    else
    {
        string inputMessage = "useSeeMap," + to_string(id) + "," + to_string(oldPitch) + "," + to_string(oldYaw) + ",";
        cout << inputMessage << endl;
        game->addServerInputMessage(inputMessage);
        lastAbilityUseTime = game->gameTime;
    }
    return true;
}

bool Player::endMapAbility()
{
    usingMapAbility = false;
    if (isClient)
    {
        playerCamera->setPitch(oldPitch);
        playerCamera->setYaw(oldYaw);
        playerCamera->setFarClip(150.0f);
        setState(stand);
        usingMapAbility = false;
        playerCamera->Update();
    }

    return true;
}


//client side to pass to server
string Player::getPlayerInputString() {

    glm::vec3 direction = playerCamera->getCameraFront();
    float yaw = playerCamera->getYaw();
    float pitch = playerCamera->getPitch();
    playerInputString = "input," + to_string(id) + "," + to_string(moving) + "," +
        to_string(yaw) + "," + to_string(pitch) + ",";
    playerInputString += to_string(state) + ",";

    for (int i = forward; i <= down; i++)
    {
        playerInputString += to_string(inputDirections[i]) + ",";
    }
    playerInputString += to_string(hasFired) + "," + to_string(pickUpAbilityKey) + "," + to_string(useAbilityKey) + ",";
    playerInputString += to_string(playerModel->animationRootModelRotation);
    hasFired = false;
    pickUpAbilityKey = false;
    useAbilityKey = false;
    return playerInputString;
}

//server side to pass to client
string Player::getPlayerInfoString() {

    string MESSAGE_TAIL = "\r\n";
    playerInfoString = "player," + to_string(id) + "," + to_string(moving) + ",";
    playerInfoString += to_string(position.x) + "," + to_string(position.y) + "," + to_string(position.z) + ",";
    playerInfoString += to_string(velocity.x) + "," + to_string(velocity.y) + "," + to_string(velocity.z) + ",";

    playerInfoString += to_string(currentHealth) + "," + to_string(maxHealth) + "," + to_string(currentArmor) + "," +
        to_string(currentDamageBoost) + "," + to_string(currentAbility) + "," + to_string(hasFired) + ",";
    //if (playerModel) {
        playerInfoString += to_string(playerModel->animationRootModelRotation);
    //}

    playerInfoString += MESSAGE_TAIL;
    //cout << "moving in getPlayerInfo: " << moving << endl;
    return playerInfoString;
}

void Player::setPosition(glm::vec3 p) {

    glm::vec3 diff = p - position;
    playerModel->rootModel[3][0] += diff.x;
    playerModel->rootModel[3][1] += diff.y;
    playerModel->rootModel[3][2] += diff.z;
    playerGunModel->rootModel[3][0] += diff.x;
    playerGunModel->rootModel[3][1] += diff.y;
    playerGunModel->rootModel[3][2] += diff.z;
    playerGunModel->animationRootModel[3][0] += diff.x;
    playerGunModel->animationRootModel[3][1] += diff.y;
    playerGunModel->animationRootModel[3][2] += diff.z;
    playerGunModelCenter += diff;

    position = p;
}

////////////////////////////////////////////////////////////////////////////////