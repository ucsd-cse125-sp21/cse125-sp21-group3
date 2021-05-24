#include "Camera.h"
#include "glm/gtx/euler_angles.hpp"
#include <glm/gtx/string_cast.hpp>

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

Camera::Camera(glm::vec3 _position) {
    Reset();
    position = _position;
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
  
    //cout << "camera position: " << glm::to_string(position) << endl;
    //constraining pitch
    if (pitch > 89.9f) {
        pitch = 89.9f;
    }
    if (pitch < -89.9f) {
        pitch = -89.9f;
    }

   
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    // Compute view matrix (inverse of world matrix), used to transform model from world space to view space
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
    NearClip = 0.01f;
    FarClip = 150.0f;

    yaw = -90.0f;
    pitch = 0.0f;
    position = glm::vec3(3.0f, 3.5f, 3.0f);
    cameraFront = glm::vec3(10.0f, 3.5, 10.0f - position.z);
}


/*
 * Sets the position of the camera to some vector p
 *
 * @param p Updated position for camera
 * @author Lucas Hwang
 */
void Camera::setPosition(glm::vec3 p) {

    position = p;
}

/*
 * Returns the position of the camera
 *
 * @return camera position
 * @author Lucas Hwang
 */
glm::vec3 Camera::getPosition() {

    return position;
}

////////////////////////////////////////////////////////////////////////////////
