#include "Weapon.h"

Weapon::Weapon()
{
	damage = 10.0f;
}

BoundingBox* closestObject(glm::vec3 origin, glm::vec3 direction) {
	std::vector<BoundingBox*> boundingBoxList;

	BoundingBox* minBox;
	float mindist = -1;
	for (int i = 0; i < boundingBoxList.size(); i++) {
		float dist = RayIntersect(origin, direction, boundingBoxList[i]);
		if (dist == -1) {
			continue;
		}
		else if (mindist == -1 || dist < mindist) {
			mindist = dist;
			minBox = boundingBoxList[i];
		}
	}
	if (mindist == -1) {
		return NULL;
	}
	else {
		return minBox;
	}
}


float RayIntersect(glm::vec3 origin, glm::vec3 direction, BoundingBox * box) {

	glm::vec3 bmin = box->getMin();
	glm::vec3 bmax = box->getMax();
	float temp;
	float tmin = (bmin.x, origin.x) / direction.x;
	float tmax = (bmax.x - origin.x) / direction.x;

	if (tmin > tmax) {
		temp = tmin;
		tmin = tmax;
		tmax = temp;
	}

	float tymin = (bmin.y - origin.y) / direction.y;
	float tymax = (bmax.y - origin.y) / direction.y;

	if (tymin > tymax) {
		float temp = tymin;
		tymin = tymax;
		tymax = temp;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (bmin.z - origin.z) / direction.z;
	float tzmax = (bmax.z - origin.z) / direction.z;

	if (tzmin > tzmax) {
		temp = tzmin;
		tzmin = tzmax;
		tzmax = temp;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return -1;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return tmin;

}


void Weapon::Shoot(glm::vec3 origin, glm::vec3 direction)
{
	if (!reloading)
	{
		BoundingBox * minBox = closestObject(origin, direction);
	}
}

void Weapon::Reload()
{
	reloading = true;

	currentAmmo = maxAmmo;
	reloading = false;
}