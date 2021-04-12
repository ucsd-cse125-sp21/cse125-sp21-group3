#include "Weapon.h"

Weapon::Weapon()
{
	damage = 10.0f;
}


float RayIntersectKensler(glm::vec3 origin, glm::vec3 direction, BoundingBox* box) {

	float tmin = 0.0f;
	float tmax = 10000.0f;

	glm::vec3 bmin = box->getMin();
	glm::vec3 bmax = box->getMax();

	for (int i = 0; i < 3; i++) {
		float invD = 1.0f / direction[i];
		float t0 = (bmin[i] - origin[i]) * invD;
		float t1 = (bmax[i] - origin[i]) * invD;

		if (invD < 0.0f) {
			float temp = t1;
			t1 = t0;
			t0 = temp;
		}

		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;

		if (tmax <= tmin)
			return -1;
	}
	return tmin;
}


BoundingBox* closestObject(std::vector<BoundingBox *> boundingBoxList, glm::vec3 origin, glm::vec3 direction) {
	
	BoundingBox* minBox;
	std::cerr << "Considering " << boundingBoxList.size() << "Objects" << std::endl;
	float mindist = -1;
	for (int i = 0; i < boundingBoxList.size(); i++) {
		float dist = RayIntersectKensler(origin, direction, boundingBoxList[i]);
		if (dist == -1) {
			continue;
		}
		else if (dist > 0 & (mindist == -1 || (dist < mindist))) {
			mindist = dist;
			std::cerr << "MinDist " << mindist << std::endl;
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





void Weapon::Shoot(std::vector<BoundingBox *> objects, glm::vec3 origin, glm::vec3 direction)
{

	std::cerr << "Origin " << origin.x << " " << origin.y << " " << origin.z << std::endl;
	std::cerr << "Origin " << direction.x << " " << direction.y << " " << direction.z << std::endl;


	std::cerr << "attempting fire" << std::endl;
	if (!reloading)
	{
		std::cerr << "not reloading" << std::endl;
		BoundingBox * minBox = closestObject(objects, origin, direction);
		if (minBox != NULL) {
			std::cerr << "Shot" << std::endl;
		}
	}
}

void Weapon::Reload()
{
	reloading = true;

	currentAmmo = maxAmmo;
	reloading = false;
}