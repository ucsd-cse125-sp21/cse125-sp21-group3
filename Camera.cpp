#include "Camera.h"
#include "glm/gtx/euler_angles.hpp"

/*
 * File Name: Camera.cpp
 *
 * This file contains the camera implementation. There are a number of parameters
 * as part of the Camera class which can be modified or changed here.
 *
 * @author Part of 169 starter code
 */

 ////////////////////////////////////////////////////////////////////////////////

/*
 * Default constructor for the camera.
 *
 * @return Camera object
 * @author Part of 169 starter code
 */
Camera::Camera() {
    Reset();
}

/*
 * Called each frame to update the camera's view. Calculates necessary
 * transformation matrices which are used in Window.cpp.  Any operations which need
 * to be applied to the camera every frame should occur in this method.
 * For example, moving, rotating, etc.
 *
 * @author Part of 169 starter code
 */
void Camera::Update() {
  
    //constraining pitch
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
   
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    // Compute view matrix (inverse of world matrix), used to transform model from world space to view space
    glm::vec3 position(world[3][0], world[3][1], world[3][2]);
    glm::mat4 view = glm::lookAt(
        position,
        position + cameraFront,
        glm::vec3(0.0f, 1.0f, 0.0f)
        );
    // Compute perspective projection matrix, used to project points in view space onto flat plane
    glm::mat4 project = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);

    // Compute final view-projection matrix, final positions which are used
    ViewProjectMtx = project * view;
}

/*
 * Initializes camera with some default values.
 *
 * @author Part of 169 starter code
 */
void Camera::Reset() {
    FOV = 45.0f;
    Aspect = 1.33f;
    NearClip = 0.1f;
    FarClip = 100.0f;

    yaw = -90.0f;
    pitch = 0.0f;
    cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    world = glm::mat4(1.0f);
    world[3][2] = -10.0f;
}

////////////////////////////////////////////////////////////////////////////////
