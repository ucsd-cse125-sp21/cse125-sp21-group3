#include "Opponent.h"
#include "Window.h"
#include <glm/gtx/string_cast.hpp>

Opponent::Opponent(int _id, glm::vec3 _position) {

    id = _id;
	position = _position;
    direction = glm::vec3(0.0f, 3.5, 0.0f - position.z);
    moving = 0;

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75f, position.y - height * 0.82f, 2.25f));
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(playerModelScale));
    glm::mat4 playerModelRootTransform = translation * rotation * scaling * glm::mat4(1.0f);
    playerModel = new Model("Assets/character.gltf", playerModelRootTransform, false);

    rotation = glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
    translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.7f, 3.0f, 2.25f));
    scaling = glm::scale(glm::mat4(1.0f), glm::vec3(playerGunModelScale));
    glm::mat4 playerGunModelRootTransform = translation * rotation * scaling;
    playerGunModel = new Model("Assets/shotgunFire.gltf", playerGunModelRootTransform, false);
    playerGunModelCenter = glm::vec3(playerGunModel->rootModel[3][0] - 0.45f, playerGunModel->rootModel[3][1], playerGunModel->rootModel[3][2] + 1.0f);

    /*playerModel->rootModel[3][0] = position.x;
    playerModel->rootModel[3][1] = position.y;
    playerModel->rootModel[3][2] = position.z;*/
    playerModelCenter = glm::vec3(playerModel->rootModel[3][0], playerModel->rootModel[3][1], playerModel->rootModel[3][2]);
    playerModel->playAnimation(playerModel->animationClipList.at(0), 0.00f, false);
}

Opponent::~Opponent() {

}

void Opponent::draw(const glm::mat4& viewProjMtx, GLuint shader) {
    if (Window::debugMode) {
        boundingBox->draw(viewProjMtx, shader);
    }

    playerModel->draw(viewProjMtx, shader);
    playerGunModel->draw(viewProjMtx, shader);
}

void Opponent::update() {

    //TODO change position and direction
  
    //handle animation
    if (moving == 1) {
        playerModel->playAnimation(playerModel->animationClipList.at(0), 0.3f, false);
    }
    if (moving == -1) {
        playerModel->playAnimation(playerModel->animationClipList.at(0), 0.3f, true);
    }
}