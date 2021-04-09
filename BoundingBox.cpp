#include "BoundingBox.h"
#include "glm/gtx/euler_angles.hpp"

/*
 * File Name: BoundingBox.cpp
 *
 * This file contains the BoundingBox implementation. This class will check
 * for collisions. Collision response will be handled by objects.
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
BoundingBox::BoundingBox(glm::vec3 boxMin, glm::vec3 boxMax) {
   
    min = boxMin;
    max = boxMax;
    active = true;
}


/*
 * Checks if the bounding box of this object is colliding with another
 * bounding box b.
 *
 * @param b Bounding box of the object we are checking for a collision with
 * @return Returns true if there is a collision and false if not
 * @author Lucas Hwang
 */
bool BoundingBox::checkCollision(BoundingBox* b) {

    //checking for collision with min point of b
    glm::vec3 a_min = min;
    glm::vec3 a_max = max;
    glm::vec3 b_min = b->getMin();
    glm::vec3 b_max = b->getMax();
    return (a_min.x <= b_max.x && a_max.x >= b_min.x) &&
        (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
        (a_min.z <= b_max.z && a_max.z >= b_min.z);
}

////////////////////////////////////////////////////////////////////////////////
