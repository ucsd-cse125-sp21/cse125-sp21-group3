#pragma once

#ifndef WEAPON_H
#define WEAPON_H

#include "main.h"
#include "core.h"
#include "BoundingBox.h"
#include<vector>
#include <glm/glm.hpp>

class Weapon {
	public:
		Weapon();
		BoundingBox* Shoot(std::vector<BoundingBox*>, glm::vec3 origin, glm::vec3 direction);

		void Reload();

	private:
		float damage;
		float reloadTime;

		float maxAmmo;
		float currentAmmo;

		bool reloading = false;
};

#endif