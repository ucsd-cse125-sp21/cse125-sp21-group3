#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "main.h"
/*
 * File Name: Camera.h
 *
 * Contains the Camera class, which provides a simple means to controlling the
 * 3D camera. It could be extended to support more interactive controls.
 * Ultimately. the camera sets the GL projection and viewing matrices.
 *
 * @author Part of 169 starter code
 */
using namespace std;
class Camera {
public:
	Camera(glm::vec3 position);

	void Update();
	void Reset();

	// Access functions
	void SetAspect(float a) { Aspect = a; }

	void setYaw(float y) { yaw = y;  }
	void setPitch(float p) { pitch = p; }
	void setCameraFront(glm::vec3 cf) { cameraFront = cf;  }
	void setDirection(glm::vec3 d) { direction = d; }
	void setPosition(glm::vec3 p);
	void setFarClip(float clip) { FarClip = clip; }


	float getYaw() { return yaw; }
	float getPitch() { return pitch; }
	glm::vec3 getCameraFront() { return cameraFront; }
	glm::vec3 getDirection() { return direction; }
	glm::vec3 getPosition();

	const glm::mat4& GetViewProjectMtx() { return ViewProjectMtx; }


private:
	// Perspective controls
	float FOV;			// Field of View Angle (degrees)
	float Aspect;		// Aspect Ratio
	float NearClip;		// Near clipping plane distance
	float FarClip;		// Far clipping plane distance

	// Polar controls
	

	// Computed data
	glm::mat4 ViewProjectMtx;

	float yaw; //x rotation of the camera
	float pitch; //y rotation of the camera
	glm::vec3 cameraFront; //normalized camera direction
	glm::vec3 position;
	glm::vec3 direction; //direction the camera is facing
};

////////////////////////////////////////////////////////////////////////////////

#endif